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
    for (int i = KNIGHT; i <= KING; i++) {
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
                    int square = __builtin_ctzll(wb);
                    eval += tables[IDX(i, WHITE, square)];
                    wb &= wb - 1;
                    
                    if (i != PAWN) {
                        points += pieceValues[i];
                    }
                }
            }
            if (blackBoard) {
                Bitboard bb = blackBoard;
                while (bb) {
                    int square = __builtin_ctzll(bb);
                    eval -= tables[IDX(i, BLACK, square)];
                    bb &= bb - 1;
                    
                    if (i != PAWN) {
                        points += pieceValues[i];
                    }
                }
            }
        }
    }

    // pawn is like the king, it has an endgame value
    Bitboard whitePawns = board.bitboards[PAWN | WHITE];
    Bitboard blackPawns = board.bitboards[PAWN | BLACK];
    float endgameWeight = 1.0f - ((float)points / combinedValues);
    if (whitePawns) {
        Bitboard wp = whitePawns;
        while (wp) {
            int square = __builtin_ctzll(wp);
            eval += (1.0f - endgameWeight) * (tables[IDX(PAWN_MIDGAME, WHITE, square)]);
            eval += endgameWeight * (tables[IDX(PAWN_ENDGAME, WHITE, square)]);
            wp &= wp - 1;
        }
    }
    if (blackPawns) {
        Bitboard bp = blackPawns;
        while (bp) {
            int square = __builtin_ctzll(bp);
            eval -= (1.0f - endgameWeight) * (tables[IDX(PAWN_MIDGAME, BLACK, square)]);
            eval -= endgameWeight * (tables[IDX(PAWN_ENDGAME, BLACK, square)]);
            bp &= bp - 1;
        }
    }
    
    return eval * (board.side_to_move == WHITE ? 1 : -1);
}

void initCombinedValues() {
    combinedValues = pieceValues[KNIGHT] * 4 + 
                     pieceValues[BISHOP] * 4 + 
                     pieceValues[ROOK] * 4 + 
                     pieceValues[QUEEN] * 2;
}