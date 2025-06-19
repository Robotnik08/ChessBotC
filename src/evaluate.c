#include "evaluate.h"
#include "chess.h"

extern Board board;
extern unsigned long long int repetition_history[1000];
extern int move_history_count;

const int pieceValues[] = {
    100, 350, 350, 525, 1000, 0 // pawn, knight, bishop, rook, queen;
};

int evaluatePosition() {
    int eval = 0;
    for (int i = PAWN; i < KING; i++) {
        eval += countBits(board.bitboards[i + WHITE]) * pieceValues[i];
        eval -= countBits(board.bitboards[i + BLACK]) * pieceValues[i];
    }
    
    return eval;
}