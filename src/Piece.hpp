#ifndef PIECE_HPP
#define PIECE_HPP

#include <raylib.h>

enum PieceType {
    PAWN = 0,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};
class Piece {

    public:

        int type;  // 0 to 5 for rook, knight, bishop, queen, king, pawn
        int color; // 0 for black, 1 for white
        Vector2 position;
        Texture2D texture;
    
        Piece(int t,int c, Vector2 pos ,Texture2D tex):type(t),color(c),position(pos),texture(tex){}

};
#endif // PIECE_HPP