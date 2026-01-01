#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include<raylib.h>

// Board Dimensions
constexpr int BOARD_SIZE = 8;
constexpr int PIECE_TYPES = 6; // Rook, Knight, Bishop, Queen, King, Pawn
constexpr int PIECE_COLORS = 2; // black, white

// These are set at runtime in Board constructor 
extern float squareSize;
extern Vector2 boardPosition; 

#endif // CONSTANTS_HPP