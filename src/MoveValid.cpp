#include "MoveValid.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

std::tuple<Piece, Vector2, Vector2> MoveValidator::lastMove = std::make_tuple(Piece{}, Vector2{0.0f, 0.0f}, Vector2{0.0f, 0.0f});


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

    int startRow = (piece.color == 0) ? 1 : 6;   // Starting row for pawns
    int direction = (piece.color == 0) ? 1 : -1;

    // Checking if new position is within board bounds
    if (newX < 0 || newX > 7 || newY < 0 || newY > 7) {
        return false; // Out of bounds
    }

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
    if (newX == currentX && newY == currentY + 2 * direction && currentY == startRow) {

        // Checking that both square directly in front and two squares in front are clear
        Vector2 intermediatePosition = {originalPosition.x, originalPosition.y + direction * squareSize};
        for (const auto &otherPiece : pieces) {
            if (Vector2Equals(otherPiece.position, intermediatePosition) || Vector2Equals(otherPiece.position, newPosition)) {
                return false; // Blocked by another piece
            }
        }
        return true;
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

    // Checking if new position is within board bounds
    if (newX < 0 || newX > 7 || newY < 0 || newY > 7) {
        return false; // Out of bounds
    }

    if(currentX != newX && currentY != newY)//To check if the Rook is moving vertically and horizontally
    {
        return false;
    }

    // To determine direction and to check each square in the path
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

    // To Check destination square(For Capture)
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

bool MoveValidator::IsBishopMoveValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition) {
   
    int currentX = static_cast<int>((originalPosition.x - boardPosition.x) / squareSize);
    int currentY = static_cast<int>((originalPosition.y - boardPosition.y) / squareSize);
    int newX = static_cast<int>((newPosition.x - boardPosition.x) / squareSize);
    int newY = static_cast<int>((newPosition.y - boardPosition.y) / squareSize);

    // std::cout << "Original Pos: (" << currentX << ", " << currentY << "), New Pos: (" << newX << ", " << newY << ")" << std::endl;

    int dx = newX - currentX;
    int dy = newY - currentY;

    if (newX < 0 || newX > 7 || newY < 0 || newY > 7) {
        return false; // Out of bounds
    }

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

    while (x != newX || y != newY) {
        if (x < 0 || x > 7 || y < 0 || y > 7) {
        // std::cout << "Intermediate Pos Out of Bounds: (" << x << ", " << y << ")" << std::endl;
        return false; // Position out of bounds, break out early
    }
        Vector2 intermediatePosition = {boardPosition.x + x * squareSize, boardPosition.y + y * squareSize};
        // std::cout << "Intermediate Pos: (" << x << ", " << y << ")" << std::endl;
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

    if (currentX == newX || currentY == newY) {
        return IsRookMoveValid(piece, newPosition, pieces, originalPosition);
    }

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

    if (newX < 0 || newX > 7 || newY < 0 || newY > 7) {
        return false;
    }

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

    int currentX = static_cast<int>((originalPosition.x - boardPosition.x) / squareSize);
    int currentY = static_cast<int>((originalPosition.y - boardPosition.y) / squareSize);
    int newX = static_cast<int>((newPosition.x - boardPosition.x) / squareSize);
    int newY = static_cast<int>((newPosition.y - boardPosition.y) / squareSize);

    if (newX < 0 || newX > 7 || newY < 0 || newY > 7) {
        return false;
    }

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
        return true; // Valid move, no piece blocking path
    }
    return false;// Invalid move for King(only one square.)
}

bool MoveValidator::IsEnPassantValid(const Piece& piece, const Vector2& newPosition, const std::vector<Piece>& pieces, const Vector2& originalPosition) {
   
    int currentX = static_cast<int>((originalPosition.x - boardPosition.x) / squareSize);
    int currentY = static_cast<int>((originalPosition.y - boardPosition.y) / squareSize);
    int newX = static_cast<int>((newPosition.x - boardPosition.x) / squareSize);
    int newY = static_cast<int>((newPosition.y - boardPosition.y) / squareSize);

    int dx = newX - currentX;
    int dy = newY - currentY;
    int direction = (piece.color == 0) ? 1 : -1;

    // Checking if the pawn is attempting an en passant move
    if (dy == direction && std::abs(dx) == 1) {
        const Piece& lastMovePiece = std::get<0>(lastMove);
        const Vector2& lastOriginalPosition = std::get<1>(lastMove);
        const Vector2& lastNewPosition = std::get<2>(lastMove);

        // Checking if last move was a double pawn move by the opponent
        if (lastMovePiece.type == PAWN && lastMovePiece.color != piece.color) {
            
            // std::cout<<"I am Inside En Passant"<<std::endl;
            int lastMoveStartY = static_cast<int>((lastOriginalPosition.y - boardPosition.y) / squareSize);
            int lastMoveEndY = static_cast<int>((lastNewPosition.y - boardPosition.y) / squareSize);

            if (std::abs(lastMoveEndY - lastMoveStartY) == 2) {
                
                // std::cout<<"Last Move was a double pawn move"<<std::endl;
                // Checking if pawn moved beside the current pawn
                int enPassantX = static_cast<int>((lastNewPosition.x - boardPosition.x) / squareSize);
                int enPassantY = lastMoveStartY - direction; // Where the opponent's pawn landed
                // std::cout << "enPassantX: " << enPassantX << " enPassantY: " << enPassantY << std::endl;
                // std::cout << "newX: " << newX << " newY: " << newY << std::endl;
                if (enPassantX == newX && enPassantY == newY) {
                    std::cout<<"It is a valid En Passant Move"<<std::endl;
                    return true; // Valid en passant move
                }
            }
        }
    }
    return false; // Not a valid en passant move
}

bool MoveValidator::IsCastlingValid(Piece &king, const Vector2 &newPosition, std::vector<Piece> &pieces, const Vector2 &originalPosition, Board &board)
{
    if (king.type != KING) {
        return false;
    }

    int currentX = static_cast<int>((originalPosition.x - boardPosition.x) / squareSize);
    int newX = static_cast<int>((newPosition.x - boardPosition.x) / squareSize);

    int dx = (newX - currentX);
    if (std::abs(dx) != 2) {
        return false; // For Castling must move exactly 2 squares horizontally
    }

    bool kingside = dx > 0;
    int rookX = kingside ? 7 : 0; 
    int rookY = static_cast<int>((originalPosition.y - boardPosition.y) / squareSize);

    // Finding rook
    Piece* rook = nullptr;
    for (auto &piece : pieces) {
        if (piece.type == ROOK && piece.color == king.color 
        && static_cast<int>((piece.position.x - boardPosition.x) / squareSize) == rookX &&
            static_cast<int>((piece.position.y - boardPosition.y) / squareSize) == rookY) {
            rook = &piece;
            break;
        }
    }
    if (!rook || king.hasMoved || rook->hasMoved) {
        return false;
    }

    int step = kingside ? 1 : -1;
    for (int x = currentX + step; x != rookX; x += step) {
        Vector2 intermediatePosition = {boardPosition.x + x * squareSize, boardPosition.y + rookY * squareSize};

        // Temporarily moving king to this intermediate position
        Vector2 tempKingPosition = king.position;
        king.position = intermediatePosition;

        // Checking if the king would be in check at this position
        if (IsKingInCheck(pieces, intermediatePosition, king.color, board)) {
            king.position = tempKingPosition; // Restore original position
            std::cout<<"Checking for King in Check:: False"<<std::endl;
            return false; // Castling is not allowed as king would be in check
        }
        

        // Restore the king's position
        king.position = tempKingPosition;

        for (const auto &piece : pieces) {
            if (Vector2Equals(piece.position, intermediatePosition)) {
                return false;
            }
        }
    }

    if(IsKingInCheck(pieces, originalPosition, king.color, board)){
        std::cout<<"King is in check(No Castle.)"<<std::endl;
        return false;//Can't castle while in check.
    }

    // Final check: Ensuring that king isn't moving into check at the castling destination
    Vector2 finalKingPosition = newPosition;
    if (IsKingInCheck(pieces, finalKingPosition, king.color, board)) {
        std::cout<<"Destination check:"<<std::endl;
        return false;
    }

    return true;
}

bool MoveValidator::ForPawnCaptureValid(const Piece &piece, const Vector2 &newPosition, const std::vector<Piece> &pieces, const Vector2 &originalPosition) {
    
    int direction = (piece.color == 0) ? 1 : -1; // Here 0 is white and 1 is black
    Vector2 attackLeft = { originalPosition.x - squareSize, originalPosition.y + (squareSize * direction) };
    Vector2 attackRight = { originalPosition.x + squareSize, originalPosition.y + (squareSize * direction) };

    // Checking if the new position matches one of the attack positions
    if (Vector2Equals(newPosition, attackLeft) || Vector2Equals(newPosition, attackRight)) {
        return true;
    }

    return false;
}


bool MoveValidator::IsKingInCheck(const std::vector<Piece>& pieces, const Vector2& kingPosition, int kingColor, const Board& board) {
  
    for (const auto& piece : pieces) {
        if (!piece.captured && piece.color != kingColor) { // Checking only opponent's pieces
            if (CanPieceAttack(piece, kingPosition, pieces)) {
                std::cout << "King is in check by piece of type " << piece.type << " at position (" << piece.position.x << "," << piece.position.y << ")" << std::endl;
                return true; // King is in check
            }
        }
    }
    return false; // King is not in check
}

bool MoveValidator::CanPieceAttack(const Piece& piece, const Vector2& targetPosition, const std::vector<Piece>& pieces) {
    
    //  std::cout << "Checking if piece of type(canpieceAttack) " << piece.type << " at position (" << piece.position.x << ", " << piece.position.y << ") can attack target position (" << targetPosition.x << ", " << targetPosition.y << ")." << std::endl;
    switch (piece.type) {
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

bool MoveValidator::SimulateMoveForOur(Piece& piece, const Vector2& newPosition, std::vector<Piece>& pieces, Board& board) {
    
    Vector2 tempPosition = piece.position;
    Piece* capturedPiece = nullptr;

    // Checking if the move would capture a piece
    for (Piece& otherPiece : pieces) {
        if (Vector2Equals(otherPiece.position, newPosition) && otherPiece.color != piece.color) {
            capturedPiece = &otherPiece;
             std::cout << "Simulating capture of piece of type " << otherPiece.type << " at position (" << otherPiece.position.x << ", " << otherPiece.position.y << ")." << std::endl;
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
    bool isInCheck = IsKingInCheck(pieces, kingPosition, piece.color, board);

    // Revert the move
    piece.position = tempPosition;
    if (capturedPiece) {
        capturedPiece->captured = false;
    }

     std::cout << "Move simulation results: Our King " << (isInCheck ? "is in check" : "is not in check") << "." << std::endl;
    return !isInCheck;
}

bool MoveValidator::SimulateMove(Piece& piece, const Vector2& newPosition, std::vector<Piece>& pieces, Board& board) {
    
    Vector2 tempPosition = piece.position;
    Piece* capturedPiece = nullptr;

    for (Piece& otherPiece : pieces) {
        if (Vector2Equals(otherPiece.position, newPosition) && otherPiece.color != piece.color) {
            capturedPiece = &otherPiece;
            std::cout << "Simulating capture of piece of type " << otherPiece.type << " at position (" << otherPiece.position.x << ", " << otherPiece.position.y << ")." << std::endl;
            break;
        }
    }

    piece.position = newPosition;
    if (capturedPiece) {
        capturedPiece->captured = true;
    }

    Vector2 opponentKingPosition = (piece.color == 0) ? board.whiteKingPosition : board.blackKingPosition;
    bool isInCheck = IsKingInCheck(pieces, opponentKingPosition, 1 - piece.color, board);

    piece.position = tempPosition;
    if (capturedPiece) {
        capturedPiece->captured = false;
    }

    std::cout << "Move simulation results: Opponent King " << (isInCheck ? "is in check" : "is not in check") << "." << std::endl;
    return !isInCheck;
}

bool MoveValidator::IsMoveValid(Piece &piece, Vector2 &newPosition,std::vector<Piece> &pieces, const Vector2 &originalPosition, Board &board) {
    
    bool isValid = false;

    // Validating move based on piece type
    switch (piece.type) {
        case PAWN:
            if (IsEnPassantValid(piece, newPosition, pieces, originalPosition)) {
                board.ExecuteEnPassant(const_cast<Piece&>(piece), const_cast<std::vector<Piece>&>(pieces), originalPosition, newPosition);
                isValid = true;
            } else {
                isValid = IsPawnMoveValid(piece, newPosition, pieces, originalPosition);
            }
            break;
        case ROOK:
            isValid = IsRookMoveValid(piece, newPosition, pieces, originalPosition);
            break;
        case BISHOP:
            isValid = IsBishopMoveValid(piece, newPosition, pieces, originalPosition);
            break;
        case QUEEN:
            isValid = IsQueenMoveValid(piece, newPosition, pieces, originalPosition);
            break;
        case KNIGHT:
            isValid = IsKnightMoveValid(piece, newPosition, pieces, originalPosition);
            break;
        case KING:
            if (IsKingMoveValid(piece, newPosition, pieces, originalPosition)) {
                if(CheckKingAfterMove(piece, newPosition, pieces, originalPosition, board)){
                    piece.hasMoved = true;
                    isValid = true;
                }
            }
            if (IsCastlingValid(piece, newPosition, pieces, originalPosition, board)) {
                bool kingside = newPosition.x > originalPosition.x;
                Board::ExecuteCastling(const_cast<Piece&>(piece), kingside, const_cast<std::vector<Piece>&>(pieces), originalPosition);
                isValid = true;
            }
            break;
        default:
            std::cout << "Invalid move: Unknown piece type" << std::endl;
            isValid = false;
            break;
    }

    // After validating the move, checking if it leaves king in check
    if (isValid && piece.type != KING) {
        if (!SimulateMoveForOur(const_cast<Piece&>(piece), newPosition, const_cast<std::vector<Piece>&>(pieces), board)) {
            std::cout << "Move would leave the king in check!" << std::endl;
            return false;
        }
    }

    // Checking if opponent's king is in check after move
    if (!SimulateMove(const_cast<Piece&>(piece), newPosition, const_cast<std::vector<Piece>&>(pieces), board)&&isValid) {
        std::cout << "Opponent's king Checking done." << std::endl;
    }



    // For pawn promotion
    if (isValid && piece.type == PAWN) {
        int newY = static_cast<int>((newPosition.y - boardPosition.y) / squareSize);
        int promotionRank = (piece.color == 0) ? 7 : 0;

        if (newY == promotionRank) {
            board.PawnPromo = true;
            board.promotionPosition = newPosition;
            board.p1 = (piece.color == 0) ? 0 : 1;
            std::cout << "Promoting pawn" << std::endl;
        }
    }

    if (isValid) {
        lastMove = std::make_tuple(piece, originalPosition, newPosition); // Storing the move
        std::cout << "Move is valid." << std::endl;
    }
    return isValid;
}

bool MoveValidator::CheckKingAfterMove(Piece &king, Vector2 &newPosition, std::vector<Piece> &pieces, const Vector2 &originalPosition, Board &board) {
    
    // Temporarily storing current position of the king
    Vector2 tempKingPosition = king.position;

    // Storing references to the piece at new position, if any
    Piece* capturedPiece = nullptr;
    bool isCaptured = false;
    
    // Find and temporarily mark the piece at the new position as captured (if any)
    for (auto& piece : pieces) {
        if (Vector2Equals(piece.position, newPosition) && !piece.captured) {
            capturedPiece = &piece;
            piece.captured = true;
            isCaptured = true;
            break;
        }
    }

    // Moving the king to new position
    king.position = newPosition;

    // Updating the board with the king's new position
    if (king.color == 0) {
        board.blackKingPosition = newPosition;
    } else {
        board.whiteKingPosition = newPosition;
    }

    // Checking if the king is still in check after moving
    bool isInCheck = false;
    for (const auto& piece : pieces) {
        if (piece.color != king.color && !piece.captured) {
            if (CanPieceAttack(piece, newPosition, pieces)) {
                std::cout << "King would be captured at new position." << std::endl;
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

    // Restoring the board state
    if (king.color == 0) {
        board.blackKingPosition = tempKingPosition;
    } else {
        board.whiteKingPosition = tempKingPosition;
    }


    if (isInCheck) {
        std::cout << "Move is invalid: King is still in check after the move." << std::endl;
        return false; //King is in check after the move
    }

    // Performing a final check if the king is still in check after all moves
    bool finalCheck = IsKingInCheck(pieces, newPosition, king.color, board);

    // Return false if the king is in check; otherwise, return true
    return !finalCheck;
}


bool MoveValidator::IsCheckmate(std::vector<Piece>& pieces, int kingColor, Board& board) {
    
    std::cout << "Checking for checkmate..." << std::endl;
    Vector2 kingPosition = (kingColor == 1) ? board.whiteKingPosition : board.blackKingPosition;

    // First, for checking if the king is in check
    if (!IsKingInCheck(pieces, kingPosition, kingColor, board)) {
        std::cout << "King is not in check" << std::endl;
        return false; // Not checkmate if the king is not in check
    }

    // Simulating all possible moves for the opponent
    std::cout << "Pass" << std::endl;
    for (auto& piece : pieces) {
        if (piece.color == kingColor && !piece.captured) {
            std::vector<Vector2> possibleMoves = GetAllPossibleMoves(piece, pieces, board); 

            for ( Vector2& move : possibleMoves) {
                std::cout << "Checking move for piece of type " << piece.type 
                          << " at position (" << piece.position.x << "," << piece.position.y 
                          << ") to position (" << move.x << "," << move.y << ")" << std::endl;

                Piece tempPiece = piece; // A copy to simulate
                Vector2 originalPosition = tempPiece.position;
                tempPiece.position = move;

                // Log simulation
                std::cout << "Simulating move..." << std::endl;
                if(IsMoveValid(tempPiece, move, pieces, piece.position ,board)){
                    std::cout << "Move simulated. Checking if king is in check..." << std::endl;
                    if (!IsKingInCheck(pieces , move , kingColor, board)) {
                        std::cout << "Found a valid move that gets the king out of check 1" << std::endl;
                        return false; // Found a valid move that gets the king out of check, so it's not checkmate
                    }
                }
                // Restoring the original position
                tempPiece.position = originalPosition;

                //Checks for blocks and captures 
                if(!SimulateMoveAndCheck(pieces, piece, move, kingColor, board)){
                    std::cout << "Found a valid move that gets the king out of check 2" << std::endl;
                    return false;
                }
            }
        }
    }

    // No valid moves found that get the king out of check, so it's checkmate
    return true;
}

std::vector<Vector2> MoveValidator::GetAllPossibleMoves(Piece& piece, std::vector<Piece>& pieces, Board& board) {
    
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
            generatedMoves = GetValidBishopMoves(piece, pieces, board); 
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

    std::cout << "Generated moves for piece of type " << piece.type << " at position (" 
              << piece.position.x << "," << piece.position.y << "):" << std::endl;
    for (const Vector2& move : generatedMoves) {
        std::cout << "    Potential move to (" << move.x << "," << move.y << ")" << std::endl;
    }

    // Validating each generated move
    for (Vector2& potentialMove : generatedMoves) {
        if (IsMoveValid(piece, potentialMove, pieces, piece.position, board)) {
            std::cout << "    Valid move to (" << potentialMove.x << "," << potentialMove.y << ")" << std::endl;
            moves.push_back(potentialMove);
        } else {
            std::cout << "    Invalid move to (" << potentialMove.x << "," << potentialMove.y << ")" << std::endl;
        }
    }

    return moves;
}


std::vector<Vector2> MoveValidator::GetPawnMoves(const Piece& piece, const std::vector<Piece>& pieces, const Board& board) {
    std::vector<Vector2> moves;
    
    // Generating potential moves (one step forward, two steps forward, and captures)
    Vector2 potentialMoves[] = {
        {piece.position.x, piece.position.y + squareSize * ((piece.color == 0) ? 1 : -1)},
        {piece.position.x, piece.position.y + 2 * squareSize * ((piece.color == 0) ? 1 : -1)},
        {piece.position.x + squareSize, piece.position.y + squareSize * ((piece.color == 0) ? 1 : -1)},
        {piece.position.x - squareSize, piece.position.y + squareSize * ((piece.color == 0) ? 1 : -1)}
    };

    for (const auto& newPosition : potentialMoves) {
        if (IsPawnMoveValid(piece, newPosition, pieces, piece.position)) {
            moves.push_back(newPosition);
        }
    }

    return moves;
}

std::vector<Vector2> MoveValidator::GetRookMoves(const Piece& piece, const std::vector<Piece>& pieces, const Board& board) {
    std::vector<Vector2> moves;

    // Generating potential horizontal and vertical moves
    std::vector<Vector2> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    for (const auto& direction : directions) {
        Vector2 newPosition = piece.position;
        while (true) {
            newPosition.x += direction.x * squareSize;
            newPosition.y += direction.y * squareSize;
            if (IsRookMoveValid(piece, newPosition, pieces, piece.position)) {
                moves.push_back(newPosition);
                // Stop if a piece is captured
                if (std::any_of(pieces.begin(), pieces.end(), [&](const Piece& p) { return Vector2Equals(p.position, newPosition); })) {
                    break;
                }
            } else {
                break;
            }
        }
    }

    return moves;
}

std::vector<Vector2> MoveValidator::GetValidBishopMoves(const Piece& piece, const std::vector<Piece>& pieces, const Board& board) {
    
    std::vector<Vector2> moves;

    std::vector<Vector2> directions = {
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
    };

    // Iterating through each diagonal direction
    for (const auto& direction : directions) {
        Vector2 newPosition = piece.position;
        while (true) {
            
            // Moving to the next diagonal position
            newPosition.x += direction.x * squareSize;
            newPosition.y += direction.y * squareSize;

            // Converting pixel position to board coordinates
            int newX = static_cast<int>((newPosition.x - boardPosition.x) / squareSize);
            int newY = static_cast<int>((newPosition.y - boardPosition.y) / squareSize);

            if (newX < 0 || newX > 7 || newY < 0 || newY > 7) {
                break;
            }

            // Checking if there's any piece at the new position
            auto it = std::find_if(pieces.begin(), pieces.end(), [&](const Piece& p) {
                return Vector2Equals(p.position, newPosition);
            });

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

std::vector<Vector2> MoveValidator::GetQueenMoves(const Piece& piece, const std::vector<Piece>& pieces, const Board& board) {
    
    std::vector<Vector2> moves;

    // Generate potential moves combining rook and bishop moves
    auto rookMoves = GetRookMoves(piece, pieces, board);
    auto bishopMoves = GetValidBishopMoves(piece, pieces, board);

    // Combine both move sets
    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());

    return moves;
}

std::vector<Vector2> MoveValidator::GetKnightMoves(const Piece& piece, const std::vector<Piece>& pieces, const Board& board) {
    
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
        if (IsKnightMoveValid(piece, newPosition, pieces, piece.position)) {
            moves.push_back(newPosition);
        }
    }

    return moves;
}

std::vector<Vector2> MoveValidator::GetKingMoves(Piece& piece, const std::vector<Piece>& pieces, const Board& board) {
    
    std::vector<Vector2> moves;

    // Generating potential king moves (one square in any direction)
    Vector2 potentialMoves[] = {
        {piece.position.x + squareSize, piece.position.y},        // Right
        {piece.position.x - squareSize, piece.position.y},        // Left
        {piece.position.x, piece.position.y + squareSize},        // Down
        {piece.position.x, piece.position.y - squareSize},        // Up
        {piece.position.x + squareSize, piece.position.y + squareSize},  // Down-right
        {piece.position.x - squareSize, piece.position.y + squareSize},  // Down-left
        {piece.position.x + squareSize, piece.position.y - squareSize},  // Up-right
        {piece.position.x - squareSize, piece.position.y - squareSize}   // Up-left
    };
    
  for (const auto& newPosition : potentialMoves) {

        if (IsKingMoveValid(piece, newPosition, pieces, piece.position)) {
            moves.push_back(newPosition);
        }
    }
    return moves;
}

bool MoveValidator::SimulateMoveAndCheck(std::vector<Piece>& pieces, Piece& piece, Vector2 newPosition, int kingColor, Board& board) {
    
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
    bool kingInCheck = IsKingInCheck(pieces, (kingColor == 1) ? board.whiteKingPosition : board.blackKingPosition, kingColor, board);

    // Reverting the move
    piece.position = originalPosition;
    if (capturedPiece) {
        capturedPiece->captured = false;
    }

    // Return's whether the king is still in check
    return kingInCheck;
}
