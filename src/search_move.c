#include "search_move.h"
#include "evaluate.h"
#include "chess.h"
#include "order.h"
#include "transposition_table.h"

extern Board board;
extern unsigned long long int repetition_history[1000];
extern int move_history_count;

extern volatile bool stop_search;

int findBestMove(int depth, Move* best_move, int best_move_index) {
    Move moves[MAX_MOVES];
    int num_moves = generateMoves(moves, false);

    int move_scores[MAX_MOVES];
    getScores(moves, move_scores, num_moves, getAttackedMapOnlyPawn());
    
    if (best_move_index != -1) {
        move_scores[best_move_index] = POSITIVE_INFINITY;
    }

    Move selectedMove = NULL_MOVE;
    
    int alpha = NEGATIVE_INFINITY;
    int beta = POSITIVE_INFINITY;

    for (int i = 0; i < num_moves; i++) {
        swapNextBestMove(moves, move_scores, num_moves, i);

        makeMove(moves[i]);
        int eval = -search(depth - 1, 0, -beta, -alpha);
        unmakeMove(moves[i]);

        if (stop_search) {
            if (best_move && selectedMove != NULL_MOVE) {
                *best_move = selectedMove;
            }

            return alpha;
        }

        if (eval > alpha) {
            alpha = eval;
            selectedMove = moves[i];
        }
    }

    if (best_move) {
        *best_move = selectedMove;
    }

    return alpha;
}

int search(int depth, int ply_from_root, int alpha, int beta) {
    if (stop_search) {
        return 0;
    }

    // skip if mating sequence was already found and better then this ply
    alpha = __max(alpha, -MATE_SCORE + ply_from_root);
    beta = __min(beta, MATE_SCORE - ply_from_root);
    if (alpha >= beta) {
        return alpha;
    }

    uint64_t hash = getZobristHash();
    TranspositionTableEntry* entry = getTranspositionTableEntry(hash);
    if (entry && entry->depth >= depth) {
        int score = correctMateScoreFromTT(entry->score, ply_from_root);
        
        if (entry->type == EXACT) {
            return score;
        } else if (entry->type == UPPER_BOUND) {
            if (score <= alpha) {
                return score;
            }
        } else if (entry->type == LOWER_BOUND) {
            if (score >= beta) {
                return score;
            }
        }
    }

    if (depth == 0) {
        return extendedSearch(alpha, beta);
    }
    
    Move moves[MAX_MOVES];
    int num_moves = generateMoves(moves, false);

    if (board.state == CHECKMATE) {
        return -(MATE_SCORE - ply_from_root);
    } else if (board.state != NONE) {
        return 0;
    }
    
    int move_scores[MAX_MOVES];
    getScores(moves, move_scores, num_moves, getAttackedMapOnlyPawn());

    int original_alpha = alpha;
    Move best_move = NULL_MOVE;

    for (int i = 0; i < num_moves; i++) {
        swapNextBestMove(moves, move_scores, num_moves, i);
        
        makeMove(moves[i]);
        int eval = -search(depth - 1, ply_from_root + 1, -beta, -alpha);
        unmakeMove(moves[i]);

        if (stop_search) {
            return 0;
        }

        if (eval >= beta) {
            storeTranspositionTableEntry(hash, beta, depth, ply_from_root, LOWER_BOUND, moves[i]);
            return beta;
        }

        if (eval > alpha) {
            alpha = eval;
            best_move = moves[i];
        }
    }

    storeTranspositionTableEntry(hash, alpha, depth, ply_from_root, (alpha > original_alpha) ? EXACT: UPPER_BOUND, best_move);

    return alpha;
}

// only searches captures
int extendedSearch (int alpha, int beta) {
    if (stop_search) {
        return 0;
    }

    int eval = evaluatePosition();
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
        int score = -extendedSearch(-beta, -alpha);
        unmakeMove(moves[i]);

        if (stop_search) {
            return 0;
        }

        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}