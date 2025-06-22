#include "transposition_table.h"
#include "chess.h"

TranspositionTableEntry transposition_table[TT_SIZE];

void initTranspositionTable() {
    clearTranspositionTable();
}

void clearTranspositionTable() {
    memset(transposition_table, 0, sizeof(transposition_table));
}

int getTranspositionTableIndex(uint64_t key) {
    return key & TT_MASK;
}

void storeTranspositionTableEntry(uint64_t key, int score, int depth, NodeType type, Move best_move) {
    transposition_table[getTranspositionTableIndex(key)] = (TranspositionTableEntry){
        .key = key,
        .score = score,
        .depth = depth,
        .type = type,
        .best_move = best_move
    };
}

TranspositionTableEntry* getTranspositionTableEntry(uint64_t key) {
    int index = getTranspositionTableIndex(key);
    TranspositionTableEntry* entry = &transposition_table[index];
    
    if (entry->key == key) {
        return entry;
    }
    
    return NULL; // Entry not found
}