#ifndef ENGINE_H
#define ENGINE_H

#include "chess.h"

void initEngine();

void cleanupEngine();

Move getbestMove(int seconds);

double minMax (int depth);

double evaluatePosition();

#endif // ENGINE_H