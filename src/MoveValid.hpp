#ifndef MOVEVALID_HPP
#define MOVEVALID_HPP

#include "Piece.hpp"
#include <vector>

class MoveValidator {
private:
    static bool Vector2Equals(const Vector2& a, const Vector2& b, float tolerance = 0.01f);
    static bool IsPawnMoveValid(const Piece& piece, const Vector2& newPosition, const std::vector<Piece>& pieces, const Vector2& originalPosition);

public:
    static bool IsMoveValid(const Piece& piece, const Vector2& newPosition, const std::vector<Piece>& pieces, const Vector2 &originalPosition);
};

#endif // MOVEVALID_HPP
