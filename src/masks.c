#include "masks.h"
#include "chess.h"

Bitboard passedPawnMask[128];

void initMasks() {
    for (int i = 0; i < 128; i++) {
        passedPawnMask[i] = 0ULL;
    }

    Bitboard fileAMask = 0x0101010101010101ULL;

    for (int i = 0; i < 64; i++) {
        int file = i % 8;
        int rank = i / 8;


        // white pawns
        passedPawnMask[i] |= fileAMask << file;
        if (file < 7) {
            passedPawnMask[i] |= fileAMask << (file + 1);
        }
        if (file > 0) {
            passedPawnMask[i] |= fileAMask << (file - 1);
        }
        passedPawnMask[i] <<= rank * 8;

        // black pawns
        passedPawnMask[i + 64] |= fileAMask << file;
        if (file < 7) {
            passedPawnMask[i + 64] |= fileAMask << (file + 1);
        }
        if (file > 0) {
            passedPawnMask[i + 64] |= fileAMask << (file - 1);
        }
        passedPawnMask[i + 64] >>= (7 - rank) * 8;
    }
}