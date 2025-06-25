#include "evaluate.h"
#include "chess.h"
#include "position_maps.h"
#include "masks.h"

extern Board board;
extern unsigned long long int repetition_history[1000];
extern int move_history_count;

extern int tables[16 * 64];
extern Bitboard passedPawnMask[128];

float combinedValues = 1.0f;

const int pieceValues[] = {
    100, 350, 350, 525, 1000, 0 // pawn, knight, bishop, rook, queen;
};

const int pastPawnBonus[] = {
    0, 100, 80, 60, 40, 20, 10, 0
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
    
    eval += calculatePawns(WHITE);
    eval -= calculatePawns(BLACK);
    
    return eval * (board.side_to_move == WHITE ? 1 : -1);
}

int calculatePawns(int color) {
    Bitboard pawns = board.bitboards[PAWN | color];
    Bitboard opponentPawns = board.bitboards[PAWN | OTHER_SIDE(color)];

    int index_offset = (color == WHITE) ? 0 : 64;

    int eval = 0;

    Bitboard bb = pawns;
    while (bb) {
        int square = __builtin_ctzll(bb);
        
        Bitboard passedMask = passedPawnMask[square + index_offset];

        if (!(opponentPawns & passedMask)) {
            // passed pawn
            int ranksFromPromotion = (color == WHITE) ? (7 - square / 8) : (square / 8);
            eval += pastPawnBonus[ranksFromPromotion];
        }

        bb &= bb - 1;
    }

    return eval;
}

void initCombinedValues() {
    combinedValues = pieceValues[KNIGHT] * 4 + 
                     pieceValues[BISHOP] * 4 + 
                     pieceValues[ROOK] * 4 + 
                     pieceValues[QUEEN] * 2;
}