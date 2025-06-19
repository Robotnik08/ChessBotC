#ifndef SEARCH_H
#define SEARCH_H

#include "chess.h"

#define IS_MATE(score) (abs(score) >= MATE_SCORE - 100)

#define POSITIVE_INFINITY 10000000
#define NEGATIVE_INFINITY -POSITIVE_INFINITY
#define MATE_SCORE 1000000 // Mate score is 10 times smaller than the maximum score to avoid confusion with normal evaluations

int findBestMove(int depth, Move* best_move);

int search(int depth, int color, int alpha, int beta);

int extendedSearch (int color, int beta, int alpha);

#endif // SEARCH_H