#include "evaluate.h"
#include "chess.h"
#include "position_maps.h"
#include "masks.h"

extern Board board;
extern unsigned long long int repetition_history[1000];
extern int move_history_count;

extern int tables[16 * 64];
extern int fromEdge[64];
extern Bitboard passedPawnMask[128];

float combinedValues = 1.0f;

const int pieceValues[] = {
    100, 350, 350, 525, 1000, 0 // pawn, knight, bishop, rook, queen;
};

const int pastPawnBonus[] = {
    0, 100, 80, 60, 40, 20, 10, 0
};

#define KING_SAFETY_BONUS 12

const Bitboard kingSafetyMasks[4] = {
    0x70707ULL, // white king queen side
    0xe0e0e0ULL, // white king king side
    0x707070000000000ULL, // black king queen side
    0xe0e0e00000000000ULL // black king king side
};

int evaluatePosition() {
    int eval = 0;

    int points = 0;
    // get points for each piece
    for (int i = KNIGHT; i < KING; i++) {
        Bitboard whiteBoard = board.bitboards[i | WHITE];
        Bitboard blackBoard = board.bitboards[i | BLACK];

        if (whiteBoard) {
            points += pieceValues[i] * __builtin_popcountll(whiteBoard);
        }
        if (blackBoard) {
            points += pieceValues[i] * __builtin_popcountll(blackBoard);
        }
    }

    float endgameWeight = 1.0f - ((float)points / combinedValues);
    float reverseEndgameWeight = 1.0f - endgameWeight;
    for (int i = KNIGHT; i <= KING; i++) {
        Bitboard whiteBoard = board.bitboards[i | WHITE];
        Bitboard blackBoard = board.bitboards[i | BLACK];

        if (i == KING) {

            int white_square = __builtin_ctzll(whiteBoard);
            int black_square = __builtin_ctzll(blackBoard);

            eval += reverseEndgameWeight * (tables[IDX(KING_MIDGAME, WHITE, white_square)]);
            eval += endgameWeight * (tables[IDX(KING_ENDGAME, WHITE, white_square)]);
            eval += endgameWeight * (fromEdge[white_square] * 10);

            eval -= reverseEndgameWeight * (tables[IDX(KING_MIDGAME, BLACK, black_square)]);
            eval -= endgameWeight * (tables[IDX(KING_ENDGAME, BLACK, black_square)]);
            eval -= endgameWeight * (fromEdge[black_square] * 10);

        } else {
            if (whiteBoard) {
                Bitboard wb = whiteBoard;
                while (wb) {
                    int square = __builtin_ctzll(wb);
                    eval += reverseEndgameWeight * tables[IDX(i, WHITE, square)];
                    eval += pieceValues[i];
                    wb &= wb - 1;
                }
            }
            if (blackBoard) {
                Bitboard bb = blackBoard;
                while (bb) {
                    int square = __builtin_ctzll(bb);
                    eval -= reverseEndgameWeight * tables[IDX(i, BLACK, square)];
                    eval -= pieceValues[i];
                    bb &= bb - 1;
                }
            }
        }
    }

    // pawn is like the king, it has an endgame value
    Bitboard whitePawns = board.bitboards[PAWN | WHITE];
    Bitboard blackPawns = board.bitboards[PAWN | BLACK];
    if (whitePawns) {
        Bitboard wp = whitePawns;
        while (wp) {
            int square = __builtin_ctzll(wp);
            eval += reverseEndgameWeight * (tables[IDX(PAWN_MIDGAME, WHITE, square)]);
            eval += endgameWeight * (tables[IDX(PAWN_ENDGAME, WHITE, square)]);
            eval += pieceValues[PAWN];
            wp &= wp - 1;
        }
    }
    if (blackPawns) {
        Bitboard bp = blackPawns;
        while (bp) {
            int square = __builtin_ctzll(bp);
            eval -= reverseEndgameWeight * (tables[IDX(PAWN_MIDGAME, BLACK, square)]);
            eval -= endgameWeight * (tables[IDX(PAWN_ENDGAME, BLACK, square)]);
            eval -= pieceValues[PAWN];
            bp &= bp - 1;
        }
    }
    
    eval += calculatePawns(WHITE);
    eval -= calculatePawns(BLACK);

    // king safety (matters more in the beginning and middle game)
    eval += reverseEndgameWeight * calculateKingSafety(WHITE);
    eval -= reverseEndgameWeight * calculateKingSafety(BLACK);
    
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

extern Bitboard getSlidingAttacks(int square, Bitboard blockMap, int ortho);

int calculateKingSafety(int color) {
    Bitboard king = board.bitboards[KING | color];
    if (!king) return 0; // no king found, should not happen

    int square = __builtin_ctzll(king);
    int activeMap = -1;
    if (color == WHITE) {
        if (king & kingSafetyMasks[0]) {
            activeMap = 0; // white king queen side
        } else if (king & kingSafetyMasks[1]) {
            activeMap = 1; // white king king side
        }
    } else {
        if (king & kingSafetyMasks[2]) {
            activeMap = 2; // black king queen side
        } else if (king & kingSafetyMasks[3]) {
            activeMap = 3; // black king king side
        }
    }

    if (activeMap == -1) return -30; // King not in safety area, return a large penalty

    int eval = 0;
    Bitboard activeMapMask = kingSafetyMasks[activeMap];
    
    // give a bonus for each friendly bishop or knight on the active map (a queen is better active)
    Bitboard bonusPieces = board.bitboards[(BISHOP | color)] | board.bitboards[(KNIGHT | color)];
    Bitboard friendlyMap = bonusPieces & activeMapMask;
    while (friendlyMap) {
        int pieceSquare = __builtin_ctzll(friendlyMap);
        eval += KING_SAFETY_BONUS;
        friendlyMap &= friendlyMap - 1;
    }

    int opponentPieces = getFriendly(OTHER_SIDE(color)) & activeMapMask;
    if (opponentPieces) {
        // penalize if there are opponent pieces on the active map
        eval -= KING_SAFETY_BONUS * 5 * __builtin_popcountll(opponentPieces);
    }

    // give a bonus for each friendly pawn on the active map (max 3 pawns)
    Bitboard friendlyPawns = board.bitboards[(PAWN | color)] & activeMapMask;
    int pawnCount = 0;
    while (friendlyPawns) {
        int pieceSquare = __builtin_ctzll(friendlyPawns);
        pawnCount++;
        friendlyPawns &= friendlyPawns - 1;
    }
    if (pawnCount == 3) { // exactly 3 pawns allows for a strong pawn shield, more pawns are stacked pawns and less is not a good place for the king to castle into
        eval += KING_SAFETY_BONUS * 3;
    }

    // generate queen vision from the king's square
    // Bitboard blockers = getFriendly(color) | getFriendly(OTHER_SIDE(color));
    // Bitboard queenVision = (getSlidingAttacks(square, blockers, true) | getSlidingAttacks(square, blockers, false)) & ~blockers;

    // // give a small penalty for each square in queen vision
    // eval -= 3 * __builtin_popcountll(queenVision);

    return eval;
}

void initCombinedValues() {
    combinedValues = pieceValues[KNIGHT] * 4 + 
                     pieceValues[BISHOP] * 4 + 
                     pieceValues[ROOK] * 4 + 
                     pieceValues[QUEEN] * 2;
}