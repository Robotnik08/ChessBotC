#include "search_move.h"
#include "evaluate.h"
#include "chess.h"
#include "order.h"

extern Board board;
extern unsigned long long int repetition_history[1000];
extern int move_history_count;

extern volatile bool stop_search;

int findBestMove(int depth, Move* best_move) {
    Move moves[MAX_MOVES];
    int num_moves = generateMoves(moves, false);

    int move_scores[MAX_MOVES];
    getScores(moves, move_scores, num_moves, getAttackedMapOnlyPawn());

    Move selectedMove = NULL_MOVE;

    int color = (board.side_to_move == WHITE) ? 1 : -1;
    
    int alpha = NEGATIVE_INFINITY;
    int beta = POSITIVE_INFINITY;

    for (int i = 0; i < num_moves; i++) {
        swapNextBestMove(moves, move_scores, num_moves, i);

        makeMove(moves[i]);
        int eval = -search(depth - 1, -color, -beta, -alpha);
        unmakeMove(moves[i]);

        if (eval > alpha) {
            alpha = eval;
            selectedMove = moves[i];
        }
        if (alpha >= beta) {
            break;
        }
    }

    if (best_move) {
        *best_move = selectedMove;
    }

    return alpha;
}

int search(int depth, int color, int alpha, int beta) {
    if (stop_search) {
        return 0;
    }

    if (depth == 0) {
        return extendedSearch(color, beta, alpha);
    }

    Move moves[MAX_MOVES];
    int num_moves = generateMoves(moves, false);
    
    int move_scores[MAX_MOVES];
    getScores(moves, move_scores, num_moves, getAttackedMapOnlyPawn());

    if (board.state == CHECKMATE) {
        return -MATE_SCORE + depth;
    } else if (board.state != NONE) {
        return 0;
    }

    for (int i = 0; i < num_moves; i++) {
        swapNextBestMove(moves, move_scores, num_moves, i);
        
        makeMove(moves[i]);
        int eval = -search(depth - 1, -color, -beta, -alpha);
        unmakeMove(moves[i]);

        if (eval >= beta) {
            return beta;
        }
        if (eval > alpha) {
            alpha = eval;
        }
    }

    return alpha;
}

// only searches captures
int extendedSearch (int color, int beta, int alpha) {
    if (stop_search) {
        return 0;
    }

    int eval = color * evaluatePosition();
    if (eval >= beta) {
        return beta;
    }
    if (eval > alpha) {
        alpha = eval;
    }

    Move moves[MAX_MOVES];
    int num_moves = generateMoves(moves, true);
    
    int move_scores[MAX_MOVES];
    getScores(moves, move_scores, num_moves, getAttackedMapOnlyPawn());

    for (int i = 0; i < num_moves; i++) {
        swapNextBestMove(moves, move_scores, num_moves, i);
        
        makeMove(moves[i]);
        int score = -extendedSearch(-color, -alpha, -beta);
        unmakeMove(moves[i]);

        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}