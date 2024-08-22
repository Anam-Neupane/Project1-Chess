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
        Texture2D promotionTexture[12];
        
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

        //For showing the Player
        Vector2 playerturnPosition;

    public:

        bool PawnPromo = false;
        bool Checkmate = false;
        bool Cwhite = false;


        bool p1;

        Board();
        ~Board();

        Vector2 promotionPosition;  // Store the pawn's position for promotion
        Vector2 blackKingPosition;
        Vector2 whiteKingPosition;

        void Reset();
        void LoadPieces();
        void LoadPromotionTexture();
        void DrawPieces();
        void UnloadPieces();
        void UpdateDragging();
        void CapturePiece(int capturedPieceIndex);
        void ExecuteEnPassant(Piece& capturingPawn, std::vector<Piece>& pieces, const Vector2& originalPosition, const Vector2& newPosition);
        void DrawScores();
        void DrawPlayer();
        static void ExecuteCastling(Piece &king,bool kingside,std::vector<Piece> &pieces,const Vector2 originalPosition);
        void DrawPromotionMenu(Vector2 position,int color);
        void HandlePawnPromotion(int color,Vector2 position);

};

#endif // BOARD_H