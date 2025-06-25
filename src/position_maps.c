#include "position_maps.h"
#include "chess.h"

int blackPawnMap[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};

int blackPawnMapEndGame[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    100,100,100,100,100,100,100,100,
    60, 60, 60, 60, 60, 60, 60, 60,
    35, 35, 35, 35, 35, 35, 35, 35,
    20, 20, 20, 20, 20, 20, 20, 20,
    10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10,
    0,  0,  0,  0,  0,  0,  0,  0,
};

int blackKnightMap[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};

int blackBishopMap[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};

int blackRookMap[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};

int blackQueenMap[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
    0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

int blackKingMapMiddleGame[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};

int blackKingMapEndGame[64] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};

int whitePawnMap[64];
int whitePawnMapEndGame[64];
int whiteKnightMap[64];
int whiteBishopMap[64];
int whiteRookMap[64];
int whiteQueenMap[64];
int whiteKingMapMiddleGame[64];
int whiteKingMapEndGame[64];

int tables[16 * 64];
extern int pieceValues[6]; // pawn, knight, bishop, rook, queen, king


void initPositionMaps() {
    // set up the position maps for the black pieces
    for (int i = 0; i < 64; i++) {
        whitePawnMap[i] = blackPawnMap[63 - i];
        whitePawnMapEndGame[i] = blackPawnMapEndGame[63 - i];
        whiteKnightMap[i] = blackKnightMap[63 - i];
        whiteBishopMap[i] = blackBishopMap[63 - i];
        whiteRookMap[i] = blackRookMap[63 - i];
        whiteQueenMap[i] = blackQueenMap[63 - i];
        whiteKingMapMiddleGame[i] = blackKingMapMiddleGame[63 - i];
        whiteKingMapEndGame[i] = blackKingMapEndGame[63 - i];
    }

    for (int i = 0; i < 64; i++) {
        whitePawnMap[i] += pieceValues[PAWN];
        whitePawnMapEndGame[i] += pieceValues[PAWN];
        whiteKnightMap[i] += pieceValues[KNIGHT];
        whiteBishopMap[i] += pieceValues[BISHOP];
        whiteRookMap[i] += pieceValues[ROOK];
        whiteQueenMap[i] += pieceValues[QUEEN];

        blackPawnMap[i] += pieceValues[PAWN];
        blackPawnMapEndGame[i] += pieceValues[PAWN];
        blackKnightMap[i] += pieceValues[KNIGHT];
        blackBishopMap[i] += pieceValues[BISHOP];
        blackRookMap[i] += pieceValues[ROOK];
        blackQueenMap[i] += pieceValues[QUEEN];
    }

    memcpy(&tables[0 * 64], whitePawnMap, sizeof(whitePawnMap));
    memcpy(&tables[1 * 64], whiteKnightMap, sizeof(whiteKnightMap));
    memcpy(&tables[2 * 64], whiteBishopMap, sizeof(whiteBishopMap));
    memcpy(&tables[3 * 64], whiteRookMap, sizeof(whiteRookMap));
    memcpy(&tables[4 * 64], whiteQueenMap, sizeof(whiteQueenMap));
    memcpy(&tables[5 * 64], whiteKingMapMiddleGame, sizeof(whiteKingMapMiddleGame));
    memcpy(&tables[6 * 64], whiteKingMapEndGame, sizeof(whiteKingMapEndGame));
    memcpy(&tables[7 * 64], whitePawnMapEndGame, sizeof(whitePawnMapEndGame));

    memcpy(&tables[8 * 64], blackPawnMap, sizeof(blackPawnMap));
    memcpy(&tables[9 * 64], blackKnightMap, sizeof(blackKnightMap));
    memcpy(&tables[10 * 64], blackBishopMap, sizeof(blackBishopMap));
    memcpy(&tables[11 * 64], blackRookMap, sizeof(blackRookMap));
    memcpy(&tables[12 * 64], blackQueenMap, sizeof(blackQueenMap));
    memcpy(&tables[13 * 64], blackKingMapMiddleGame, sizeof(blackKingMapMiddleGame));
    memcpy(&tables[14 * 64], blackKingMapEndGame, sizeof(blackKingMapEndGame));
    memcpy(&tables[15 * 64], blackPawnMapEndGame, sizeof(blackPawnMapEndGame));
}