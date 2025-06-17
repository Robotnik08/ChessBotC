#ifndef ENGINE_H
#define ENGINE_H

#include "chess.h"

void initEngine();

void cleanupEngine();

Move getbestMove(int seconds, int* depth_searched);

double findBestMove(int depth, Move* best_move);

double search(int depth, int color, double alpha, double beta);

double evaluatePosition();

#endif // ENGINE_H