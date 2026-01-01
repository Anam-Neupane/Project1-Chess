#ifndef PIECE_HPP
#define PIECE_HPP

#include <raylib.h>
#include "Constants.hpp"

enum PieceType {
    NONE = 0,
    ROOK = 1,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
    PAWN,
};
class Piece {

    public:
        Piece(){}
        int type;  // 1 to 6 for rook, knight, bishop, queen, king, pawn
        int color; // 0 for black, 1 for white
        Vector2 position;
        Texture2D texture;
        bool captured;
        bool hasMoved = false;
    
        Piece(int t,int c, Vector2 pos ,Texture2D tex):type(t),color(c),position(pos),texture(tex),captured(false)
        {}

};
#endif // PIECE_HPP