#ifndef ENGINE_H
#define ENGINE_H

#include "chess.h"

void initEngine();

void cleanupEngine();

Move getbestMove(int milli_seconds, int* depth_searched, int* eval);

#endif // ENGINE_H