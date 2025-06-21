#include "evaluate.h"
#include "chess.h"
#include "position_maps.h"

extern Board board;
extern unsigned long long int repetition_history[1000];
extern int move_history_count;

extern int tables[16 * 64];

float combinedValues = 1.0f;

const int pieceValues[] = {
    100, 350, 350, 525, 1000, 0 // pawn, knight, bishop, rook, queen;
};

int evaluatePosition() {
    int eval = 0;

    int points = 0;
    for (int i = PAWN; i <= KING; i++) {
        Bitboard whiteBoard = board.bitboards[i | WHITE];
        Bitboard blackBoard = board.bitboards[i | BLACK];

        if (i == KING) {
            float endgameWeight = 1.0f - ((float)points / combinedValues);

            int white_square = __builtin_ctzll(whiteBoard);
            int black_square = __builtin_ctzll(blackBoard);

            eval += (1.0f - endgameWeight) * (tables[IDX(KING_MIDGAME, WHITE, white_square)]);
            eval += endgameWeight * (tables[IDX(KING_ENDGAME, WHITE, white_square)]);

            eval -= (1.0f - endgameWeight) * (tables[IDX(KING_MIDGAME, BLACK, black_square)]);
            eval -= endgameWeight * (tables[IDX(KING_ENDGAME, BLACK, black_square)]);
        } else {
            if (whiteBoard) {
                Bitboard wb = whiteBoard;
                while (wb) {
                    int square = __builtin_ctzll(wb); // get the index of the least significant bit
                    eval += tables[IDX(i, WHITE, square)]; // add positional value for white pieces
                    wb &= wb - 1; // clear the least significant bit
                    
                    if (i != PAWN) {
                        points += pieceValues[i];
                    }
                }
            }
            if (blackBoard) {
                Bitboard bb = blackBoard;
                while (bb) {
                    int square = __builtin_ctzll(bb); // get the index of the least significant bit
                    eval -= tables[IDX(i, BLACK, square)]; // subtract positional value for black pieces
                    bb &= bb - 1; // clear the least significant bit
                    
                    if (i != PAWN) {
                        points += pieceValues[i];
                    }
                }
            }
        }
    }
    
    return eval;
}

void initCombinedValues() {
    combinedValues = pieceValues[KNIGHT] * 4 + 
                     pieceValues[BISHOP] * 4 + 
                     pieceValues[ROOK] * 4 + 
                     pieceValues[QUEEN] * 2;
}