#include "engine.h"
#include "chess.h"
#include "search_move.h"
#include "threads.h"
#include "position_maps.h"
#include "evaluate.h"
#include "transposition_table.h"
#include "opening_book.h"
#include "masks.h"

#include <time.h>
#include <math.h>
#include <float.h>
#include <stdint.h>

extern Board board;

bool inBook = false;

void initEngine() {
    initPositionMaps();
    initCombinedValues();
    initTranspositionTable();
    initOpeningBook();
    initMasks();
}

void cleanupEngine(){
    // cleanup
}

volatile bool stop_search = false;
Move best_move_so_far = NULL_MOVE;
int depth = 2;
int best_move_index = -1;

int highest_eval;

ThreadReturn THREAD_CALLCONV search_thread_fn(void* arg) {
    depth = 2;

    while (!stop_search) {
        Move move = NULL_MOVE;
        int eval = findBestMove(depth, &move, best_move_index);
        if (stop_search) {
            if (move != NULL_MOVE) {
                // don't discard the best move found so far, since if it's the same as the last one, no harm is done, but if it found a better move, we want to keep it
                best_move_so_far = move;
            }
            break;
        }
        highest_eval = eval;
        if (IS_MATE(eval)) {
            best_move_so_far = move;
            stop_search = true;

            clearTranspositionTable(); // the transposition table breaks when a mate is found, don't know why yet
            break;
        }
        Move moves[MAX_MOVES];
        int num_moves = generateMoves(moves, false);
        for (int i = 0; i < num_moves; i++) {
            if (moves[i] == move) {
                best_move_index = i;
                break;
            }
        }
        best_move_so_far = move;
        depth++;
    }

    return 0;
}

Move getbestMove(int milli_seconds, int* depth_searched, int* eval) {
    Thread thread;
    stop_search = false;
    best_move_so_far = NULL_MOVE;

    // check book if book move is found
    if (inBook) {
        char old_en_passant_file = board.en_passant_file;

        uint64_t hash = getZobristHash();

        Move book_move = getBookMove(hash, time(NULL));
        if (book_move != NULL_MOVE) {
            best_move_so_far = book_move;
            best_move_index = -1; // no index in the move list, since it's a book move
            if (depth_searched) {
                *depth_searched = 0; // no depth searched for book moves
            }
            if (eval) {
                *eval = 0; // no evaluation for book moves
            }
            return best_move_so_far;
        }

        inBook = false; // disable book after the first move non book move is found
    }

    depth = 1;
    
    best_move_index = -1;
    Move move = NULL_MOVE;
    int depth1_eval = findBestMove(depth, &move, best_move_index);
    if (IS_MATE(depth1_eval)) {
        stop_search = true;
        clearTranspositionTable(); // the transposition table breaks when a mate is found, don't know why yet
    }
    best_move_so_far = move;
    highest_eval = depth1_eval;

    Move moves[MAX_MOVES];
    int num_moves = generateMoves(moves, false);
    for (int i = 0; i < num_moves; i++) {
        if (moves[i] == move) {
            best_move_index = i;
            break;
        }
    }


    START_THREAD(&thread, search_thread_fn, NULL);

#if defined(_WIN32) || defined(_WIN64)
    // Windows: wait up to `seconds`, but return early if thread ends
    DWORD wait_result = WaitForSingleObject(thread, milli_seconds);
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
        *depth_searched = depth - 1; // Return the last depth searched
    }
    if (eval) {
        *eval = highest_eval; // Return the evaluation of the last move
    }

    return best_move_so_far;
}