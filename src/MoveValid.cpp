#include "MoveValid.hpp"
#include <iostream>
#include <cmath>

bool MoveValidator::Vector2Equals(const Vector2& a, const Vector2& b, float tolerance) {
    return (std::abs(a.x - b.x) < tolerance) && (std::abs(a.y - b.y) < tolerance);
}

bool MoveValidator::IsPawnMoveValid(const Piece& piece, const Vector2& newPosition, const std::vector<Piece>& pieces, const Vector2& originalPosition) {
    // Converting pixel positions to board coordinates
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
        return true;// Normal Pawn move
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


    return false; // By default, moves are invalid 
}

bool MoveValidator::IsRookMoveValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition)
{
    // Converting pixel positions to board coordinates
    int currentX = static_cast<int>((originalPosition.x - boardPosition.x) / squareSize);
    int currentY = static_cast<int>((originalPosition.y - boardPosition.y) / squareSize);
    int newX = static_cast<int>((newPosition.x - boardPosition.x) / squareSize);
    int newY = static_cast<int>((newPosition.y - boardPosition.y) / squareSize);

    if(currentX != newX && currentY != newY)//To check if the Rook is moving vertically and horizontally
    {
        return false;
    }
    // To determine the direction and check each square in the path
    int xDirection = (newX - currentX) != 0 ? ((newX - currentX) > 0 ? 1 : -1) : 0;
    int yDirection = (newY - currentY) != 0 ? ((newY - currentY) > 0 ? 1 : -1) : 0;

    int x = currentX + xDirection;
    int y = currentY + yDirection;

    while (x != newX || y != newY) {
        for (const auto &otherPiece : pieces) {
            Vector2 otherPosition = {boardPosition.x + x * squareSize, boardPosition.y + y * squareSize};
            if (Vector2Equals(otherPiece.position, otherPosition)) {
                return false; // Path is blocked
            }
        }
        x += xDirection;
        y += yDirection;
    }

    // To Check the destination square(For Capture)
    for (const auto &otherPiece : pieces) {
        if (Vector2Equals(otherPiece.position, newPosition)) {
            if (otherPiece.color == piece.color) {
                return false; // Can't capture its own piece
            } else {
                return true; // Valid capture move
            }
        }
    }

    return true; // Valid move, no piece blocking the path
}

bool MoveValidator::IsBishopMoveValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition)
{
    // Converting pixel positions to board coordinates
    int currentX = static_cast<int>((originalPosition.x - boardPosition.x) / squareSize);
    int currentY = static_cast<int>((originalPosition.y - boardPosition.y) / squareSize);
    int newX = static_cast<int>((newPosition.x - boardPosition.x) / squareSize);
    int newY = static_cast<int>((newPosition.y - boardPosition.y) / squareSize);

    int dx = newX - currentX;
    int dy = newY - currentY;

    // Checking if move is diagonal
    if (std::abs(dx) != std::abs(dy)) {
        return false; // Not a diagonal move
    }

    // To Determine the direction of movement
    int xDirection = (dx > 0) ? 1 : -1;
    int yDirection = (dy > 0) ? 1 : -1;

    // Checking if pieces are blocking the path
    int x = currentX + xDirection;
    int y = currentY + yDirection;

    while (x != newX && y != newY) {
        Vector2 intermediatePosition = {boardPosition.x + x * squareSize, boardPosition.y + y * squareSize};
        for (const auto &otherPiece : pieces) {
            if (Vector2Equals(otherPiece.position, intermediatePosition)) {
                return false; // Path is blocked
            }
        }
        x += xDirection;
        y += yDirection;
    }

    // Checking the destination square(For Capture)
    for (const auto &otherPiece : pieces) {
        if (Vector2Equals(otherPiece.position, newPosition)) {
            if (otherPiece.color == piece.color) {
                return false; // Can't capture its own piece
            } else {
                return true; // Valid capture move
            }
        }
    }

    return true; // Valid move, no piece blocking the path
}

bool MoveValidator::IsQueenMoveValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition)
{
    // Converting pixel positions to board coordinates
    int currentX = static_cast<int>((originalPosition.x - boardPosition.x) / squareSize);
    int currentY = static_cast<int>((originalPosition.y - boardPosition.y) / squareSize);
    int newX = static_cast<int>((newPosition.x - boardPosition.x) / squareSize);
    int newY = static_cast<int>((newPosition.y - boardPosition.y) / squareSize);

    int dx = newX - currentX;
    int dy = newY - currentY;

    // All Oueen moves are Rook moves but not all Rook moves are Queen's move.😶‍🌫️ 
    if (currentX == newX || currentY == newY) {
        return IsRookMoveValid(piece, newPosition, pieces, originalPosition);
    }

    // All queen moves are Bishop move but not all Bishop moves are Queen's move.😂
    if (std::abs(dx) == std::abs(dy)) {
        return IsBishopMoveValid(piece, newPosition, pieces, originalPosition);
    }

    return false; // Invalid move if neither Rook move nor Bishop move
}

bool MoveValidator::IsKnightMoveValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition)
{
    // Converting pixel positions to board coordinates
    int currentX = static_cast<int>((originalPosition.x - boardPosition.x) / squareSize);
    int currentY = static_cast<int>((originalPosition.y - boardPosition.y) / squareSize);
    int newX = static_cast<int>((newPosition.x - boardPosition.x) / squareSize);
    int newY = static_cast<int>((newPosition.y - boardPosition.y) / squareSize);
  
    int dx = std::abs(newX - currentX);
    int dy = std::abs(newY - currentY);

    // Checking if the move is in an "L" shape
    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) {
        return false; // Move is not in "L" shape
    }

    // Checking the destination square(For Capture)
    for (const auto &otherPiece : pieces) {
        if (Vector2Equals(otherPiece.position, newPosition)) {
            if (otherPiece.color == piece.color) {
                return false; // Can't capture its own piece
            } else {
                return true; // Valid capture move
            }
        }
    }

    return true; // Valid Knight move
}

bool MoveValidator::IsKingMoveValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition)
{
        // Convert pixel positions to board coordinates
    int currentX = static_cast<int>((originalPosition.x - boardPosition.x) / squareSize);
    int currentY = static_cast<int>((originalPosition.y - boardPosition.y) / squareSize);
    int newX = static_cast<int>((newPosition.x - boardPosition.x) / squareSize);
    int newY = static_cast<int>((newPosition.y - boardPosition.y) / squareSize);

    int dx = std::abs(newX - currentX);
    int dy = std::abs(newY - currentY);

    // King movement in any direction(only one square)
    if ((dx <= 1 && dy <= 1) && !(dx == 0 && dy == 0)) {

        // Checking if the move is blocked by another piece
        for (const auto& otherPiece : pieces) {
            if (Vector2Equals(otherPiece.position, newPosition)) {
                if (otherPiece.color == piece.color) {
                    return false; // Can't move to a square occupied by its own piece
                } else {
                    return true; // Valid capture move
                }
            }
        }

        //To Prevent two Kings from being adjacent
        for (const auto& otherPiece : pieces) 
        {
            if (otherPiece.type == KING && otherPiece.color != piece.color) 
            {
                // Converting the opponent King's position to board coordinates
                int opponentKingX = static_cast<int>((otherPiece.position.x - boardPosition.x) / squareSize);
                int opponentKingY = static_cast<int>((otherPiece.position.y - boardPosition.y) / squareSize);

                // Checking if the new position is adjacent to the opponent King
                if (std::abs(newX - opponentKingX) <= 1 && std::abs(newY - opponentKingY) <= 1) 
                {
                    return false; // Invalid move, Kings cannot be adjacent
                }
            }
        }

        return true; // Valid move, no piece blocking the path
    }
    return false;// Invalid move for King(only one square.)
}

bool MoveValidator::IsInCheck(int kingColor, std::vector<Piece>& pieces) {

    //To find the king of the specified color
    Piece* king = nullptr;
    for (auto& piece : pieces) {
        if (piece.type == KING && piece.color == kingColor) {
            king = &piece;
            break;
        }
    }

    // Check if any opponent's piece can attack the king
    for (auto& piece : pieces) {
        if (piece.color != kingColor) { // Opponent's pieces
            // Directly validate the move without checking for check condition
            if (IsMoveValid(piece, king->position, pieces, piece.position)) {
                std::cout << "King is in check by piece at: " << piece.position.x << ", " << piece.position.y << std::endl;
                return true; // King is in check
            }
        }
    }

    return false; // No opponent piece can attack the king
}

bool MoveValidator::IsMoveValid(Piece& piece, const Vector2& newPosition, std::vector<Piece>& pieces, const Vector2& originalPosition) {

    // Validate the move based on piece type rules
    bool moveValid = false;
    switch (piece.type) {
        case PAWN:
            moveValid = IsPawnMoveValid(piece, newPosition, pieces, originalPosition);
            break;
        case ROOK:
            moveValid = IsRookMoveValid(piece, newPosition, pieces, originalPosition);
            break;
        case BISHOP:
            moveValid = IsBishopMoveValid(piece, newPosition, pieces, originalPosition);
            break;
        case QUEEN:
            moveValid = IsQueenMoveValid(piece, newPosition, pieces, originalPosition);
            break;
        case KNIGHT:
            moveValid = IsKnightMoveValid(piece, newPosition, pieces, originalPosition);
            break;
        case KING:
            moveValid = IsKingMoveValid(piece, newPosition, pieces, originalPosition);
            break;
        default:
            std::cout << "Invalid move: Unknown piece type\n";
            return false;
    }

    if (!moveValid) {
        return false;
    }

    // Backup the original state of the board
    Vector2 originalPiecePosition = piece.position;
    Piece* targetPiece = nullptr;
    Vector2 originalTargetPosition = { -1, -1 }; // Initialize with a dummy value

    // Find and temporarily remove the piece at the new position (if any)
    for (auto& otherPiece : pieces) {
        if (Vector2Equals(otherPiece.position, newPosition)) {
            targetPiece = &otherPiece;
            originalTargetPosition = otherPiece.position;
            otherPiece.position = { -1, -1 }; // Temporarily remove the piece
            break;
        }
    }

    // Move the piece to the new position
    piece.position = newPosition;

    // Check if the move leaves the king in check
    bool validMove = !IsInCheck(piece.color, pieces);

    // Undo the move
    piece.position = originalPiecePosition;
    if (targetPiece != nullptr) {
        targetPiece->position = originalTargetPosition; // Restore the captured piece's position
    }

    if (!validMove) {
        std::cout << "Move is invalid because it leaves the king in check.\n";
        return false;
    }

    std::cout << "Move is valid.\n";
    return true;
}
