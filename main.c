#include <stdio.h>
#include "chess/chess.h"

int main() {
    init();

    parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    Move move_list[MAX_MOVES];

    int move_count = generateMoves(move_list);

    printf("Generated %d moves\n", move_count);

    return 0;
}