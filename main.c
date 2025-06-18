#include <stdio.h>
#include "chess.h"
#include "engine.h"

extern Board board;
extern unsigned long long int repetition_history[1000];
extern int move_history_count;

int main(int argc, char* argv[]) {
    init();

    if (argc == 2 && strcmp(argv[1], "engine") == 0) {
        while (1) {
            char input[255];
            scanf("%s", input);
            input[254] = '\0';

            if (strcmp(input, "setfen") == 0) {
                char fen[255];
                // Clear input buffer before reading the FEN string
                int c;
                while ((c = getchar()) != '\n' && c != EOF); // discard leftover input
                if (fgets(fen, sizeof(fen), stdin)) {
                    // Remove trailing newline if present
                    size_t len = strlen(fen);
                    if (len > 0 && fen[len - 1] == '\n') {
                        fen[len - 1] = '\0';
                    }
                    parseFEN(fen);
                    repetition_history[0] = getZobristHash();
                    printf("ok\n");
                    fflush(stdout);
                }
            }

            if (strcmp(input, "setmovehistory") == 0) { // Set the board based from start to move history
                char moves[6 * 1000]; // a Move takes max 6 characters (e.g., "e2e4 " or "e7e8q "), around 16000 moves are possible? let's assume 1000 moves max

                int c;
                while ((c = getchar()) != '\n' && c != EOF); // discard leftover input
                if (fgets(moves, sizeof(moves), stdin)) {
                    // Remove trailing newline if present
                    size_t len = strlen(moves);
                    if (len > 0 && moves[len - 1] == '\n') {
                        moves[len - 1] = '\0';
                    }

                    char* token = strtok(moves, " ");
                    while (token != NULL) {
                        int extra = 0;
                        if (token[4] != '\0') {
                            switch (token[4]) {
                                case 'e': extra = EN_PASSANT; break;
                                case 'l': extra = PAWN_LEAP; break;
                                case 'o': extra = CASTLE; break;
                                case 'n': extra = PROMOTION_KNIGHT; break;
                                case 'b': extra = PROMOTION_BISHOP; break;
                                case 'r': extra = PROMOTION_ROOK; break;
                                case 'q': extra = PROMOTION_QUEEN; break;
                            }
                        }
                        Move move = MOVE((token[0] - 'a') + 8 * (token[1] - '1'),
                                         (token[2] - 'a') + 8 * (token[3] - '1'), extra);
                        makeMove(move);
                        repetition_history[move_history_count] = getZobristHash();

                        token = strtok(NULL, " ");
                    }
                    printf("ok\n");
                    fflush(stdout);
                }
            }

            if (strcmp(input, "getmove") == 0) {
                // get amount of seconds to think
                int seconds;
                scanf("%d", &seconds);
                // Clear input buffer before reading the next command
                int c;
                while ((c = getchar()) != '\n' && c != EOF); // discard leftover input
                
                initEngine();
                int depth_searched = 0;
                Move best_move = getbestMove(seconds, &depth_searched);

                printMove(best_move);
                
                printf("%d\n", depth_searched);
                
                cleanupEngine();

                printf("ok\n");

                fflush(stdout);
            }

            if (strcmp(input, "end") == 0) {
                break;
            }
        }

        cleanup();
    }
    return 0;
}