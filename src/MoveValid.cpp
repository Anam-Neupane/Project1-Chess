#include "MoveValid.hpp"
#include <iostream>
#include <cmath>

bool MoveValidator::Vector2Equals(const Vector2& a, const Vector2& b, float tolerance) {
    return (std::abs(a.x - b.x) < tolerance) && (std::abs(a.y - b.y) < tolerance);
}

bool MoveValidator::IsPawnMoveValid(const Piece& piece, const Vector2& newPosition, const std::vector<Piece>& pieces, const Vector2& originalPosition) {
    // Convert pixel positions to board coordinates
    int currentX = static_cast<int>((originalPosition.x - boardPosition.x) / squareSize);
    int currentY = static_cast<int>((originalPosition.y - boardPosition.y) / squareSize);
    int newX = static_cast<int>((newPosition.x - boardPosition.x) / squareSize);
    int newY = static_cast<int>((newPosition.y - boardPosition.y) / squareSize);

    int dx = newX - currentX;
    int dy = newY - currentY;

    int direction = (piece.color == 0) ? 1 : -1;

    // Normal move
    if (dy == direction && dx == 0) {
        for (const auto& otherPiece : pieces) {
            if (Vector2Equals(otherPiece.position, newPosition)) {
                return false; // Cannot move to a square occupied by another piece
            }
        }
        return true;//Normal Pawn move
    }

    // Initial double move
    if (dy == direction * 2 && dx == 0 && 
        std::abs(originalPosition.y - (boardPosition.y + (piece.color == 1 ? 6 : 1) * squareSize)) < 0.1f) {
        for (const auto& otherPiece : pieces) {
            if (Vector2Equals(otherPiece.position, newPosition) || 
                Vector2Equals(otherPiece.position, Vector2{piece.position.x, piece.position.y + direction * squareSize})) {
                return false; // Cannot move to a square occupied by another piece or jump over a piece
            }
        }
        return true;//Valid Initial Double move pawn
    }

    // Capture move
    if (dy == direction && abs(dx) == 1) {
        for (const auto& otherPiece : pieces) {
            if (Vector2Equals(otherPiece.position, newPosition) && otherPiece.color != piece.color) {
                return true; // Can capture an opponent's piece
            }
        }
        return false; // Cannot move diagonally without capturing
    }


    return false; // By default, moves are invalid unless specified otherwise
}



bool MoveValidator::IsMoveValid(const Piece& piece, const Vector2& newPosition, const std::vector<Piece>& pieces, const Vector2 & originalPosition) {
    
    for (const auto& otherPiece : pieces){
            if (Vector2Equals(otherPiece.position, newPosition) && otherPiece.type == KING) {

            return false; // Invalid move, king can't be captured
        }
    }
    
    switch (piece.type) {
        case PAWN:
            return IsPawnMoveValid(piece, newPosition, pieces, originalPosition);
        default:
            std::cout << "Invalid move: Unknown piece type" << std::endl;
            return false;
    }
}
