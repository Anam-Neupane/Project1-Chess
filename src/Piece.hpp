#ifndef PIECE_HPP
#define PIECE_HPP

#include <raylib.h>

extern float squareSize;
extern Vector2 boardPosition;

enum PieceType {
    ROOK = 1,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
    PAWN
};
class Piece {

    public:

        int type;  // 1 to 6 for rook, knight, bishop, queen, king, pawn
        int color; // 0 for black, 1 for white
        Vector2 position;
        Texture2D texture;
        bool captured;
    
        Piece(int t,int c, Vector2 pos ,Texture2D tex):type(t),color(c),position(pos),texture(tex),captured(false)
        {}

};
class CameraHandler {
public:
    Camera2D camera;
    
    CameraHandler(int screenWidth, int screenHeight);
    void UpdateCamera();
    void BeginCamera();
    void EndCamera();
};
#endif // PIECE_HPP