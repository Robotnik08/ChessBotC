#include "opening_book.h"
#include "chess.h"

extern const unsigned char opening_book_compressed[];

// parse compressed opening book data into hashtable

// hash table variables
#define BOOK_SIZE 100000

// hash table entry structure
typedef struct {
    uint64_t hash; // hash of the position
    Move moves[EXPECTED_MAX_ENTRIES]; // moves for this position
    unsigned int weights[EXPECTED_MAX_ENTRIES]; // weights for each move
    unsigned char num_entries; // number of entries for this position
    void* next; // pointer to the next entry in case of hash collision
    bool is_used; // flag to indicate if this entry is used
} BookEntry;
// hash table
BookEntry opening_book[BOOK_SIZE]; // hash table for the opening book

void initOpeningBook() {
    // initialize the opening book
    memset(opening_book, 0, sizeof(opening_book));

    //  format:
    //  first 4 bytes: amount of positions in the book
    //  per position:
    //      first byte: number of entries
    //      next 8 bytes: hash
    //      per entry:
    //          next 2 bytes: move
    //          next 4 bytes: weight
    const unsigned char* ptr = opening_book_compressed;
    unsigned int num_positions = *(unsigned int*)ptr;
    ptr += sizeof(unsigned int);

    for (unsigned int i = 0; i < num_positions; i++) {
        unsigned char num_entries = *ptr++;
        uint64_t book_hash = *(uint64_t*)ptr;
        ptr += sizeof(uint64_t);

        // find the index in the hash table
        unsigned int index = book_hash % BOOK_SIZE;

        BookEntry* entry = &opening_book[index];
        // find an empty slot or a slot with the same hash
        while (entry->is_used) {
            if (entry->hash == book_hash) {
                // found an existing entry with the same hash, break
                break;
            }
            // move to the next entry in case of collision
            if (entry->next == NULL) {
                entry->next = calloc(1, sizeof(BookEntry));
            }
            entry = (BookEntry*)entry->next;
        }

        // fill the entry
        opening_book[index].hash = book_hash;
        opening_book[index].num_entries = num_entries;
        opening_book[index].is_used = true;

        for (unsigned char j = 0; j < num_entries; j++) {
            opening_book[index].moves[j] = *(Move*)ptr;
            ptr += sizeof(Move);
            opening_book[index].weights[j] = *(unsigned int*)ptr;
            ptr += sizeof(unsigned int);
        }
    }
}

Move getBookMove(uint64_t hash, unsigned int seed) {
    srand(seed);

    unsigned int index = hash % BOOK_SIZE;
    BookEntry* entry = &opening_book[index];
    while (entry->is_used) {
        if (entry->hash == hash) {
            unsigned char num_entries = entry->num_entries;
            if (num_entries == 0) {
                return NULL_MOVE; // no moves available
            }

            unsigned int total_weight = 0;
            for (unsigned char i = 0; i < num_entries; i++) {
                total_weight += entry->weights[i];
            }

            unsigned int random_value = rand() % total_weight;
            unsigned int cumulative_weight = 0;

            for (unsigned char i = 0; i < num_entries; i++) {
                cumulative_weight += entry->weights[i];
                if (random_value < cumulative_weight) {
                    return entry->moves[i];
                }
            }
        }
        if (entry->next == NULL) {
            break; // no more entries to check
        }
        entry = (BookEntry*)entry->next;
    }


    return NULL_MOVE; // no move found in the book
}