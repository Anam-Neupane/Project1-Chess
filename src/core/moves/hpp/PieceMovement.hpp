#ifndef PIECEMOVEMNT_HPP
#define PIECEMOVEMNT_HPP

#include "../../Piece.hpp"
#include <vector>

class Board;

namespace PieceMovement {

    bool IsPawnMoveValid(const Piece &piece, const Vector2 &newPosition, 
                         const std::vector<Piece> &pieces, const Vector2 &originalPosition);
    bool IsRookMoveValid(const Piece &piece, const Vector2 &newPosition, 
                         const std::vector<Piece> &pieces, const Vector2 &originalPosition);
    bool IsBishopMoveValid(const Piece &piece, const Vector2 &newPosition, 
                           const std::vector<Piece> &pieces, const Vector2 &originalPosition);
    bool IsQueenMoveValid(const Piece &piece, const Vector2 &newPosition, 
                          const std::vector<Piece> &pieces, const Vector2 &originalPosition);
    bool IsKnightMoveValid(const Piece &piece, const Vector2 &newPosition, 
                           const std::vector<Piece> &pieces, const Vector2 &originalPosition);
    bool IsKingMoveValid(const Piece &piece, const Vector2 &newPosition, 
                         const std::vector<Piece> &pieces, const Vector2 &originalPosition);

    //Special Moves
    bool IsCastlingValid(Piece &king, const Vector2 &newPosition, 
                         std::vector<Piece> &pieces, const Vector2 &originalPosition, Board &board);
    bool IsEnPassantValid(const Piece &piece, const Vector2 &newPosition, 
                          const std::vector<Piece> &pieces, const Vector2 &originalPosition);
    bool ForPawnCaptureValid(const Piece &piece, const Vector2 &newPosition, 
                             const std::vector<Piece> &pieces, const Vector2 &originalPosition);

    // Attack detection
    bool CanPieceAttack(const Piece &piece, const Vector2 &targetPosition, 
                        const std::vector<Piece> &pieces);
    
}

#endif