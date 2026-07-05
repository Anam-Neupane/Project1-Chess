#ifndef ENGINE_MOVE_HPP
#define ENGINE_MOVE_HPP

#include <raylib.h>

// EngineMove - a move returned by any chess engine implementation. 

struct EngineMove 
{
    Vector2 from;  // Source square in pixel coordinates (e.g. {0.0f, 55.0f})
    Vector2 to;    // Destination square 
    int promotionPiece;  // 0 = no promotion; otherwise a PieceType value (QUEEN = 4)
    bool isValid;   // false = engine has no move 


    EngineMove() : 
    from ({-1.0f, -1.0f}),
    to({-1.0f, -1.0f}),
    promotionPiece(0),
    isValid(false)
    {}
};

#endif