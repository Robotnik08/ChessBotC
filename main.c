#include <stdio.h>
#include "chess.h"
#include "engine.h"

#include "opening_book.h"

extern Board board;
extern unsigned long long int repetition_history[1000];
extern int move_history_count;

int debug = 0;

extern bool inBook;

void trim_newline(char* str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

int main(int argc, char* argv[]) {
    initChess();
    initEngine();

    char line[2048];
    bool running = true;
    inBook = true; // start with book enabled

    if (argc == 2 && strcmp(argv[1], "engine") == 0) {
        while (running && fgets(line, sizeof(line), stdin)) {
            trim_newline(line);

            if (strcmp(line, "setfen") == 0) {
                if (!fgets(line, sizeof(line), stdin)) break;
                trim_newline(line);
                parseFEN(line);
                repetition_history[0] = getZobristHash();
                move_history_count = 0;
                printf("ok\n");
            } else if (strcmp(line, "setmovehistory") == 0) {
                if (!fgets(line, sizeof(line), stdin)) break;
                trim_newline(line);

                char* token = strtok(line, " ");
                while (token != NULL) {
                    Move move = stringToMove(token);

                    makeMove(move);
                    move_history_count++;
                    repetition_history[move_history_count] = getZobristHash();
                    token = strtok(NULL, " ");
                }

                printf("ok\n");

            } else if (strcmp(line, "getmove") == 0) {
                if (!fgets(line, sizeof(line), stdin)) break;
                trim_newline(line);
                int milliseconds = atoi(line);

                int depth_searched = 0;
                Move best_move = getbestMove(milliseconds, &depth_searched);
                printMove(best_move); // this prints to stdout (e.g., "e2e4")
                printf("%d\n", depth_searched);

                printf("ok\n");

            } else if (strcmp(line, "end") == 0 || strcmp(line, "quit") == 0) {
                printf("ok\n");
                running = false;

            } else {
                printf("unknown command: %s\n", line);
            }

            fflush(stdout);
        }

        cleanupEngine();
        cleanupChess();
    }
    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdbool.h>
// #include "chess.h"

// #define MAX_LINE_LENGTH 1024
// #define MAX_MOVES 256
// #define MAX_ENTRIES 100000

// typedef struct {
//     char *fen;
//     char **moves;
//     int move_count;
//     bool updated;
// } Entry;

// Entry table[MAX_ENTRIES];
// int table_size = 0;
// int changes = 0;

// // Read file into a buffer
// char *read_file(const char *filename) {
//     FILE *f = fopen(filename, "r");
//     if (!f) {
//         perror("Failed to open file");
//         exit(1);
//     }

//     fseek(f, 0, SEEK_END);
//     long length = ftell(f);
//     rewind(f);

//     char *buffer = malloc(length + 1);
//     fread(buffer, 1, length, f);
//     buffer[length] = '\0';

//     fclose(f);
//     return buffer;
// }

// // Split a string by a delimiter
// char **split_string(const char *str, const char *sep, int *count) {
//     int capacity = 16;
//     char **result = malloc(capacity * sizeof(char *));
//     *count = 0;

//     const char *start = str;
//     const char *match;

//     size_t sep_len = strlen(sep);

//     while ((match = strstr(start, sep)) != NULL) {
//         size_t len = match - start;
//         char *part = malloc(len + 1);
//         strncpy(part, start, len);
//         part[len] = '\0';

//         if (*count >= capacity) {
//             capacity *= 2;
//             result = realloc(result, capacity * sizeof(char *));
//         }
//         result[(*count)++] = part;

//         start = match + sep_len;
//     }

//     // Last part
//     if (*start != '\0') {
//         result[(*count)++] = strdup(start);
//     }

//     return result;
// }

// // Load entries from file content
// void load_entries(const char *content) {
//     int part_count;
//     char **parts = split_string(content, "pos ", &part_count);
//     for (int i = 1; i < part_count; i++) {
//         int move_count;
//         char **lines = split_string(parts[i], "\n", &move_count);
//         if (move_count == 0) continue;

//         table[table_size].fen = strdup(lines[0]);
//         table[table_size].moves = malloc((move_count - 1) * sizeof(char *));
//         table[table_size].move_count = move_count - 1;
//         table[table_size].updated = false;

//         for (int j = 1; j < move_count; j++) {
//             table[table_size].moves[j - 1] = strdup(lines[j]);
//         }

//         table_size++;

//         for (int j = 0; j < move_count; j++) free(lines[j]);
//         free(lines);
//     }

//     for (int i = 0; i < part_count; i++) free(parts[i]);
//     free(parts);
// }

// void extract_positional_fen(const char *fen, char *out_trimmed) {
//     int space_count = 0;
//     const char *p = fen;
//     while (*p && space_count < 3) {
//         if (*p == ' ') {
//             space_count++;
//         }
//         *out_trimmed++ = *p++;
//     }
//     *out_trimmed = '\0';
// }

// // Recursive fixer
// void fix_table(const char *fen) {
//     char trimmed_fen[128];
//     extract_positional_fen(fen, trimmed_fen);

//     int entry_index = -1;
//     for (int i = 0; i < table_size; i++) {
//         char stored_trimmed[128];
//         extract_positional_fen(table[i].fen, stored_trimmed);
//         if (strcmp(trimmed_fen, stored_trimmed) == 0) {
//             entry_index = i;
//             break;
//         }
//     }

//     if (entry_index == -1) {
//         return;
//     }

//     if (table[entry_index].updated) {
//         printf("Entry for FEN: %s already fixed, skipping.\n", trimmed_fen);
//         return;
//     }

//     // Load the FEN and generate moves
//     char **available = NULL;
//     int available_count = 0;
//     {
//         Move moves[MAX_MOVES];
//         available_count = generateMoves(moves, false);
//         available = malloc(available_count * sizeof(char *));
//         for (int i = 0; i < available_count; i++) {
//             available[i] = strdup(getNotation(moves[i]));
//         }
//     }

//     // Fix moves
//     char **corrected = malloc(table[entry_index].move_count * sizeof(char *));
//     int corrected_count = 0;

//     for (int i = 0; i < table[entry_index].move_count; i++) {
//         char *m = strdup(table[entry_index].moves[i]);
//         char *move_part = strtok(m, " ");
//         char *rest = strtok(NULL, "");

//         bool found = false;
//         for (int j = 0; j < available_count; j++) {
//             if (strcmp(available[j], move_part) == 0) {
//                 corrected[corrected_count++] = strdup(table[entry_index].moves[i]);
//                 found = true;
//                 break;
//             }
//         }

//         if (!found) {
//             for (int j = 0; j < available_count; j++) {
//                 if (strncmp(available[j], move_part, 4) == 0) {
//                     char corrected_move[32];
//                     snprintf(corrected_move, sizeof(corrected_move), "%s %s", available[j], rest ? rest : "");
//                     corrected[corrected_count++] = strdup(corrected_move);
//                     break;
//                 }
//             }
//         }

//         free(m);
//     }

//     // Replace original with corrected
//     for (int i = 0; i < table[entry_index].move_count; i++) {
//         free(table[entry_index].moves[i]);
//     }
//     free(table[entry_index].moves);

//     table[entry_index].moves = corrected;
//     table[entry_index].move_count = corrected_count;
//     table[entry_index].fen = generateFEN();
//     table[entry_index].updated = true;

//     changes++;
//     printf("(%d changes so far) Fixed entry for FEN: %s \n", changes, trimmed_fen);

//     // Apply corrected moves recursively
//     for (int i = 0; i < corrected_count; i++) {
//         char *m = strdup(corrected[i]);
//         char *move_str = strtok(m, " ");
//         Move move = stringToMove(move_str);
//         makeMove(move);

//         char *next_fen = generateFEN();
//         fix_table(next_fen);
//         free(next_fen);

//         unmakeMove(move);
//         free(m);
//     }

//     for (int i = 0; i < available_count; i++) free(available[i]);
//     free(available);
// }

// int main() {
//     initChess();

//     const char *input_file = "book.txt";
//     char *content = read_file(input_file);

//     load_entries(content);
//     free(content);

//     parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
//     fix_table("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

//     printf("All entries fixed.\n");

//     // write fixed entries back to file
//     FILE *f = fopen("fixed_book.txt", "w");
//     if (!f) {
//         perror("Failed to open output file");
//         exit(1);
//     }

//     for (int i = 0; i < table_size; i++) {
//         if (!table[i].updated) {
//             continue; // skip entries that were not updated, these stay corrupted
//         }
//         fprintf(f, "pos %s\n", table[i].fen);
//         for (int j = 0; j < table[i].move_count; j++) {
//             fprintf(f, "%s\n", table[i].moves[j]);
//         }
//     }

//     return 0;
// }
