#ifndef ORDER_H
#define ORDER_H

#include "chess.h"

#define EN_PASSANT_VALUE 900
#define ATTACKED_PENALTY 350

int getMoveScore (Move move, Bitboard attacked_map);

void getScores (Move* move_list, int* score_list, int num_moves, Bitboard attacked_map);

void swapNextBestMove (Move* move_list, int* score_list, int num_moves, int index);

#endif // ORDER_H