#include "engine.h"
#include "chess.h"

#include <time.h>
#include <math.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

extern Board board;
extern unsigned long long int repetition_history[1000];
extern int move_history_count;

void initEngine() {
    // init
    srand((unsigned int)time(NULL));
}

void cleanupEngine(){
    // cleanup
}

Move getbestMove(int seconds) {
    Move moves[MAX_MOVES];
    int num_moves = generateMoves(moves);
    Move selectedMove = NULL_MOVE;

    bool side_to_move = board.side_to_move;
    
    double bestScore = side_to_move == WHITE ? -INFINITY : INFINITY;
    for (int i = 0; i < num_moves; i++) {
        makeMove(moves[i]);
        double eval = minMax(2);

        if (side_to_move == WHITE ? eval > bestScore : eval < bestScore) {
            bestScore = eval;
            selectedMove = moves[i];
        }
        
        unmakeMove(moves[i]);
    }
    
    return selectedMove;
}

double minMax (int depth) {
    if (depth < 0) {
        return evaluatePosition();
    }

    Move moves[MAX_MOVES];
    int num_moves = generateMoves(moves);
    bool side_to_move = board.side_to_move;

    double score = side_to_move == WHITE ? -INFINITY : INFINITY;

    if (num_moves > 0) {
        for (int i = 0; i < num_moves; i++) {
            makeMove(moves[i]);
            double eval = minMax(depth - 1);
            score = side_to_move == WHITE ? max(score, eval) : min(score, eval);
            unmakeMove(moves[i]);
        }
    } else if (board.state != CHECKMATE) {
        return 0;
    }

    return score;
}

double evaluatePosition() {

    return 0;
}