#ifndef MOVEVALID_HPP
#define MOVEVALID_HPP

#include "../../Piece.hpp"
#include <vector>
#include <tuple>

class Board; // Forward Declaration

namespace MoveValidator {

    bool IsMoveLegal(const Piece& piece, const Vector2& newPosition,
                     const std::vector<Piece>& pieces, const Vector2& originalPosition,
                     Board& board);

    void ExecuteMove(Piece& piece, Vector2& newPosition,
                     std::vector<Piece>& pieces, const Vector2& originalPosition,
                     Board& board);

    // Combined validation + execution (for backwards compatibility)
    bool IsMoveValid(Piece &piece, Vector2 &newPosition, 
                          std::vector<Piece> &pieces, const Vector2 &originalPosition, Board &board, 
                          bool forHighlightOnly = false); 

    bool IsCheckmate(std::vector<Piece> &pieces, int kingColor, Board &board);
    bool IsKingInCheck(const std::vector<Piece> &pieces,
                           const Vector2 &kingPosition, int kingColor, const Board &board);

    // Quick check if a move is in the pre-generated valid moves list
     bool IsMoveInValidMoves(const Vector2& targetPosition, const std::vector<Vector2>& validMoves);
};

#endif // MOVEVALID_HPP
