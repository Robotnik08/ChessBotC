#ifndef POSITION_MAPS_H
#define POSITION_MAPS_H

#include "chess.h"

#define IDX(piece, color, square) ((piece | color) * 64 + square)

#define KING_MIDGAME (KING)
#define KING_ENDGAME (KING + 1)

#define PAWN_MIDGAME (PAWN)
#define PAWN_ENDGAME (KING + 2)

void initPositionMaps();

#endif