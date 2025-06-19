#include "evaluate.h"
#include "chess.h"
#include "position_maps.h"

extern Board board;
extern unsigned long long int repetition_history[1000];
extern int move_history_count;

extern int tables[16 * 64];

const int pieceValues[] = {
    100, 350, 350, 525, 1000, 0 // pawn, knight, bishop, rook, queen;
};

int evaluatePosition() {
    int eval = 0;
    for (int i = PAWN; i <= KING; i++) {
        Bitboard whiteBoard = board.bitboards[i | WHITE];
        Bitboard blackBoard = board.bitboards[i | BLACK];

        if (i == KING) {
            
        } else {
            if (whiteBoard) {
                Bitboard wb = whiteBoard;
                while (wb) {
                    int square = __builtin_ctzll(wb); // get the index of the least significant bit
                    eval += tables[IDX(i, WHITE, square)]; // add positional value for white pieces
                    wb &= wb - 1; // clear the least significant bit
                }
            }
            if (blackBoard) {
                Bitboard bb = blackBoard;
                while (bb) {
                    int square = __builtin_ctzll(bb); // get the index of the least significant bit
                    eval -= tables[IDX(i, BLACK, square)]; // subtract positional value for black pieces
                    bb &= bb - 1; // clear the least significant bit
                }
            }
        }
    }
    
    return eval;
}