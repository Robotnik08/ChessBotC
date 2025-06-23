#ifndef SEARCH_H
#define SEARCH_H

#include "chess.h"

#define POSITIVE_INFINITY 10000000
#define NEGATIVE_INFINITY -POSITIVE_INFINITY
#define MATE_SCORE 1000000
#define MAX_PLY 1000

#define IS_MATE(score) (abs(score) >= MATE_SCORE - MAX_PLY)

int findBestMove(int depth, Move* best_move, int best_move_index);

int search(int depth, int ply_from_root, int alpha, int beta);

int extendedSearch (int beta, int alpha);

#endif // SEARCH_H