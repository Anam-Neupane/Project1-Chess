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
                return true; // King is in check
            }
        }
    }
    return false; // King is not in check
}

bool MoveValidator::IsMoveLegal(const Piece &piece, const Vector2 &newPosition,
                                const std::vector<Piece> &pieces, const Vector2 &originalPosition,
                                Board &board)
{
    bool isValid = false;

    // Create mutable copies for functions that require non-const references
    Piece tempPiece = piece;
    Vector2 tempNewPos = newPosition;
    std::vector<Piece> tempPieces = pieces;

    // Validate move based on piece type (no execution, just checking)
    switch (piece.type)
    {
    case PAWN:
        if (PieceMovement::IsEnPassantValid(piece, newPosition, pieces, originalPosition))
        {
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
            if (MoveSimulation::CheckKingAfterMove(tempPiece, tempNewPos, tempPieces, originalPosition, board))
            {
                isValid = true;
            }
        }
        // Use temp copies for IsCastlingValid since it requires non-const references
        if (PieceMovement::IsCastlingValid(tempPiece, tempNewPos, tempPieces, originalPosition, board))
        {
            isValid = true;
        }
        break;
    default:
        isValid = false;
        break;
    }

    // After validating the move, check if it leaves our king in check
    if (isValid && piece.type != KING)
    {
        // Reset temp copies for simulation
        tempPiece = piece;
        tempPieces = pieces;
        tempNewPos = newPosition;

        if (!MoveSimulation::SimulateMoveForOur(tempPiece, tempNewPos, tempPieces, board))
        {
            return false;
        }
    }

    return isValid;
}

void MoveValidator::ExecuteMove(Piece &piece, Vector2 &newPosition,
                                std::vector<Piece> &pieces, const Vector2 &originalPosition,
                                Board &board)
{
    // Handle special moves based on piece type
    switch (piece.type)
    {
    case PAWN:
        if (PieceMovement::IsEnPassantValid(piece, newPosition, pieces, originalPosition))
        {
            board.ExecuteEnPassant(piece, pieces, originalPosition, newPosition);
        }
        // Check for pawn promotion
        {
            int newY = static_cast<int>((newPosition.y - boardPosition.y) / squareSize);
            int promotionRank = (piece.color == 0) ? 7 : 0;
            if (newY == promotionRank)
            {
                board.PawnPromo = true;
                board.promotionPosition = newPosition;
                board.p1 = (piece.color == 0) ? 0 : 1;
            }
        }
        break;
    case ROOK:
        piece.hasMoved = true;
        break;
    case KING:
        piece.hasMoved = true;
        if (PieceMovement::IsCastlingValid(piece, newPosition, pieces, originalPosition, board))
        {
            bool kingside = newPosition.x > originalPosition.x;
            Board::ExecuteCastling(piece, kingside, pieces, originalPosition);
        }
        break;
    default:
        break;
    }

    // Update lastMove for en passant detection and move highlighting
    MoveSimulation::lastMove = std::make_tuple(piece, originalPosition, newPosition);

    // Check if opponent's king is in check (for future check/checkmate detection)
    MoveSimulation::SimulateMove(piece, newPosition, pieces, board);
}

bool MoveValidator::IsMoveValid(Piece &piece, Vector2 &newPosition, std::vector<Piece> &pieces, const Vector2 &originalPosition, Board &board, bool forHighlightOnly)
{

    bool isValid = false;

    // Validating move based on piece type
    switch (piece.type)
    {
    case PAWN:
        if (PieceMovement::IsEnPassantValid(piece, newPosition, pieces, originalPosition))
        {
            // Only execute en passant when not just checking for highlighting
            if (!forHighlightOnly)
            {
                board.ExecuteEnPassant(const_cast<Piece &>(piece), const_cast<std::vector<Piece> &>(pieces), originalPosition, newPosition);
            }
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
                // Only set hasMoved when actually executing the move
                if (!forHighlightOnly)
                {
                    piece.hasMoved = true;
                }
                isValid = true;
            }
        }
        if (PieceMovement::IsCastlingValid(piece, newPosition, pieces, originalPosition, board))
        {
            // Only execute castling when actually making the move, not for highlighting
            if (!forHighlightOnly)
            {
                bool kingside = newPosition.x > originalPosition.x;
                Board::ExecuteCastling(const_cast<Piece &>(piece), kingside, const_cast<std::vector<Piece> &>(pieces), originalPosition);
            }
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
            return false;
        }
    }

    // Checking if opponent's king is in check after move (for future check/checkmate detection)
    MoveSimulation::SimulateMove(const_cast<Piece &>(piece), newPosition, const_cast<std::vector<Piece> &>(pieces), board);

    // For pawn promotion - only trigger when actually making the move, not for highlighting
    if (isValid && piece.type == PAWN && !forHighlightOnly)
    {
        int newY = static_cast<int>((newPosition.y - boardPosition.y) / squareSize);
        int promotionRank = (piece.color == 0) ? 7 : 0;

        if (newY == promotionRank)
        {
            board.PawnPromo = true;
            board.promotionPosition = newPosition;
            board.p1 = (piece.color == 0) ? 0 : 1;
        }
    }

    // Only update lastMove when actually executing a move, not when checking for highlighting
    if (isValid && !forHighlightOnly)
    {
        MoveSimulation::lastMove = std::make_tuple(piece, originalPosition, newPosition); // Storing the move
    }
    return isValid;
}

bool MoveValidator::IsCheckmate(std::vector<Piece> &pieces, int kingColor, Board &board)
{
    Vector2 kingPosition = (kingColor == 1) ? board.whiteKingPosition : board.blackKingPosition;

    // First, check if the king is in check
    if (!MoveValidator::IsKingInCheck(pieces, kingPosition, kingColor, board))
    {
        return false; // Not checkmate if the king is not in check
    }

    // Try all possible moves for all pieces of the same color
    for (auto &piece : pieces)
    {
        if (piece.color == kingColor && !piece.captured)
        {
            Vector2 originalPosition = piece.position;

            // Generate potential moves based on piece type
            std::vector<Vector2> potentialMoves;

            switch (piece.type)
            {
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

            // Check if any move is legal (IsMoveLegal validates piece rules AND checks if king stays safe)
            for (const Vector2 &move : potentialMoves)
            {
                if (MoveValidator::IsMoveLegal(piece, move, pieces, originalPosition, board))
                {
                    return false; // Found a valid escape move
                }
            }
        }
    }

    // No valid moves found that get the king out of check
    return true;
}

bool MoveValidator::IsMoveInValidMoves(const Vector2 &targetPosition, const std::vector<Vector2> &validMoves)
{
    for (const auto &move : validMoves)
    {

        if (std::abs(move.x - targetPosition.x) < 1.0f &&
            std::abs(move.y - targetPosition.y) < 1.0f)
        {
            return true;
        }
    }
    return false;
}