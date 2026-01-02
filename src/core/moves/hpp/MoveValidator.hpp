#ifndef MOVEVALID_HPP
#define MOVEVALID_HPP

#include "../../Piece.hpp"
#include <vector>
#include <tuple>

class Board; // Forward Declaration

class MoveValidator {
        
    public:

    static bool IsMoveValid(Piece &piece, Vector2 &newPosition, 
                           std::vector<Piece> &pieces, const Vector2 &originalPosition, Board &board); 
    static bool IsCheckmate(std::vector<Piece> &pieces, int kingColor, Board &board);
    static bool IsKingInCheck(const std::vector<Piece> &pieces,
                            const Vector2 &kingPosition, int kingColor, const Board &board);
};

#endif // MOVEVALID_HPP
