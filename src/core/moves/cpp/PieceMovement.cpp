#include "../hpp/PieceMovement.hpp"
#include "../hpp/MoveUtils.hpp"
#include "../hpp/MoveSimulation.hpp"
#include "../../Board.hpp"
#include "../../Constants.hpp"
#include "../hpp/MoveValidator.hpp"

#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <algorithm>
#include <cmath>

namespace PieceMovement
{

    bool IsPawnMoveValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition)
    {

        // Converting pixel positions to board coordinates
        int currentX = MoveUtils::PixelToBoard(originalPosition.x, boardPosition.x, squareSize);
        int currentY = MoveUtils::PixelToBoard(originalPosition.y, boardPosition.y, squareSize);
        int newX = MoveUtils::PixelToBoard(newPosition.x, boardPosition.x, squareSize);
        int newY = MoveUtils::PixelToBoard(newPosition.y, boardPosition.y, squareSize);

        int dx = newX - currentX;
        int dy = newY - currentY;

        int startRow = (piece.color == 0) ? 1 : 6; // Starting row for pawns
        int direction = (piece.color == 0) ? 1 : -1;

        // Checking if new position is within board bounds
        if (!MoveUtils::IsInBounds(newX, newY))
        {
            return false; // Out of bounds
        }

        // Normal move
        if (dy == direction && dx == 0)
        {
            for (const auto &otherPiece : pieces)
            {
                if (!otherPiece.captured && Vector2Equals(otherPiece.position, newPosition))
                {
                    return false; // Cannot move to a square occupied by another piece
                }
            }
            return true; // Normal Pawn move
        }

        // Initial double move
        if (newX == currentX && newY == currentY + 2 * direction && currentY == startRow)
        {

            // Checking that both square directly in front and two squares in front are clear
            Vector2 intermediatePosition = {originalPosition.x, originalPosition.y + direction * squareSize};
            for (const auto &otherPiece : pieces)
            {
                if (!otherPiece.captured && (Vector2Equals(otherPiece.position, intermediatePosition) || Vector2Equals(otherPiece.position, newPosition)))
                {
                    return false; // Blocked by another piece
                }
            }
            return true;
        }

        // Capture move
        if (dy == direction && abs(dx) == 1)
        {
            for (const auto &otherPiece : pieces)
            {
                if (!otherPiece.captured && Vector2Equals(otherPiece.position, newPosition) && otherPiece.color != piece.color)
                {
                    return true; // Can capture an opponent's piece
                }
            }
            return false; // Cannot move diagonally without capturing
        }

        return false; // By default, moves are invalid
    }

    bool IsRookMoveValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition)
    {
        // Converting pixel positions to board coordinates
        int currentX = MoveUtils::PixelToBoard(originalPosition.x, boardPosition.x, squareSize);
        int currentY = MoveUtils::PixelToBoard(originalPosition.y, boardPosition.y, squareSize);
        int newX = MoveUtils::PixelToBoard(newPosition.x, boardPosition.x, squareSize);
        int newY = MoveUtils::PixelToBoard(newPosition.y, boardPosition.y, squareSize);

        // Checking if new position is within board bounds
        if (!MoveUtils::IsInBounds(newX, newY))
        {
            return false; // Out of bounds
        }

        if (currentX != newX && currentY != newY) // To check if the Rook is moving vertically and horizontally
        {
            return false;
        }

        // To determine direction and to check each square in the path
        int xDirection = (newX - currentX) != 0 ? ((newX - currentX) > 0 ? 1 : -1) : 0;
        int yDirection = (newY - currentY) != 0 ? ((newY - currentY) > 0 ? 1 : -1) : 0;

        int x = currentX + xDirection;
        int y = currentY + yDirection;

        while (x != newX || y != newY)
        {
            for (const auto &otherPiece : pieces)
            {
                if (otherPiece.captured)
                    continue;
                Vector2 otherPosition = {boardPosition.x + x * squareSize, boardPosition.y + y * squareSize};
                if (Vector2Equals(otherPiece.position, otherPosition))
                {
                    return false; // Path is blocked
                }
            }
            x += xDirection;
            y += yDirection;
        }

        // To Check destination square(For Capture)
        for (const auto &otherPiece : pieces)
        {
            if (!otherPiece.captured && Vector2Equals(otherPiece.position, newPosition))
            {
                if (otherPiece.color == piece.color)
                {
                    return false; // Can't capture its own piece
                }
                else
                {
                    return true; // Valid capture move
                }
            }
        }

        return true; // Valid move, no piece blocking the path
    }

    bool IsBishopMoveValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition)
    {

        int currentX = MoveUtils::PixelToBoard(originalPosition.x, boardPosition.x, squareSize);
        int currentY = MoveUtils::PixelToBoard(originalPosition.y, boardPosition.y, squareSize);
        int newX = MoveUtils::PixelToBoard(newPosition.x, boardPosition.x, squareSize);
        int newY = MoveUtils::PixelToBoard(newPosition.y, boardPosition.y, squareSize);


        int dx = newX - currentX;
        int dy = newY - currentY;

        if (!MoveUtils::IsInBounds(newX, newY))
        {
            return false; // Out of bounds
        }

        // Checking if move is diagonal
        if (std::abs(dx) != std::abs(dy))
        {
            return false; // Not a diagonal move
        }

        // To Determine the direction of movement
        int xDirection = (dx > 0) ? 1 : -1;
        int yDirection = (dy > 0) ? 1 : -1;

        // Checking if pieces are blocking the path
        int x = currentX + xDirection;
        int y = currentY + yDirection;

        while (x != newX || y != newY)
        {
            if (!MoveUtils::IsInBounds(x, y))
            {

                return false; // Position out of bounds, break out early
            }
            Vector2 intermediatePosition = {boardPosition.x + x * squareSize, boardPosition.y + y * squareSize};

            for (const auto &otherPiece : pieces)
            {
                if (!otherPiece.captured && Vector2Equals(otherPiece.position, intermediatePosition))
                {
                    return false; // Path is blocked
                }
            }
            x += xDirection;
            y += yDirection;
        }

        // Checking the destination square(For Capture)
        for (const auto &otherPiece : pieces)
        {
            if (!otherPiece.captured && Vector2Equals(otherPiece.position, newPosition))
            {
                if (otherPiece.color == piece.color)
                {
                    return false; // Can't capture its own piece
                }
                else
                {
                    return true; // Valid capture move
                }
            }
        }

        return true; // Valid move, no piece blocking the path
    }

    bool IsQueenMoveValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition)
    {
        // Converting pixel positions to board coordinates
        int currentX = MoveUtils::PixelToBoard(originalPosition.x, boardPosition.x, squareSize);
        int currentY = MoveUtils::PixelToBoard(originalPosition.y, boardPosition.y, squareSize);
        int newX = MoveUtils::PixelToBoard(newPosition.x, boardPosition.x, squareSize);
        int newY = MoveUtils::PixelToBoard(newPosition.y, boardPosition.y, squareSize);

        int dx = newX - currentX;
        int dy = newY - currentY;

        if (currentX == newX || currentY == newY)
        {
            return IsRookMoveValid(piece, newPosition, pieces, originalPosition);
        }

        if (std::abs(dx) == std::abs(dy))
        {
            return IsBishopMoveValid(piece, newPosition, pieces, originalPosition);
        }

        return false; // Invalid move if neither Rook move nor Bishop move
    }

    bool IsKnightMoveValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition)
    {
        // Converting pixel positions to board coordinates
        int currentX = MoveUtils::PixelToBoard(originalPosition.x, boardPosition.x, squareSize);
        int currentY = MoveUtils::PixelToBoard(originalPosition.y, boardPosition.y, squareSize);
        int newX = MoveUtils::PixelToBoard(newPosition.x, boardPosition.x, squareSize);
        int newY = MoveUtils::PixelToBoard(newPosition.y, boardPosition.y, squareSize);

        int dx = std::abs(newX - currentX);
        int dy = std::abs(newY - currentY);

        if (!MoveUtils::IsInBounds(newX, newY))
        {
            return false;
        }

        // Checking if the move is in an "L" shape
        if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2)))
        {
            return false; // Move is not in "L" shape
        }

        // Checking the destination square(For Capture)
        // Checking the destination square(For Capture)
        for (const auto &otherPiece : pieces)
        {
            if (!otherPiece.captured && Vector2Equals(otherPiece.position, newPosition))
            {
                if (otherPiece.color == piece.color)
                {
                    return false; // Can't capture its own piece
                }
                else
                {
                    return true; // Valid capture move
                }
            }
        }

        return true; // Valid Knight move
    }

    bool IsKingMoveValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition)
    {

        int currentX = MoveUtils::PixelToBoard(originalPosition.x, boardPosition.x, squareSize);
        int currentY = MoveUtils::PixelToBoard(originalPosition.y, boardPosition.y, squareSize);
        int newX = MoveUtils::PixelToBoard(newPosition.x, boardPosition.x, squareSize);
        int newY = MoveUtils::PixelToBoard(newPosition.y, boardPosition.y, squareSize);

        if (!MoveUtils::IsInBounds(newX, newY))
        {
            return false;
        }

        int dx = std::abs(newX - currentX);
        int dy = std::abs(newY - currentY);

        // King movement in any direction(only one square)
        if ((dx <= 1 && dy <= 1) && !(dx == 0 && dy == 0))
        {

            // Checking if the move is blocked by another piece
            for (const auto &otherPiece : pieces)
            {
                if (!otherPiece.captured && Vector2Equals(otherPiece.position, newPosition))
                {
                    if (otherPiece.color == piece.color)
                    {
                        return false; // Can't move to a square occupied by its own piece
                    }
                    else
                    {
                        return true; // Valid capture move
                    }
                }
            }
            return true; // Valid move, no piece blocking path
        }
        return false; // Invalid move for King(only one square.)
    }

    bool IsEnPassantValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition)
    {
        // Use std::round to fix floating-point precision errors
        int currentX = static_cast<int>(std::round((originalPosition.x - boardPosition.x) / squareSize));
        int currentY = static_cast<int>(std::round((originalPosition.y - boardPosition.y) / squareSize));
        int newX = static_cast<int>(std::round((newPosition.x - boardPosition.x) / squareSize));
        int newY = static_cast<int>(std::round((newPosition.y - boardPosition.y) / squareSize));

        int dx = newX - currentX;
        int dy = newY - currentY;
        int direction = (piece.color == 0) ? 1 : -1;

        // Checking if the pawn is attempting an en passant move
        if (dy == direction && std::abs(dx) == 1)
        {
            const Piece &lastMovePiece = std::get<0>(MoveSimulation::lastMove);
            const Vector2 &lastOriginalPosition = std::get<1>(MoveSimulation::lastMove);
            const Vector2 &lastNewPosition = std::get<2>(MoveSimulation::lastMove);

            // Checking if last move was a double pawn move by the opponent
            if (lastMovePiece.type == PAWN && lastMovePiece.color != piece.color)
            {

                // Use std::round for floating-point precision
                int lastMoveStartY = static_cast<int>(std::round((lastOriginalPosition.y - boardPosition.y) / squareSize));
                int lastMoveEndY = static_cast<int>(std::round((lastNewPosition.y - boardPosition.y) / squareSize));

                if (std::abs(lastMoveEndY - lastMoveStartY) == 2)
                {

                    // Checking if pawn moved beside the current pawn
                    int enPassantX = static_cast<int>(std::round((lastNewPosition.x - boardPosition.x) / squareSize));
                    int enPassantY = lastMoveStartY - direction; // En passant capture square
                    if (enPassantX == newX && enPassantY == newY)
                    {
                        return true; // Valid en passant move
                    }
                }
            }
        }
        return false; // Not a valid en passant move
    }

    bool IsCastlingValid(Piece &king, const Vector2 &newPosition, std::vector<Piece> &pieces, const Vector2 &originalPosition, Board &board)
    {
        if (king.type != KING)
        {
            return false;
        }

        int currentX = static_cast<int>((originalPosition.x - boardPosition.x) / squareSize);
        int newX = static_cast<int>((newPosition.x - boardPosition.x) / squareSize);

        int dx = (newX - currentX);
        if (std::abs(dx) != 2)
        {
            return false; // For Castling must move exactly 2 squares horizontally
        }

        bool kingside = dx > 0;
        int rookX = kingside ? 7 : 0;
        int rookY = static_cast<int>((originalPosition.y - boardPosition.y) / squareSize);

        // Finding rook
        Piece *rook = nullptr;
        for (auto &piece : pieces)
        {
            if (piece.type == ROOK && piece.color == king.color && static_cast<int>((piece.position.x - boardPosition.x) / squareSize) == rookX &&
                static_cast<int>((piece.position.y - boardPosition.y) / squareSize) == rookY)
            {
                rook = &piece;
                break;
            }
        }
        if (!rook || king.hasMoved || rook->hasMoved)
        {
            return false;
        }

        int step = kingside ? 1 : -1;
        for (int x = currentX + step; x != rookX; x += step)
        {
            Vector2 intermediatePosition = {boardPosition.x + x * squareSize, boardPosition.y + rookY * squareSize};

            // Temporarily moving king to this intermediate position
            Vector2 tempKingPosition = king.position;
            king.position = intermediatePosition;

            // Checking if the king would be in check at this position
            if (MoveValidator::IsKingInCheck(pieces, intermediatePosition, king.color, board))
            {
                king.position = tempKingPosition; // Restore original position
                return false;                     // Castling is not allowed as king would be in check
            }

            // Restore the king's position
            king.position = tempKingPosition;

            for (const auto &piece : pieces)
            {
                if (Vector2Equals(piece.position, intermediatePosition))
                {
                    return false;
                }
            }
        }

        if (MoveValidator::IsKingInCheck(pieces, originalPosition, king.color, board))
        {
            return false; // Can't castle while in check.
        }

        // Final check: Ensuring that king isn't moving into check at the castling destination
        Vector2 finalKingPosition = newPosition;
        if (MoveValidator::IsKingInCheck(pieces, finalKingPosition, king.color, board))
        {
            return false;
        }

        return true;
    }

    bool CanPieceAttack(const Piece &piece, const Vector2 &targetPosition, const std::vector<Piece> &pieces)
    {

        switch (piece.type)
        {
        case PAWN:
            return ForPawnCaptureValid(piece, targetPosition, pieces, piece.position);
        case ROOK:
            return IsRookMoveValid(piece, targetPosition, pieces, piece.position);
        case BISHOP:
            return IsBishopMoveValid(piece, targetPosition, pieces, piece.position);
        case QUEEN:
            return IsQueenMoveValid(piece, targetPosition, pieces, piece.position);
        case KNIGHT:
            return IsKnightMoveValid(piece, targetPosition, pieces, piece.position);
        case KING:
            return IsKingMoveValid(piece, targetPosition, pieces, piece.position);
        default:
            return false;
        }
    }

    bool ForPawnCaptureValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition)
    {

        int direction = (piece.color == 0) ? 1 : -1; // Here 0 is white and 1 is black
        Vector2 attackLeft = {originalPosition.x - squareSize, originalPosition.y + (squareSize * direction)};
        Vector2 attackRight = {originalPosition.x + squareSize, originalPosition.y + (squareSize * direction)};

        // Checking if the new position matches one of the attack positions
        if (Vector2Equals(newPosition, attackLeft) || Vector2Equals(newPosition, attackRight))
        {
            return true;
        }

        return false;
    }

}