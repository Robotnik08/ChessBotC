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
    100, 350, 350, 525, 1000 // pawn, knight, bishop, rook, queen;
};

void initEngine() {
    // init
}

void cleanupEngine(){
    // cleanup
}

volatile bool stop_search = false;
Move best_move_so_far = NULL_MOVE;
int depth = 2;

ThreadReturn THREAD_CALLCONV search_thread_fn(void* arg) {
    depth = 2;

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

    depth = 1;
    
    Move move = NULL_MOVE;
    double eval = findBestMove(depth, &move);
    if (IS_MATE(eval)) {
        stop_search = true;
    }
    best_move_so_far = move;

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
    int num_moves = generateMoves(moves, false);
    Move selectedMove = NULL_MOVE;

    bool side_to_move = board.side_to_move;
    
    double alpha = -INFINITY;
    double beta = INFINITY;

    for (int i = 0; i < num_moves; i++) {
        makeMove(moves[i]);
        double eval = -search(depth - 1, (side_to_move == WHITE ? -1 : 1), -beta, -alpha);
        unmakeMove(moves[i]);

        if (stop_search) {
            return 0.0;
        }

        if (eval > alpha) {
            alpha = eval;
            selectedMove = moves[i];
        }
        if (alpha >= beta) {
            break;
        }
    }

    if (best_move) {
        *best_move = selectedMove;
    }

    return alpha;
}

double search(int depth, int color, double alpha, double beta) {
    if (stop_search) {
        return 0.0;
    }

    if (depth == 0) {
        return extendedSearch(color, beta, alpha);
    }

    Move moves[MAX_MOVES];
    int num_moves = generateMoves(moves, false);

    if (board.state == CHECKMATE) {
        return -MATE_SCORE + depth;
    } else if (board.state != NONE) {
        return 0.0;
    }

    for (int i = 0; i < num_moves; i++) {
        makeMove(moves[i]);
        double eval = -search(depth - 1, -color, -beta, -alpha);
        unmakeMove(moves[i]);

        if (eval >= beta) {
            return beta;
        }
        if (eval > alpha) {
            alpha = eval;
        }
    }

    return alpha;
}

// only searches captures
double extendedSearch (int color, double beta, double alpha) {
    if (stop_search) {
        return 0.0;
    }

    double eval = color * evaluatePosition();
    if (eval >= beta) {
        return beta;
    }
    if (eval > alpha) {
        alpha = eval;
    }

    Move moves[MAX_MOVES];
    int num_moves = generateMoves(moves, true);

    for (int i = 0; i < num_moves; i++) {
        makeMove(moves[i]);
        double score = -extendedSearch(-color, -alpha, -beta);
        unmakeMove(moves[i]);

        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}

double evaluatePosition() {
    double eval = 0.0;
    for (int i = PAWN; i <= KING; i++) {
        eval += countBits(board.bitboards[i + WHITE]) * pieceValues[i];
        eval -= countBits(board.bitboards[i + BLACK]) * pieceValues[i];
    }
    
    return eval;
}