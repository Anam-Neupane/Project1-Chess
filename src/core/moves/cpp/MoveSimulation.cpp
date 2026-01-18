#include "../hpp/MoveUtils.hpp"
#include "../hpp/MoveSimulation.hpp"
#include "../../Board.hpp"
#include "../../Constants.hpp"
#include "../hpp/MoveValidator.hpp"
#include "../hpp/PieceMovement.hpp"

#include <iostream>
#include <raylib.h>
#include <raymath.h>

namespace MoveSimulation {

    
bool SimulateMoveForOur(Piece& piece, const Vector2& newPosition, std::vector<Piece>& pieces, Board& board) {
    
    Vector2 tempPosition = piece.position;
    Piece* capturedPiece = nullptr;

    // Checking if the move would capture a piece
    for (Piece& otherPiece : pieces) {
        if (Vector2Equals(otherPiece.position, newPosition) && otherPiece.color != piece.color) {
            capturedPiece = &otherPiece;
            break;
        }
    }

    // Temporarily moving the piece
    piece.position = newPosition;
    if (capturedPiece) {
        capturedPiece->captured = true;
    }

    // King's position after the move
    Vector2 kingPosition = (piece.color == 0) ? board.blackKingPosition : board.whiteKingPosition;
    bool isInCheck = MoveValidator::IsKingInCheck(pieces, kingPosition, piece.color, board);

    // Revert the move
    piece.position = tempPosition;
    if (capturedPiece) {
        capturedPiece->captured = false;
    }

        return !isInCheck;
    }

    bool SimulateMove(Piece &piece, const Vector2 &newPosition, std::vector<Piece> &pieces, Board &board)
    {

        Vector2 tempPosition = piece.position;
        Piece *capturedPiece = nullptr;

        for (Piece &otherPiece : pieces)
        {
            if (Vector2Equals(otherPiece.position, newPosition) && otherPiece.color != piece.color)
            {
                capturedPiece = &otherPiece;
                break;
            }
        }

    piece.position = newPosition;
    if (capturedPiece) {
        capturedPiece->captured = true;
    }

    Vector2 opponentKingPosition = (piece.color == 0) ? board.whiteKingPosition : board.blackKingPosition;
    bool isInCheck = MoveValidator::IsKingInCheck(pieces, opponentKingPosition, 1 - piece.color, board);

    piece.position = tempPosition;
    if (capturedPiece) {
        capturedPiece->captured = false;
    }

        return !isInCheck;
    }

bool CheckKingAfterMove(Piece &king, Vector2 &newPosition, std::vector<Piece> &pieces, const Vector2 &originalPosition, Board &board) {
    
    // Temporarily storing current position of the king
    Vector2 tempKingPosition = king.position;

    // Storing references to the piece at new position, if any
    Piece* capturedPiece = nullptr;
    bool isCaptured = false;
    
    // Find and temporarily mark the piece at the new position as captured (if any)
    for (auto& piece : pieces) {
        if (Vector2Equals(piece.position, newPosition) && !piece.captured) {
            capturedPiece = &piece;
            capturedPiece->captured = true;
            isCaptured = true;
            break;
        }
    }

    // Moving the king to new position
    king.position = newPosition;


        // Checking if the king is still in check after moving
        bool isInCheck = false;
        for (const auto &piece : pieces)
        {
            if (piece.color != king.color && !piece.captured)
            {
                if (PieceMovement::CanPieceAttack(piece, newPosition, pieces))
                {
                    isInCheck = true;
                    break;
                }
            }
        }

    // King's original position
    king.position = tempKingPosition;

    // Restoring the captured piece if it was temporarily marked
    if (isCaptured && capturedPiece) {
        capturedPiece->captured = false;
    }


    if (isInCheck) {
        return false; //King is in check after the move
    }

    // Performing a final check if the king is still in check after all moves
    bool finalCheck = MoveValidator::IsKingInCheck(pieces, newPosition, king.color, board);

    // Return false if the king is in check; otherwise, return true
    return !finalCheck;
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
