#ifndef BOARD_H
#define BOARD_H

#include <raylib.h>
#include <vector>
#include "Piece.hpp"

const int boardSize = 8;
const int pieceTypes = 6; //  rook, knight, bishop, queen, king, pawn
const int pieceColors = 2; // black, white
class Board{
    private:
        std::vector<Piece> pieces; //class object;
        float pieceWidth;
        float pieceHeight;
        Vector2 mousePos;
        bool dragging;
        int draggedPieceIndex;
        Vector2 originalPosition;
        Vector2 offset;
        int CurrentPlayer;  // 0 for black and 1 for white

        const int initialBoard[boardSize][boardSize] = {
        { 1,  2,  3,  4,  5,  3,  2,  1},
        { 6,  6,  6,  6,  6,  6,  6,  6},
        { 0,  0,  0,  0,  0,  0,  0,  0},
        { 0,  0,  0,  0,  0,  0,  0,  0},
        { 0,  0,  0,  0,  0,  0,  0,  0},
        { 0,  0,  0,  0,  0,  0,  0,  0},
        {-6, -6, -6, -6, -6, -6, -6, -6},
        {-1, -2, -3, -4, -5, -3, -2, -1}
        };

        //For capturing and recording.
        int whiteCapturedCount = 0;
        int blackCapturedCount = 0;
        int whiteScore = 0;
        int blackScore = 0;
        int GetPieceValue(int pieceType);

        //For showing the points
        Vector2 whiteScorePosition;
        Vector2 blackScorePosition;

    public:

        Board();
        ~Board();

        void LoadPieces();
        void DrawPieces();
        void UnloadPieces();
        void UpdateDragging();
        void CapturePiece(int capturedPieceIndex);
        void DrawScores();
};

#endif // BOARD_H
