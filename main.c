#include <stdio.h>
#include "chess.h"
#include "engine.h"

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