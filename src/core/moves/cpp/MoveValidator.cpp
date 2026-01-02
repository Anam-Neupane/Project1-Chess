#include "../hpp/MoveValidator.hpp"
#include "../hpp/PieceMovement.hpp"
#include "../hpp/MoveGeneration.hpp"
#include "../hpp/MoveSimulation.hpp"
#include "../../Board.hpp"
#include <iostream>
#include <algorithm>
#include <raymath.h>

// Define lastMove in MoveSimulation namespace
std::tuple<Piece, Vector2, Vector2> MoveSimulation::lastMove = std::make_tuple(Piece{}, Vector2{0.0f, 0.0f}, Vector2{0.0f, 0.0f});

bool MoveValidator::IsKingInCheck(const std::vector<Piece> &pieces, const Vector2 &kingPosition, int kingColor, const Board &board)
{

    for (const auto &piece : pieces)
    {
        if (!piece.captured && piece.color != kingColor)
        { // Checking only opponent's pieces
            if (PieceMovement::CanPieceAttack(piece, kingPosition, pieces))
            {
                std::cout << "King is in check by piece of type " << piece.type << " at position (" << piece.position.x << "," << piece.position.y << ")" << std::endl;
                return true; // King is in check
            }
        }
    }
    return false; // King is not in check
}

bool MoveValidator::IsMoveValid(Piece &piece, Vector2 &newPosition, std::vector<Piece> &pieces, const Vector2 &originalPosition, Board &board)
{

    bool isValid = false;

    // Validating move based on piece type
    switch (piece.type)
    {
    case PAWN:
        if (PieceMovement::IsEnPassantValid(piece, newPosition, pieces, originalPosition))
        {
            board.ExecuteEnPassant(const_cast<Piece &>(piece), const_cast<std::vector<Piece> &>(pieces), originalPosition, newPosition);
            isValid = true;
        }
        else
        {
            isValid = PieceMovement::IsPawnMoveValid(piece, newPosition, pieces, originalPosition);
        }
        break;
    case ROOK:
        isValid = PieceMovement::IsRookMoveValid(piece, newPosition, pieces, originalPosition);
        break;
    case BISHOP:
        isValid = PieceMovement::IsBishopMoveValid(piece, newPosition, pieces, originalPosition);
        break;
    case QUEEN:
        isValid = PieceMovement::IsQueenMoveValid(piece, newPosition, pieces, originalPosition);
        break;
    case KNIGHT:
        isValid = PieceMovement::IsKnightMoveValid(piece, newPosition, pieces, originalPosition);
        break;
    case KING:
        if (PieceMovement::IsKingMoveValid(piece, newPosition, pieces, originalPosition))
        {
            if (MoveSimulation::CheckKingAfterMove(piece, newPosition, pieces, originalPosition, board))
            {
                piece.hasMoved = true;
                isValid = true;
            }
        }
        if (PieceMovement::IsCastlingValid(piece, newPosition, pieces, originalPosition, board))
        {
            bool kingside = newPosition.x > originalPosition.x;
            Board::ExecuteCastling(const_cast<Piece &>(piece), kingside, const_cast<std::vector<Piece> &>(pieces), originalPosition);
            isValid = true;
        }
        break;
    default:
        std::cout << "Invalid move: Unknown piece type" << std::endl;
        isValid = false;
        break;
    }

    // After validating the move, checking if it leaves king in check
    if (isValid && piece.type != KING)
    {
        if (!MoveSimulation::SimulateMoveForOur(const_cast<Piece &>(piece), newPosition, const_cast<std::vector<Piece> &>(pieces), board))
        {
            std::cout << "Move would leave the king in check!" << std::endl;
            return false;
        }
    }

    // Checking if opponent's king is in check after move
    if (!MoveSimulation::SimulateMove(const_cast<Piece &>(piece), newPosition, const_cast<std::vector<Piece> &>(pieces), board) && isValid)
    {
        std::cout << "Opponent's king Checking done." << std::endl;
    }

    // For pawn promotion
    if (isValid && piece.type == PAWN)
    {
        int newY = static_cast<int>((newPosition.y - boardPosition.y) / squareSize);
        int promotionRank = (piece.color == 0) ? 7 : 0;

        if (newY == promotionRank)
        {
            board.PawnPromo = true;
            board.promotionPosition = newPosition;
            board.p1 = (piece.color == 0) ? 0 : 1;
            std::cout << "Promoting pawn" << std::endl;
        }
    }

    if (isValid)
    {
        MoveSimulation::lastMove = std::make_tuple(piece, originalPosition, newPosition); // Storing the move
        std::cout << "Move is valid." << std::endl;
    }
    return isValid;
}

bool MoveValidator::IsCheckmate(std::vector<Piece> &pieces, int kingColor, Board &board)
{
    std::cout << "Checking for checkmate..." << std::endl;
    Vector2 kingPosition = (kingColor == 1) ? board.whiteKingPosition : board.blackKingPosition;

    // First, check if the king is in check
    if (!MoveValidator::IsKingInCheck(pieces, kingPosition, kingColor, board))
    {
        std::cout << "King is not in check" << std::endl;
        return false; // Not checkmate if the king is not in check
    }

    std::cout << "King is in check, looking for escape moves..." << std::endl;

    // Try all possible moves for all pieces of the same color
    for (auto &piece : pieces)
    {
        if (piece.color == kingColor && !piece.captured)
        {
            // Generate raw potential moves (without IsMoveValid filtering)
            std::vector<Vector2> potentialMoves;
            
            switch (piece.type) {
                case PAWN:
                    potentialMoves = MoveGeneration::GetPawnMoves(piece, pieces, board);
                    break;
                case ROOK:
                    potentialMoves = MoveGeneration::GetRookMoves(piece, pieces, board);
                    break;
                case BISHOP:
                    potentialMoves = MoveGeneration::GetBishopMoves(piece, pieces, board);
                    break;
                case QUEEN:
                    potentialMoves = MoveGeneration::GetQueenMoves(piece, pieces, board);
                    break;
                case KNIGHT:
                    potentialMoves = MoveGeneration::GetKnightMoves(piece, pieces, board);
                    break;
                case KING:
                    potentialMoves = MoveGeneration::GetKingMoves(piece, pieces, board);
                    break;
                default:
                    break;
            }

            for (const Vector2 &move : potentialMoves)
            {
                // Save original state
                Vector2 originalPosition = piece.position;
                
                // Find if there's an enemy piece to capture at the target position
                Piece* capturedPiece = nullptr;
                for (auto &otherPiece : pieces)
                {
                    if (!otherPiece.captured && 
                        Vector2Equals(otherPiece.position, move) && 
                        otherPiece.color != piece.color)
                    {
                        capturedPiece = &otherPiece;
                        break;
                    }
                }

                // Check if move is blocked by own piece (can't move there)
                bool blockedByOwnPiece = false;
                for (const auto &otherPiece : pieces)
                {
                    if (!otherPiece.captured && 
                        Vector2Equals(otherPiece.position, move) && 
                        otherPiece.color == piece.color)
                    {
                        blockedByOwnPiece = true;
                        break;
                    }
                }
                
                if (blockedByOwnPiece)
                {
                    continue; // Can't move to a square occupied by own piece
                }

                // Simulate the move
                piece.position = move;
                if (capturedPiece)
                {
                    capturedPiece->captured = true;
                }

                // Update king position if the king moved
                Vector2 checkKingPos = kingPosition;
                if (piece.type == KING)
                {
                    checkKingPos = move;
                }

                // Check if king is still in check after this move
                bool stillInCheck = MoveValidator::IsKingInCheck(pieces, checkKingPos, kingColor, board);

                // Restore original state
                piece.position = originalPosition;
                if (capturedPiece)
                {
                    capturedPiece->captured = false;
                }

                if (!stillInCheck)
                {
                    std::cout << "Found escape: piece type " << piece.type
                              << " from (" << originalPosition.x << "," << originalPosition.y
                              << ") to (" << move.x << "," << move.y << ")";
                    if (capturedPiece)
                    {
                        std::cout << " (capturing piece type " << capturedPiece->type << ")";
                    }
                    std::cout << std::endl;
                    return false; // Found a valid move that escapes check
                }
            }
        }
    }

    // No valid moves found that get the king out of check
    std::cout << "CHECKMATE! No escape moves found." << std::endl;
    return true;
}
