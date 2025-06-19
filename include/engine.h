#ifndef ENGINE_H
#define ENGINE_H

#include "chess.h"

void initEngine();

void cleanupEngine();

Move getbestMove(int seconds, int* depth_searched);

#endif // ENGINE_H