#ifndef EVALUATE_H
#define EVALUATE_H

#include "chess.h"

int evaluatePosition();

int calculatePawns(int color);

void initCombinedValues();

int calculateKingSafety(int color);

#endif // EVALUATE_H