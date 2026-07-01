#include "../hpp/MoveUtils.hpp"
#include "../hpp/MoveSimulation.hpp"
#include "../../Board.hpp"
#include "../../Constants.hpp"
#include "../hpp/MoveValidator.hpp"
#include "../hpp/PieceMovement.hpp"

#include <iostream>
#include <raylib.h>

using MoveUtils::Vector2Equals;

namespace MoveSimulation {

    
bool SimulateMoveForOur(Piece& piece, const Vector2& newPosition, std::vector<Piece>& pieces, Board& board) {
    
    // Find the actual piece in the pieces vector (piece might be a copy)
    Piece* movingPiece = nullptr;
    for (Piece& p : pieces) {
        if (!p.captured && 
            Vector2Equals(p.position, piece.position) && 
            p.type == piece.type && 
            p.color == piece.color) {
            movingPiece = &p;
            break;
        }
    }
    
    if (!movingPiece) {
        return false; // Piece not found in vector
    }
    
    Vector2 tempPosition = movingPiece->position;
    Piece* capturedPiece = nullptr;

    // Checking if the move would capture a piece
    for (Piece& otherPiece : pieces) {
        if (!otherPiece.captured && 
            Vector2Equals(otherPiece.position, newPosition) && 
            otherPiece.color != piece.color) {
            capturedPiece = &otherPiece;
            break;
        }
    }

    // Temporarily moving the piece IN THE VECTOR
    movingPiece->position = newPosition;
    if (capturedPiece) {
        capturedPiece->captured = true;
    }

    // King's position after the move
    Vector2 kingPosition = (piece.color == 0) ? board.blackKingPosition : board.whiteKingPosition;
    bool isInCheck = MoveValidator::IsKingInCheck(pieces, kingPosition, piece.color, board);

    // Revert the move
    movingPiece->position = tempPosition;
    if (capturedPiece) {
        capturedPiece->captured = false;
    }

    return !isInCheck;
}

    bool SimulateMove(Piece &piece, const Vector2 &newPosition, std::vector<Piece> &pieces, Board &board)
    {
        // Find the actual piece in the pieces vector (piece might be a copy)
        Piece* movingPiece = nullptr;
        for (Piece& p : pieces) {
            if (!p.captured && 
                Vector2Equals(p.position, piece.position) && 
                p.type == piece.type && 
                p.color == piece.color) {
                movingPiece = &p;
                break;
            }
        }
        
        if (!movingPiece) {
            return false; // Piece not found in vector
        }

        Vector2 tempPosition = movingPiece->position;
        Piece *capturedPiece = nullptr;

        for (Piece &otherPiece : pieces)
        {
            if (!otherPiece.captured && 
                Vector2Equals(otherPiece.position, newPosition) && 
                otherPiece.color != piece.color)
            {
                capturedPiece = &otherPiece;
                break;
            }
        }

        movingPiece->position = newPosition;
        if (capturedPiece) {
            capturedPiece->captured = true;
        }

        Vector2 opponentKingPosition = (piece.color == 0) ? board.whiteKingPosition : board.blackKingPosition;
        bool isInCheck = MoveValidator::IsKingInCheck(pieces, opponentKingPosition, 1 - piece.color, board);

        movingPiece->position = tempPosition;
        if (capturedPiece) {
            capturedPiece->captured = false;
        }

        return !isInCheck;
    }

bool CheckKingAfterMove(Piece &king, Vector2 &newPosition, std::vector<Piece> &pieces, const Vector2 &originalPosition, Board &board) {
    
    // Find the actual king in the pieces vector (king might be a copy)
    Piece* actualKing = nullptr;
    for (Piece& p : pieces) {
        if (!p.captured && 
            p.type == KING && 
            p.color == king.color) {
            actualKing = &p;
            break;
        }
    }
    
    if (!actualKing) {
        return false; // King not found
    }
    
    // Store original position
    Vector2 tempKingPosition = actualKing->position;

    // Find and temporarily mark the piece at new position as captured (if any)
    Piece* capturedPiece = nullptr;
    for (auto& piece : pieces) {
        if (!piece.captured && 
            Vector2Equals(piece.position, newPosition) && 
            piece.color != king.color) {
            capturedPiece = &piece;
            capturedPiece->captured = true;
            break;
        }
    }

    // Move the king to new position IN THE VECTOR
    actualKing->position = newPosition;

    // Check if king is in check at the new position
    bool isInCheck = MoveValidator::IsKingInCheck(pieces, newPosition, king.color, board);

    // Restore original state
    actualKing->position = tempKingPosition;
    if (capturedPiece) {
        capturedPiece->captured = false;
    }

    return !isInCheck;
}


bool SimulateMoveAndCheck(std::vector<Piece>& pieces, Piece& piece, const Vector2& newPosition, int kingColor, Board& board) {
    
    // Storing original position
    Vector2 originalPosition = piece.position;

    // Simulating the move
    piece.position = newPosition;

    // Temporarily remove any captured piece at the new position
    Piece* capturedPiece = nullptr;
    for (auto& p : pieces) {
        if (p.position.x == newPosition.x && p.position.y == newPosition.y && !p.captured) {
            capturedPiece = &p;
            capturedPiece->captured = true;
            break;
        }
    }

    // Checking if the king is still in check after this move
    bool kingInCheck = MoveValidator::IsKingInCheck(pieces, (kingColor == 1) ? board.whiteKingPosition : board.blackKingPosition, kingColor, board);

    // Reverting the move
    piece.position = originalPosition;
    if (capturedPiece) {
        capturedPiece->captured = false;
    }

    // Return's whether the king is still in check
    return kingInCheck;
}

}
