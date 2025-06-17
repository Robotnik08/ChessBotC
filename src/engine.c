#include "engine.h"
#include "chess.h"
#include "threads.h"

#include <time.h>
#include <math.h>
#include <float.h>

#define MATE_SCORE 1000000.0
#define IS_MATE(score) (fabs(score) >= MATE_SCORE - 100)

extern Board board;
extern unsigned long long int repetition_history[1000];
extern int move_history_count;


const double pieceValues[] = {
    100, 350, 350, 525, 1000, 10, // pawn, knight, bishop, rook, queen, king
    0, 0,
    -100, -350, -350, -525, -1000, -10, // pawn, knight, bishop, rook, queen, king
    0, 0
};

void initEngine() {
    // init
}

void cleanupEngine(){
    // cleanup
}

volatile bool stop_search = false;
Move best_move_so_far = NULL_MOVE;
int depth = 1;

ThreadReturn THREAD_CALLCONV search_thread_fn(void* arg) {
    depth = 1;

    while (!stop_search) {
        Move move = NULL_MOVE;
        double eval = findBestMove(depth, &move);
        if (stop_search) break;
        if (IS_MATE(eval)) {
            best_move_so_far = move;
            stop_search = true;
            break;
        }
        best_move_so_far = move;
        depth++;
    }

    return 0;
}

Move getbestMove(int seconds, int* depth_searched) {
    Thread thread;
    stop_search = false;
    best_move_so_far = NULL_MOVE;

    START_THREAD(&thread, search_thread_fn, NULL);

#if defined(_WIN32) || defined(_WIN64)
    // Windows: wait up to `seconds`, but return early if thread ends
    DWORD wait_result = WaitForSingleObject(thread, seconds * 1000);
    if (wait_result == WAIT_TIMEOUT) {
        stop_search = true;
        WaitForSingleObject(thread, INFINITE);  // ensure cleanup
    }
    CloseHandle(thread);

#else
    // POSIX: check time repeatedly
    time_t start_time = time(NULL);
    while (difftime(time(NULL), start_time) < seconds) {
        int status = pthread_tryjoin_np(thread, NULL);  // GNU extension
        if (status == 0) {
            // Thread has finished
            return best_move_so_far;
        }
        usleep(10000);  // Sleep 10ms to avoid busy-waiting
    }
    stop_search = true;
    pthread_join(thread, NULL);
#endif

    if (depth_searched) {
        *depth_searched = depth;
    }

    return best_move_so_far;
}

double findBestMove(int depth, Move* best_move) {
    Move moves[MAX_MOVES];
    int num_moves = generateMoves(moves);
    Move selectedMove = NULL_MOVE;

    bool side_to_move = board.side_to_move;
    
    double alpha = -INFINITY;
    double beta = INFINITY;
    double bestScore = -INFINITY;

    for (int i = 0; i < num_moves; i++) {
        makeMove(moves[i]);
        double eval = -search(depth - 1, (side_to_move == WHITE ? -1 : 1), -INFINITY, INFINITY);
        unmakeMove(moves[i]);

        if (eval > bestScore) {
            bestScore = eval;
            selectedMove = moves[i];
        }

        if (eval > alpha) {
            alpha = eval;
        }

        if (alpha >= beta) {
            break;
        }        
    }

    if (best_move) {
        *best_move = selectedMove;
    }
    
    return bestScore;
}

double search(int depth, int color, double alpha, double beta) {
    if (stop_search) {
        return 0.0; // Search was stopped
    }

    if (depth == 0) {
        return color * evaluatePosition();
    }

    Move moves[MAX_MOVES];
    int num_moves = generateMoves(moves);

    if (board.state == CHECKMATE) {
        return -MATE_SCORE + depth;
    } else if (board.state != NONE) {
        return 0.0;
    }

    double maxEval = -INFINITY;

    for (int i = 0; i < num_moves; i++) {
        makeMove(moves[i]);
        double eval = -search(depth - 1, -color, -beta, -alpha);
        unmakeMove(moves[i]);
        if (eval > maxEval) {
            maxEval = eval;
        }
        if (maxEval > alpha) {
            alpha = maxEval;
        }
        if (alpha >= beta) {
            break;
        }
    }

    return maxEval;
}

double evaluatePosition() {
    double eval = 0.0;
    for (int i = 0; i < BB_MAXVAL; i++) {
        eval += countBits(board.bitboards[i]) * pieceValues[i];
    }
    
    return eval;
}