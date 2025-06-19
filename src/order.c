#include "order.h"
#include "chess.h"

extern Board board;
extern int pieceValues[6]; // pawn, knight, bishop, rook, queen, king

// MVV-LVA scoring
int getMoveScore (Move move, Bitboard attacked_map) {
    int score = 0;

    int from = FROM(move);
    int to = TO(move);
    int extra = EXTRA(move);

    int piece_from = TYPE(board.bitboards[getFromLocation(from)]);
    int piece_to = TYPE(board.bitboards[getFromLocation(to)]);

    if (piece_from == PAWN) {
        switch (extra) {
            case EN_PASSANT:
                score += EN_PASSANT_VALUE;
                break;
            case PROMOTION_QUEEN:
                score += pieceValues[QUEEN];
                break;
            case PROMOTION_ROOK:
                score += pieceValues[ROOK];
                break;
            case PROMOTION_BISHOP:
                score += pieceValues[BISHOP];
                break;
            case PROMOTION_KNIGHT:
                score += pieceValues[KNIGHT];
                break;
            default:
                break;
        }
    } else if (to & attacked_map) {
        score += ATTACKED_PENALTY; // penalize moves to attacked squares
    }
    

    if (piece_to != 0) {
        int value_from = pieceValues[piece_from];
        int value_to = pieceValues[piece_to];

        score += 10 * value_to - value_from;
    }

    return score;
}

void getScores (Move* move_list, int* score_list, int num_moves, Bitboard attacked_map) {
    for (int i = 0; i < num_moves; i++) {
        score_list[i] = getMoveScore(move_list[i], attacked_map);
    }
}

void swapNextBestMove (Move* move_list, int* score_list, int num_moves, int index) {
    int best_score = score_list[index];
    int best_index = index;

    for (int i = index + 1; i < num_moves; i++) {
        if (score_list[i] > best_score) {
            best_score = score_list[i];
            best_index = i;
        }
    }

    if (best_index != index) {
        // swap the moves
        Move temp_move = move_list[index];
        move_list[index] = move_list[best_index];
        move_list[best_index] = temp_move;

        // swap the scores
        int temp_score = score_list[index];
        score_list[index] = score_list[best_index];
        score_list[best_index] = temp_score;
    }
}