#ifndef BOARD_H
#define BOARD_H

#include <raylib.h>
#include <vector>

const int boardSize = 8;
const int pieceTypes = 6; //  rook , bishop, queen, king, knight, pawn
const int pieceColors = 2; // black, white
class Piece {
    public:

    int type;  // 0 to 5 for , rook ,bishop, queen, king, knight, pawn
    int color; // 0 for black, 1 for white
    Vector2 position;
    Texture2D texture;
    
    Piece(int t,int c, Vector2 pos ,Texture2D tex):type(t),color(c),position(pos),texture(tex){}

};
class Board{

    private:
    std::vector<Piece> pieces; 
    float pieceWidth;
    float pieceHeight;
    Vector2 mousePos;
    bool dragging;
    int draggedPieceIndex;
    Vector2 boardPosition;
    Vector2 offset;


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

    public:

    Board();
    ~Board();

    void LoadPieces();
    void DrawPieces();
    void UnloadPieces();
    void UpdateDragging();
};

#endif // BOARD_H
