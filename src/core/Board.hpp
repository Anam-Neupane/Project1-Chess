#ifndef BOARD_H
#define BOARD_H

#include "Piece.hpp"
#include "GameState.hpp"
#include "Constants.hpp"
#include <raylib.h>
#include <vector>
#include "MoveHistory.hpp"

struct EngineMove; // Forward declaration -- full definition in EngineMove.hpp

class Board
{
private:
    GameState *gameState;      // Must be first - used by other members during init
    std::vector<Piece> pieces; // class object;
    float pieceWidth;
    float pieceHeight;
    Vector2 mousePos;
    bool dragging;
    int draggedPieceIndex;
    Vector2 originalPosition;
    Vector2 offset;
    Texture2D promotionTexture[12];
    MoveHistory moveHistory; // Stores full game transcript. 

    // Helper methods for board rotation
    Vector2 TransformPosition(Vector2 pos)
    {
        if (gameState->isBoardFlipped())
        {
            // Flip position: (0,0) become (7,7), and so on.
            return {
                (7 * squareSize) - pos.x,
                (7 * squareSize) - pos.y + 110};
        }
        return pos;
    }

    Vector2 TransformMouse(Vector2 mousePos)
    {
        if (gameState->isBoardFlipped())
        {
            return {
                (8 * squareSize) - mousePos.x,
                (8 * squareSize) - mousePos.y + 110};
        }
        return mousePos;
    }

    const int initialBoard[boardSize][boardSize] = {
        {1, 2, 3, 4, 5, 3, 2, 1},
        {6, 6, 6, 6, 6, 6, 6, 6},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {-6, -6, -6, -6, -6, -6, -6, -6},
        {-1, -2, -3, -4, -5, -3, -2, -1}};

    // For capturing and recording.
    int GetPieceValue(int pieceType);

    // For showing points
    Vector2 whiteScorePosition;
    Vector2 blackScorePosition;

    // For showing Player's Turn
    Vector2 playerturnPosition;

    bool showValidMoves;                    // Toggle state
    std::vector<Vector2> currentValidMoves; // Stores valid moves for
    Vector2 selectedPiecePosition;          // Position of piece (ORIGINAL position before drag)
    bool hasPieceSelected;                  // Is piece selected? for valid selection
    int selectedPieceType;                  // Type of selected piece (for en passant highlight)

    // Click-to-move selection state
    int clickSelectedPieceIndex = -1; // Index of piece selected via click (-1 = none)
    Vector2 clickOriginalPosition; // Original position of click-selected piece

    // For tracking captured pieces display positions
    int whiteCapturedCount;
    int blackCapturedCount;

    std::string posToUCI(Vector2 pos) const; // Converts pixel position to UCI square "e4"

    // Helper function for blur effect
    void DrawBlurredRectangle(float x, float y,float width, float height, Color baseColor, int blurLayers = 8);

public:
    bool PawnPromo = false;
    bool Checkmate = false;
    bool Stalemate = false;
    bool Resigned = false;
    int resignedPlayer = -1; // Color that resigned: 0 = black , 1 = white , -1 = nobody

    bool Cwhite = false;

    bool kingInCheck = false; // For king in check highlight 

    bool showMoveHistory = true; // toggle for the key-H

    bool p1;
    Board(GameState *state);
    ~Board();

    Vector2 promotionPosition; // Storing pawn's position for promotion
    Vector2 blackKingPosition;
    Vector2 whiteKingPosition;

    void Reset();
    void LoadPieces();
    void LoadPromotionTexture();
    void DrawPieces();
    void DrawLastMoveHightlight(); // For highlight of last move
    void UnloadPieces();
    void UpdateDragging();
    void HandleClickToMove();
    void CapturePiece(int capturedPieceIndex);
    void ExecuteEnPassant(Piece &capturingPawn, std::vector<Piece> &pieces, const Vector2 &originalPosition, const Vector2 &newPosition);
    void DrawScores();
    void DrawPlayer();
    static void ExecuteCastling(Piece &king, bool kingside, std::vector<Piece> &pieces, const Vector2 originalPosition);
    void DrawPromotionMenu(Vector2 position, int color);
    void HandlePawnPromotion(int color, Vector2 position);

    bool TryExecuteMove(int pieceIndex, Vector2 from, Vector2 to); // Execute a validated move: does what update draggign needed. 

    void ToggleShowValidMoves();    // Toggle the highlight features
    void DrawValidMoveHighlights(); // Draw the valid move indicators
    void ClearSelection();          // Clear selected piece and valid moves
    void DrawCheckHighlight();      // Draws a crimson glow under the king when in ckeck 

    void DrawMoveHistory(); // renders the side panel 
    
    bool ApplyEngineMove(const EngineMove& move); // Executes the engine move
    std::vector<std::string> uciMoveList;       // all MOves in UCI format: "e2e4", "e7e5"
    

};

#endif // BOARD_H