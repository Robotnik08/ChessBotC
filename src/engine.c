#include "engine.h"
#include "chess.h"
#include "search_move.h"
#include "threads.h"
#include "position_maps.h"

#include <time.h>
#include <math.h>
#include <float.h>

void initEngine() {
    initPositionMaps();
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
        int eval = findBestMove(depth, &move);
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
    int eval = findBestMove(depth, &move);
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
        *depth_searched = depth - 1; // Return the last depth searched
    }

    return best_move_so_far;
}