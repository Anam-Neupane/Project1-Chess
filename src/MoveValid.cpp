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
        return true; // Valid move, no piece blocking the path
    }
    return false;// Invalid move for King(only one square.)
}

bool MoveValidator::IsCastlingValid(const Piece &king, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition)
{

    std::cout << "King X: " << static_cast<int>((originalPosition.x - boardPosition.x) / squareSize) << std::endl;


    for (const auto &piece : pieces) {
    std::cout << "Piece Type: " << piece.type << ", Position: (" << piece.position.x << ", " << piece.position.y << ")" << std::endl;
    }

    if (king.type != KING) {
        return false;
    }
        // Convert pixel positions to board coordinates
    int currentX = static_cast<int>((originalPosition.x - boardPosition.x) / squareSize);
    int newX = static_cast<int>((newPosition.x - boardPosition.x) / squareSize);
    std::cout << "Current X: " << currentX << ", New X: " << newX << std::endl;

    int dx = (newX - currentX);
    std::cout<<"dx = "<<dx<<std::endl;
    if (std::abs(dx) != 2) {
        return false; // Castling must move exactly 2 squares horizontally
    }

    // Determine if castling is kingside or queenside
    bool kingside = dx > 0;
    int rookX = kingside ? 7 : 0;  // Kingside rook is at 7, Queenside rook is at 0
    int rookY = static_cast<int>((originalPosition.y - boardPosition.y) / squareSize);

    // Find the rook
    const Piece* rook = nullptr;
    for (const auto &piece : pieces) {
        if (piece.type == ROOK && piece.color == king.color && static_cast<int>((piece.position.x - boardPosition.x) / squareSize) == rookX &&
            static_cast<int>((piece.position.y - boardPosition.y) / squareSize) == rookY) {
            rook = &piece;
            break;
        }
    }
    std::cout << "Rook X: " << rookX << ", Rook Y: " << rookY << std::endl;
    if (!rook) {
        return false; // No rook found in the correct position
    }

    // Check if the king or rook has moved
    if (king.hasMoved || rook->hasMoved) {
        return false;
    }

    // Check if the path between the king and rook is clear
    int step = kingside ? 1 : -1;
    for (int x = currentX + step; x != rookX; x += step) {
        Vector2 intermediatePosition = {boardPosition.x + x * squareSize, boardPosition.y + rookY * squareSize};
        for (const auto &piece : pieces) {
            if (Vector2Equals(piece.position, intermediatePosition)) {
                return false; // Path is blocked
            }
        }
    }

    return true;
}

bool MoveValidator::IsMoveValid(const Piece& piece, const Vector2& newPosition, const std::vector<Piece>& pieces, const Vector2 & originalPosition) {

    for (const auto& otherPiece : pieces){
            if (Vector2Equals(otherPiece.position, newPosition) && otherPiece.type == KING) {
            return false; // Invalid move, king can't be captured
        }
    }
    
    switch (piece.type) {
        case PAWN:
            return IsPawnMoveValid(piece,newPosition,pieces,originalPosition);
        case ROOK:
            return IsRookMoveValid(piece,newPosition,pieces,originalPosition);
        case BISHOP:
            return IsBishopMoveValid(piece,newPosition,pieces,originalPosition);
        case QUEEN:
            return IsQueenMoveValid(piece,newPosition,pieces,originalPosition);
        case KNIGHT:
            return IsKnightMoveValid(piece,newPosition,pieces,originalPosition);
        case KING:
            return IsKingMoveValid(piece,newPosition,pieces,originalPosition)|| IsCastlingValid(piece, newPosition, pieces, originalPosition);
        default:
            std::cout << "Invalid move: Unknown piece type" << std::endl;
            return false;
    }
}

