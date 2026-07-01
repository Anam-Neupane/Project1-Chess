#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include<raylib.h>

// Board Dimensions
constexpr int boardSize = 8;
constexpr int pieceTypes = 6; // Rook, Knight, Bishop, Queen, King, Pawn
constexpr int pieceColors = 2; // black, white

// These are set at runtime in Board constructor 
extern float squareSize;
extern Vector2 boardPosition; 

#endif // CONSTANTS_HPP