#ifndef OPENING_BOOK_H
#define OPENING_BOOK_H

#include "chess.h"
#include "opening_book_values.h"
#include <stdint.h>

#define EXPECTED_MAX_ENTRIES 20 // Maximum number of entries per position in the book

void initOpeningBook();

Move getBookMove(uint64_t hash, unsigned int seed);

#endif // OPENING_BOOK_H