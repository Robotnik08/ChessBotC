#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <stdint.h>
#include "chess.h"

typedef enum {
    EXACT,
    LOWER_BOUND,
    UPPER_BOUND
} NodeType;

typedef struct {
    uint64_t key;
    int score;
    int depth;
    NodeType type;
    Move best_move;
} TranspositionTableEntry;

#define TT_SIZE (1 << 26)
#define TT_MASK (TT_SIZE - 1)

void initTranspositionTable();

void clearTranspositionTable();

int getTranspositionTableIndex(uint64_t key);

int convertMateScoreToTT(int score, int depth_searched);

int correctMateScoreFromTT(int score, int depth_searched);

void storeTranspositionTableEntry(uint64_t key, int score, int depth, int depth_searched, NodeType type, Move best_move);

TranspositionTableEntry* getTranspositionTableEntry(uint64_t key);

#endif // TRANSPOSITION_TABLE_H