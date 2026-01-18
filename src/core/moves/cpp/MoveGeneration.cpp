#include "../hpp/MoveGeneration.hpp"
#include "../hpp/MoveUtils.hpp"
#include "../hpp/MoveSimulation.hpp"
#include "../hpp/MoveValidator.hpp"
#include "../hpp/PieceMovement.hpp"
#include "../../Board.hpp"
#include "../../Constants.hpp"

#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <algorithm>
#include <cmath>

namespace MoveGeneration {

std::vector<Vector2> GetAllPossibleMoves(Piece& piece, std::vector<Piece>& pieces, Board& board) {
    
    std::vector<Vector2> moves;
    std::vector<Vector2> generatedMoves;

    // Generate potential moves based on piece type
    switch (piece.type) {
        case PAWN:
            generatedMoves = GetPawnMoves(piece, pieces, board); 
            break;
        case ROOK:
            generatedMoves = GetRookMoves(piece, pieces, board); 
            break;
        case BISHOP:
            generatedMoves = GetBishopMoves(piece, pieces, board); 
            break;
        case QUEEN:
            generatedMoves = GetQueenMoves(piece, pieces, board); 
            break;
        case KNIGHT:
            generatedMoves = GetKnightMoves(piece, pieces, board);  
            break;
        case KING:
            generatedMoves = GetKingMoves(piece, pieces, board); 
            break;
        default:
            std::cout << "Unknown piece type: " << piece.type << std::endl;
            break;
    }

        // Validating each generated move (for highlighting only - no side effects)
        for (size_t i = 0; i < generatedMoves.size(); i++)
        {
            // Make a deep copy BEFORE validation - IsMoveValid modifies the Vector2 reference!
            float origX = generatedMoves[i].x;
            float origY = generatedMoves[i].y;
            Vector2 moveToValidate = {origX, origY};
            
            bool valid = MoveValidator::IsMoveValid(piece, moveToValidate, pieces, piece.position, board, true);
            
            if (valid)
            {
                // Store the ORIGINAL coordinates, not the modified ones
                moves.push_back({origX, origY});
            }
        }

    return moves;
}

    std::vector<Vector2> GetPawnMoves(const Piece &piece, const std::vector<Piece> &pieces, const Board &board)
    {
        std::vector<Vector2> moves;

        int direction = (piece.color == 0) ? 1 : -1;

        // Generating potential moves (one step forward, two steps forward, and diagonal captures including en passant)
        // These will all be validated by IsMoveValid which checks both IsPawnMoveValid AND IsEnPassantValid
        
        // Forward move
        moves.push_back({piece.position.x, piece.position.y + squareSize * direction});
        
        // Double move from starting position
        moves.push_back({piece.position.x, piece.position.y + 2 * squareSize * direction});
        
        // Diagonal captures (also covers en passant positions)
        moves.push_back({piece.position.x + squareSize, piece.position.y + squareSize * direction});
        moves.push_back({piece.position.x - squareSize, piece.position.y + squareSize * direction});

        return moves;
    }

std::vector<Vector2> GetRookMoves(const Piece& piece, const std::vector<Piece>& pieces, const Board& board) {
    std::vector<Vector2> moves;

    // Generating potential horizontal and vertical moves
    std::vector<Vector2> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

        for (const auto &direction : directions)
        {
            Vector2 newPosition = piece.position;
            while (true)
            {
                newPosition.x += direction.x * squareSize;
                newPosition.y += direction.y * squareSize;
                // Snap to grid to avoid floating-point drift
                newPosition = MoveUtils::SnapToGrid(newPosition, boardPosition, squareSize);
                if (PieceMovement::IsRookMoveValid(piece, newPosition, pieces, piece.position))
                {
                    moves.push_back(newPosition);
                    // Stop if a piece is captured
                    if (std::any_of(pieces.begin(), pieces.end(), [&](const Piece &p)
                                    { return !p.captured && Vector2Equals(p.position, newPosition); }))
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
        }

    return moves;
}

std::vector<Vector2> GetBishopMoves(const Piece& piece, const std::vector<Piece>& pieces, const Board& board) {
    
    std::vector<Vector2> moves;

    std::vector<Vector2> directions = {
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
    };

        // Iterating through each diagonal direction
        for (const auto &direction : directions)
        {
            Vector2 newPosition = piece.position;
            while (true)
            {

                // Moving to the next diagonal position
                newPosition.x += direction.x * squareSize;
                newPosition.y += direction.y * squareSize;
                // Snap to grid to avoid floating-point drift
                newPosition = MoveUtils::SnapToGrid(newPosition, boardPosition, squareSize);

                // Converting pixel position to board coordinates
                int newX = MoveUtils::PixelToBoard(newPosition.x, boardPosition.x, squareSize);
                int newY = MoveUtils::PixelToBoard(newPosition.y, boardPosition.y, squareSize);

                if (!MoveUtils::IsInBounds(newX, newY))
                {
                    break;
                }

                // Checking if there's any piece at the new position
                auto it = std::find_if(pieces.begin(), pieces.end(), [&](const Piece &p)
                                       { return !p.captured && Vector2Equals(p.position, newPosition); });

            // If a piece is found
            if (it != pieces.end()) {
                
                // Checking if it's an enemy piece
                if (it->color != piece.color) {
                    moves.push_back(newPosition); // Valid capture
                }
                break; // Path is blocked, stoping further exploration

            } else {
                // No piece found, valid move
                moves.push_back(newPosition);
            }
        }
    }

    return moves;
}

std::vector<Vector2> GetQueenMoves(const Piece& piece, const std::vector<Piece>& pieces, const Board& board) {
    
    std::vector<Vector2> moves;

    // Generate potential moves combining rook and bishop moves
    auto rookMoves = GetRookMoves(piece, pieces, board);
    auto bishopMoves = GetBishopMoves(piece, pieces, board);

    // Combine both move sets
    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());

    return moves;
}

std::vector<Vector2> GetKnightMoves(const Piece& piece, const std::vector<Piece>& pieces, const Board& board) {
    
    std::vector<Vector2> moves;
    // Generating all possible L-shaped moves
    std::vector<Vector2> potentialMoves = {
        {piece.position.x + 2 * squareSize, piece.position.y + squareSize},
        {piece.position.x + 2 * squareSize, piece.position.y - squareSize},
        {piece.position.x - 2 * squareSize, piece.position.y + squareSize},
        {piece.position.x - 2 * squareSize, piece.position.y - squareSize},
        {piece.position.x + squareSize, piece.position.y + 2 * squareSize},
        {piece.position.x + squareSize, piece.position.y - 2 * squareSize},
        {piece.position.x - squareSize, piece.position.y + 2 * squareSize},
        {piece.position.x - squareSize, piece.position.y - 2 * squareSize}
    };

    for (const auto& newPosition : potentialMoves) {
        if (PieceMovement::IsKnightMoveValid(piece, newPosition, pieces, piece.position)) {
            moves.push_back(newPosition);
        }
    }

    return moves;
}

std::vector<Vector2> GetKingMoves(Piece& piece, const std::vector<Piece>& pieces, const Board& board) {
    
    std::vector<Vector2> moves;

        // Generating potential king moves (one square in any direction)
        Vector2 potentialMoves[] = {
            {piece.position.x + squareSize, piece.position.y},              // Right
            {piece.position.x - squareSize, piece.position.y},              // Left
            {piece.position.x, piece.position.y + squareSize},              // Down
            {piece.position.x, piece.position.y - squareSize},              // Up
            {piece.position.x + squareSize, piece.position.y + squareSize}, // Down-right
            {piece.position.x - squareSize, piece.position.y + squareSize}, // Down-left
            {piece.position.x + squareSize, piece.position.y - squareSize}, // Up-right
            {piece.position.x - squareSize, piece.position.y - squareSize}  // Up-left
        };

        for (const auto &newPosition : potentialMoves)
        {
            if (PieceMovement::IsKingMoveValid(piece, newPosition, pieces, piece.position))
            {
                moves.push_back(newPosition);
            }
        }

        // Add castling moves (king moves 2 squares horizontally)
        // Kingside castling (move right 2 squares)
        Vector2 kingsideCastle = {piece.position.x + 2 * squareSize, piece.position.y};
        moves.push_back(kingsideCastle);

        // Queenside castling (move left 2 squares)
        Vector2 queensideCastle = {piece.position.x - 2 * squareSize, piece.position.y};
        moves.push_back(queensideCastle);

        return moves;
    }

}