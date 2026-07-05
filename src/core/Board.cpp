#include "Board.hpp"
#include "moves/hpp/MoveValidator.hpp"
#include "moves/hpp/MoveGeneration.hpp"
#include "moves/hpp/MoveSimulation.hpp"
#include "../engine/EngineMove.hpp"
#include <raymath.h>
#include <iostream>
#include <string>
#include <cmath>
#include <unordered_set>
#include <algorithm> 

// For Global variable
float squareSize = 112.6;
Vector2 boardPosition = {0, 55};

std::string Board::posToUCI(Vector2 pos) const
{
    // Reverse of the pixel-to-board formula: col = round(x - boardx) / size) 
    int col = static_cast<int>(std::round((pos.x - boardPosition.x) / squareSize));
    int row = static_cast<int>(std::round((pos.y - boardPosition.y) / squareSize));

    char fileChar = static_cast<char>('a' + col); 
    char rankChar = static_cast<char>('8' - row); 

    return std::string(1, fileChar) + std::string(1, rankChar);
}

int Board::GetPieceValue(int pieceType)
{
    switch (pieceType)
    {
    case PAWN:
        return 1;
    case KNIGHT:
        return 3;
    case BISHOP:
        return 3;
    case ROOK:
        return 5;
    case QUEEN:
        return 9;
    default:
        return 0;
    }
}
// Constructor
Board::Board(GameState *state) : gameState(state), dragging(false), draggedPieceIndex(-1),
                                 promotionTexture{},
                                 whiteScorePosition({boardPosition.x + 5, boardPosition.y + squareSize * 8 + 10}), // Initialization of the Score Position
                                 blackScorePosition({boardPosition.x + 5, boardPosition.y - 25}),

                                 // Initialization of Player's Turn Position
                                 playerturnPosition({boardPosition.x + squareSize * 8 + 93, boardPosition.y + squareSize * 4 - 29}),
                                 showValidMoves(true),            // Default on
                                 selectedPiecePosition({-1, -1}), // No piece selected
                                 hasPieceSelected(false),
                                 selectedPieceType(PAWN),
                                 whiteCapturedCount(0),
                                 blackCapturedCount(0),
                                 p1(1),
                                 blackKingPosition({boardPosition.x + 4 * squareSize, boardPosition.y}),
                                 whiteKingPosition({boardPosition.x + 4 * squareSize, boardPosition.y + 7 * squareSize})
{
}

void Board::DrawScores()
{ // Drawing scores
    std::string whiteScoreText = "White : " + std::to_string(gameState->getWhiteScore());
    std::string blackScoreText = "Black : " + std::to_string(gameState->getBlackScore());

    // Swap score positions when board is flipped
    if (gameState->isBoardFlipped())
    {
        // White score at top, Black score at bottom (flipped)
        DrawText(whiteScoreText.c_str(), blackScorePosition.x, blackScorePosition.y, 30, WHITE);
        DrawText(blackScoreText.c_str(), whiteScorePosition.x, whiteScorePosition.y, 30, WHITE);
    }
    else
    {
        // Normal: White at bottom, Black at top
        DrawText(whiteScoreText.c_str(), whiteScorePosition.x, whiteScorePosition.y, 30, WHITE);
        DrawText(blackScoreText.c_str(), blackScorePosition.x, blackScorePosition.y, 30, WHITE);
    }
}

void Board::DrawPlayer()
{
    if (gameState->getCurrentPlayer() == 1)
    {

        DrawRectangle(playerturnPosition.x - 35, playerturnPosition.y - 6, 300, 45, WHITE);
        DrawText("WHITE's Turn", playerturnPosition.x - 30, playerturnPosition.y, 40, BLACK);
    }
    else
    {
        DrawRectangle(playerturnPosition.x - 35, playerturnPosition.y - 6, 300, 45, BLACK);
        DrawText("BLACK's Turn", playerturnPosition.x - 30, playerturnPosition.y, 40, WHITE);
    }
}

static void UnloadPromotionTextures(Texture2D textures[], int count,
                                    const std::unordered_set<unsigned int> &skipIds)
{
    for (int i = 0; i < count; i++)
    {
        if (textures[i].id != 0 && skipIds.find(textures[i].id) == skipIds.end())
        {
            UnloadTexture(textures[i]);
        }
        textures[i] = {0};
    }
}

static int ResolvePromotionColor(const std::vector<Piece> &pieces, const Vector2 &promotionPosition, int fallbackColor)
{
    for (const auto &piece : pieces)
    {
        if (!piece.captured && piece.type == PAWN && Vector2Equals(piece.position, promotionPosition))
        {
            return piece.color;
        }
    }

    return fallbackColor;
}

Board::~Board()
{
    std::unordered_set<unsigned int> pieceIds;
    for (const auto &piece : pieces)
    {
        pieceIds.insert(piece.texture.id);
    }
    UnloadPieces();
    UnloadPromotionTextures(promotionTexture, 12, pieceIds);
}

void Board::LoadPieces()
{

    Image piecesImage = LoadImage("resource/figure1.png");

    if (piecesImage.data == nullptr)
    { // For checking if the file is loaded correctly.
        std::cout << "Failed to load image: resource/figure1.png" << std::endl;
        return;
    }

    // Figure1.png contains two rows of pieces: one for black and one for white
    pieceWidth = piecesImage.width / pieceTypes;
    pieceHeight = piecesImage.height / pieceColors;

    Texture2D pieceTextures[pieceTypes * pieceColors];

    for (int y = 0; y < pieceColors; y++)
    {
        for (int x = 0; x < pieceTypes; x++)
        {

            Image pieceImage = ImageFromImage(piecesImage, Rectangle{static_cast<float>(x * pieceWidth),
                                                                     static_cast<float>(y * pieceHeight), static_cast<float>(pieceWidth), static_cast<float>(pieceHeight)});

            // For resizing the pieces cuz it's too biggggg....
            int newWidth = static_cast<int>(pieceWidth * 0.38);
            int newHeight = static_cast<int>(pieceHeight * 0.38);
            ImageResize(&pieceImage, newWidth, newHeight);

            pieceTextures[y * pieceTypes + x] = LoadTextureFromImage(pieceImage);

            UnloadImage(pieceImage);
        }
    }

    UnloadImage(piecesImage);

    // Initialization of pieces according to the board
    for (int y = 0; y < boardSize; y++)
    {
        for (int x = 0; x < boardSize; x++)
        {
            int pieceType = initialBoard[y][x];
            if (pieceType != 0)
            {

                int type = abs(pieceType);
                int color = (pieceType > 0) ? 0 : 1;
                Vector2 position = {((x * squareSize) + boardPosition.x), ((y * squareSize) + boardPosition.y)};
                Texture2D texture = pieceTextures[color * pieceTypes + (type - 1)];
                pieces.emplace_back(type, color, position, texture);
            }
        }
    }
}

void Board::LoadPromotionTexture()
{
    Image piecesImage = LoadImage("resource/figure1.png");

    if (piecesImage.data == nullptr)
    {
        // For checking if the file is loaded correctly.
        std::cout << "Failed to load image: resource/figure1.png" << std::endl;
        return;
    }

    // Figure1.png contains two rows of pieces: one for black and one for white
    pieceWidth = piecesImage.width / pieceTypes;
    pieceHeight = piecesImage.height / pieceColors;

    for (int y = 0; y < pieceColors; y++)
    {
        for (int x = 0; x < pieceTypes; x++)
        {

            Image pieceImage = ImageFromImage(piecesImage, Rectangle{static_cast<float>(x * pieceWidth),
                                                                     static_cast<float>(y * pieceHeight), static_cast<float>(pieceWidth), static_cast<float>(pieceHeight)});

            // For resizing the pieces cuz it's too biggggg....
            int newWidth = static_cast<int>(pieceWidth * 0.38);
            int newHeight = static_cast<int>(pieceHeight * 0.38);
            ImageResize(&pieceImage, newWidth, newHeight);

            promotionTexture[y * pieceTypes + x] = LoadTextureFromImage(pieceImage);

            UnloadImage(pieceImage);
        }
    }
    UnloadImage(piecesImage);
}

void Board::DrawPieces()
{
    DrawScores();
    for (const auto &piece : pieces)
    {
        if (piece.captured)
        {
            // Draw captured pieces at smaller scale in side panel
            float capturedScale = 0.6f; // 60% of original size
            DrawTextureEx(piece.texture, piece.position, 0.0f, capturedScale, WHITE);
        }
        else
        {
            // Transform position for flipped board (PVP_LOCAL only)
            Vector2 drawPos = TransformPosition(piece.position);
            DrawTexture(piece.texture, drawPos.x, drawPos.y, WHITE);
        }
    }
}

void Board::DrawLastMoveHightlight()
{

    // Only draw if a move has been made
    if (!gameState->getHasLastMove())
    {
        return;
    }

    // Highlight color Jade greeen
    Color hightlightColor = {46, 175, 60, 80}; // Jade greeen with 80% alpha

    // Get the last move positions
    Vector2 fromPos = gameState->getLastMoveFrom();
    Vector2 toPos = gameState->getLastMoveTo();

    // Transform Positions if board is flipped for 1v1 mode
    Vector2 drawFromPos = TransformPosition(fromPos);
    Vector2 drawToPos = TransformPosition(toPos);

    // Highlight rectangles on source and destination squares
    DrawBlurredRectangle(
        static_cast<int>(drawFromPos.x),
        static_cast<int>(drawFromPos.y + 3.4),
        static_cast<int>(squareSize),
        static_cast<int>(squareSize),
        hightlightColor);

    DrawBlurredRectangle(
        static_cast<int>(drawToPos.x),
        static_cast<int>(drawToPos.y + 3.4),
        static_cast<int>(squareSize),
        static_cast<int>(squareSize),
        hightlightColor);
}

void Board::DrawCheckHighlight()
{
    if (!kingInCheck)
        return;

    int currentColor = gameState->getCurrentPlayer();
    Vector2 kingPos = (currentColor == 1) ? whiteKingPosition : blackKingPosition;
    Vector2 drawPos = TransformPosition(kingPos); // Respects PVP board-flip

    DrawBlurredRectangle(
        drawPos.x,
        drawPos.y + 3.4f,
        squareSize, squareSize,
        {220, 20, 60, 140});
}

// For Piece Move Highlight
void Board::ToggleShowValidMoves()
{
    showValidMoves = !showValidMoves;
    if (!showValidMoves)
    {
        // Clear highlight-related selection state when toggled off.
        ClearSelection();
    }
}

void Board::ClearSelection()
{
    currentValidMoves.clear();
    hasPieceSelected = false;
    selectedPiecePosition = {-1, -1};
    selectedPieceType = PAWN; // Reset to default (doesn't matter, hasPieceSelected is false)
}

void Board::DrawValidMoveHighlights()
{

    // Only draw if feature is enabled and we have valid moves to show
    if (!showValidMoves || currentValidMoves.empty())
    {
        return;
    }

    // Define highlight colors
    Color moveColor = {111, 45, 189, 100}; // Semi-transparent {sortof} blue for moves
    Color captureColor = {255, 0, 0, 100}; // Semi-transparent red for captures
    Color Blackcolor = {0, 0, 0, 100};

    // Get current player's color to identify enemy pieces
    int currentPlayerColor = gameState->getCurrentPlayer();

    for (const Vector2 &move : currentValidMoves)
    {

        // Transform for flipped board
        Vector2 drawPos = TransformPosition(move);

        // Check if this move would be a capture (enemy piece at target position)
        bool isCapture = false;
        for (const auto &piece : pieces)
        {
            // Use tolerance-based comparison for floating-point positions
            // Only count as capture if it's an ENEMY piece (different color)
            if (!piece.captured &&
                piece.color != currentPlayerColor &&
                std::abs(piece.position.x - move.x) < 1.0f &&
                std::abs(piece.position.y - move.y) < 1.0f)
            {
                isCapture = true;
                break;
            }
        }

        // Check for en passant capture:
        // If selected piece is a PAWN and this move is DIAGONAL but no piece at destination,
        // it MUST be en passant (a pawn can only move diagonally when capturing)
        if (!isCapture && hasPieceSelected && selectedPieceType == PAWN)
        {
            // Use selectedPiecePosition (the ORIGINAL position before dragging started)
            // Use round() to avoid floating-point precision errors (e.g., 1.9999 -> 2, not 1)
            int pawnX = static_cast<int>(std::round((selectedPiecePosition.x - boardPosition.x) / squareSize));
            int pawnY = static_cast<int>(std::round((selectedPiecePosition.y - boardPosition.y) / squareSize));
            int moveX = static_cast<int>(std::round((move.x - boardPosition.x) / squareSize));
            int moveY = static_cast<int>(std::round((move.y - boardPosition.y) / squareSize));

            int dx = std::abs(moveX - pawnX);
            int dy = std::abs(moveY - pawnY);

            // Diagonal pawn move (dx==1, dy==1) with no piece at destination = en passant
            if (dx == 1 && dy == 1)
            {
                isCapture = true;
            }
        }

        if (isCapture)
        {

            // Draw capture indicator (filled square with border)
            DrawBlurredRectangle(
                static_cast<int>(drawPos.x),
                static_cast<int>(drawPos.y + 3.4),
                static_cast<int>(squareSize),
                static_cast<int>(squareSize),
                captureColor);
        }
        else
        {

            // Draw move indicator (circle in center of square)
            float centerX = drawPos.x + squareSize / 2;
            float centerY = drawPos.y + 3.4 + squareSize / 2;
            float radius = squareSize / 6; // Small circle

            DrawCircle(
                static_cast<int>(centerX),
                static_cast<int>(centerY),
                radius,
                Blackcolor);

            DrawBlurredRectangle(
                static_cast<int>(drawPos.x),
                static_cast<int>(drawPos.y + 3.4),
                static_cast<int>(squareSize),
                static_cast<int>(squareSize),
                moveColor);
        }
    }
}

void Board::DrawBlurredRectangle(float x, float y, float width, float height, Color baseColor, int blurLayers)
{

    // blurLayers: Number of outer Layers for the blur effect
    // Each Layer extends outward has less opacity

    float layerSize = 2.0f; // How many pixels each blur layer extends

    // Draw outer blur Layers first (back to front)
    for (int i = blurLayers; i > 0; i--)
    {

        // Calcualte layer dimensions (extends outwards from the main rectangle)
        float layerX = x - (0.8 * i * layerSize);
        float layerY = y - (0.6 * i * layerSize);
        float layerWidth = width + (i * layerSize);
        float layerHeight = height + (i * layerSize);

        // Calculate alpha: outer layer are more transparent
        // Uses exponential falloff for smoother blur
        float alphaFactor = 1.0f / (i + 1);
        unsigned char layerAlpha = static_cast<unsigned char>(baseColor.a * alphaFactor * 0.5f);

        Color layerColor = {baseColor.r, baseColor.g, baseColor.b, layerAlpha};

        // Draw only the border of each layer (not filled)
        // Glow effect
        DrawRectangleLinesEx(
            Rectangle{layerX, layerY, layerWidth, layerHeight},
            layerSize, // line thickness
            layerColor);
    }

    // Daw the main filled rectangle on top
    DrawRectangle(
        static_cast<int>(x),
        static_cast<int>(y),
        static_cast<int>(width),
        static_cast<int>(height),
        baseColor);
}

void Board::DrawPromotionMenu(Vector2 position, int color)
{
    color = ResolvePromotionColor(pieces, promotionPosition, color);

    // Drawing promotion menu background and border
    DrawRectangleLines(position.x, position.y, 4 * squareSize, squareSize, WHITE);

    // Drawing black promotion pieces
    for (int i = 0; i < 4; i++)
    {
        Vector2 piecePosition = {position.x + i * squareSize, position.y};
        if (color == 0)
        {
            DrawTexture(promotionTexture[i], piecePosition.x, piecePosition.y, WHITE);
        }
        else // Drawing white promotion pieces
        {
            DrawTexture(promotionTexture[i + 6], piecePosition.x, piecePosition.y, WHITE);
        }
    }
    HandlePawnPromotion(color, position);
}

void Board::DrawMoveHistory()
{
    if (!showMoveHistory)
        return;

    // Match the full right-side panel area.
    float panelX = 914.0f;
    float panelY = 55.0f;
    float panelWidth = 380.0f;
    float panelHeight = 910.0f;

    moveHistory.DrawPanel(panelX, panelY, panelWidth, panelHeight);
}

void Board::HandlePawnPromotion(int color, Vector2 position)
{
    color = ResolvePromotionColor(pieces, promotionPosition, color);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mousePos = GetMousePosition();

        for (int i = 0; i < 4; i++)
        {
            Rectangle pieceRect = {static_cast<float>(position.x) + i * squareSize, static_cast<float>(position.y), squareSize, squareSize};
            if (CheckCollisionPointRec(mousePos, pieceRect))
            {
                // Finding pawn that is being promoted
                for (auto &piece : pieces)
                {
                    if (Vector2Equals(piece.position, promotionPosition) && piece.type == PAWN && piece.color == color)
                    {
                        // Replacing pawn with the selected piece
                        piece.type = static_cast<PieceType>(i + 1); // i + 1 because 0: Rook, 1: Knight, 2: Bishop, 3: Queen
                        piece.texture = promotionTexture[(color == 0 ? i : i + 6)];
                        
                        // Append the promotion character to the UCI string for Stockfish
                        if (!uciMoveList.empty()) {
                            static constexpr char promoChars[] = {'r', 'n', 'b', 'q'};
                            uciMoveList.back() += promoChars[i];
                        }
                        
                        // Update move history with the promoted piece
                        if (!moveHistory.GetMoves().empty()) {
                            moveHistory.GetLastMoveMutable().promotedTo = static_cast<PieceType>(piece.type);
                        }

                        // Re-evaluate Check/Checkmate/Stalemate because the newly promoted piece might cause them.
                        // The turn was already switched in UpdateDragging, so the opponent is now the current player.
                        int opponentColor = gameState->getCurrentPlayer();
                        Vector2 opponentKingPos = (opponentColor == 1) ? whiteKingPosition : blackKingPosition;
                        bool opponentInCheck = MoveValidator::IsKingInCheck(pieces, opponentKingPos, opponentColor, *this);
                        
                        if (opponentInCheck)
                        {
                            if (MoveValidator::IsCheckmate(pieces, opponentColor, *this))
                            {
                                if (opponentColor == 0) // White made the promotion and checkmated Black
                                    Cwhite = true;
                                Checkmate = true;
                            }
                        }
                        
                        if (!Checkmate && MoveValidator::IsStatemate(pieces, opponentColor, *this))
                        {
                            Stalemate = true;
                        }
                        
                        kingInCheck = opponentInCheck && !Checkmate && !Stalemate;
                        if (!moveHistory.GetMoves().empty()) {
                            moveHistory.GetLastMoveMutable().isCheck = opponentInCheck && !Checkmate;
                        }
                        
                        break;
                    }
                }

                PawnPromo = false; // Close promotion menu
                break;
            }
        }
    }
}

void Board::CapturePiece(int capturedPieceIndex)
{
    // Side panel captured pieces layout
    float sidePanelX = 920; // Start X position in side panel
    float spacing = 70;     // Space between pieces
    int piecesPerRow = 5;   // Pieces per row in side panel

    int pieceValue = GetPieceValue(pieces[capturedPieceIndex].type);

    // Use gameState for score tracking
    int capturingColor = (pieces[capturedPieceIndex].color == 1) ? 0 : 1; // Opposite color captures
    gameState->addCapture(capturingColor, pieceValue);

    if (pieces[capturedPieceIndex].color == 1)
    { // White piece captured by black - show in top section of side panel
        int row = whiteCapturedCount / piecesPerRow;
        int col = whiteCapturedCount % piecesPerRow;

        pieces[capturedPieceIndex].position.x = sidePanelX + spacing * col;
        pieces[capturedPieceIndex].position.y = boardPosition.y + 20 + row * spacing; // Top area

        whiteCapturedCount++;
    }
    else
    { // Black piece captured by white - show in bottom section of side panel
        int row = blackCapturedCount / piecesPerRow;
        int col = blackCapturedCount % piecesPerRow;

        pieces[capturedPieceIndex].position.x = sidePanelX + spacing * col;
        // New rows go upward (subtract row * spacing)
        pieces[capturedPieceIndex].position.y = boardPosition.y + 7 * squareSize + 20 - row * spacing;

        blackCapturedCount++;
    }
    pieces[capturedPieceIndex].captured = true;
}

void Board::ExecuteCastling(Piece &king, bool kingside, std::vector<Piece> &pieces, const Vector2 originalPosition)
{

    int kingY = static_cast<int>((originalPosition.y - boardPosition.y) / squareSize);
    int rookX = kingside ? 7 : 0; // Kingside or Queenside rook
    int rookY = kingY;

    // Finding rook
    Piece *rook = nullptr;
    for (auto &piece : pieces)
    {
        if (piece.type == ROOK && piece.color == king.color &&
            static_cast<int>((piece.position.x - boardPosition.x) / squareSize) == rookX &&
            static_cast<int>((piece.position.y - boardPosition.y) / squareSize) == rookY)
        {
            rook = &piece;
            break;
        }
    }

    if (!rook)
    {
        return; // Rook not found
    }

    // Checking if king or rook has moved
    if (king.hasMoved || rook->hasMoved)
    {
        return; // Either the king or rook has moved
    }

    // Move the king
    Vector2 newKingPos = {boardPosition.x + (kingside ? 6 : 2) * squareSize, originalPosition.y};
    king.position.x = newKingPos.x;
    king.position.y = newKingPos.y;

    // Move the rook
    Vector2 newRookPos = {boardPosition.x + (kingside ? 5 : 3) * squareSize, rook->position.y};
    rook->position.x = newRookPos.x;
    rook->position.y = newRookPos.y;

    // Mark king and rook as moved
    king.hasMoved = true;
    rook->hasMoved = true;
}

void Board::ExecuteEnPassant(Piece &capturingPawn, std::vector<Piece> &pieces, const Vector2 &originalPosition, const Vector2 &newPosition)
{
    float capturedPawnX = newPosition.x;
    float capturedPawnY = originalPosition.y; // The captured pawn is directly behind the capturing pawn

    for (std::size_t i = 0; i < pieces.size(); ++i)
    {
        // Use tolerance-based comparison for floating-point positions
        if (std::abs(pieces[i].position.x - capturedPawnX) < 1.0f &&
            std::abs(pieces[i].position.y - capturedPawnY) < 1.0f &&
            pieces[i].type == PAWN && pieces[i].color != capturingPawn.color)
        {
            this->CapturePiece(i);
            break;
        }
    }

    capturingPawn.position = newPosition; // Move capturing pawn to the new position
}

void Board::UnloadPieces()
{
    std::unordered_set<unsigned int> unloadedIds;
    for (auto &piece : pieces)
    {
        if (piece.texture.id != 0 && unloadedIds.insert(piece.texture.id).second)
        {
            UnloadTexture(piece.texture);
        }
        piece.texture = {0};
    }
}

// For Drag and Drop (Basically moving)
void Board::UpdateDragging()
{
    mousePos = GetMousePosition();

    // Transform mouse for flipped board (PVP_LOCAL only)
    mousePos = TransformMouse(mousePos);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        for (std::size_t i = 0; i < pieces.size(); i++)
        {

            // For Skiping Captured Pieces
            if (pieces[i].captured)
            {
                continue;
            }
            /*---------------------------------------------*/

            Rectangle pieceRect = {pieces[i].position.x,
                                   pieces[i].position.y, squareSize, squareSize};

            if (CheckCollisionPointRec(mousePos, pieceRect))
            {
                // Only allow picking up pieces that belong to the current player.
                // This prevents the player from visually dragging opponent pieces
                if (pieces[i].color != gameState->getCurrentPlayer())
                {
                    ClearSelection();
                    break;
                }

                dragging = true;
                draggedPieceIndex = i;
                offset = Vector2Subtract(mousePos, pieces[i].position);
                originalPosition = pieces[i].position;

                // Generate valid moves for highlighting
                if (showValidMoves && pieces[i].color == gameState->getCurrentPlayer())
                {

                    // Skip regeneration if same piece is already selected
                    if (!hasPieceSelected || !Vector2Equals(selectedPiecePosition, pieces[i].position))
                    {

                        // New piece selected - generate moves and store piece info
                        selectedPiecePosition = pieces[i].position;
                        selectedPieceType = pieces[i].type; // Store piece type for highlight detection
                        hasPieceSelected = true;
                        currentValidMoves = MoveGeneration::GetAllPossibleMoves(pieces[i], pieces, *this);
                    }
                }
                else
                {
                    ClearSelection();
                }

                // Moving the dragged piece to the back of the vector so it is drawn last
                Piece draggedPiece = pieces[i];
                pieces.erase(pieces.begin() + i);
                pieces.push_back(draggedPiece);
                draggedPieceIndex = pieces.size() - 1; // Update the index after moving the piece
                break;
            }
        }
    }

    if (dragging)
    {
        pieces[draggedPieceIndex].position = Vector2Subtract(mousePos, offset);
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            dragging = false;

            float snappedX = roundf((pieces[draggedPieceIndex].position.x - boardPosition.x) / squareSize) * squareSize + boardPosition.x;
            float snappedY = roundf((pieces[draggedPieceIndex].position.y - boardPosition.y) / squareSize) * squareSize + boardPosition.y;

            bool outOfBounds = (snappedX < boardPosition.x || snappedX >= (boardPosition.x + 8 * squareSize) ||
                                snappedY < boardPosition.y || snappedY >= (boardPosition.y + 8 * squareSize));

            if (outOfBounds)
            {
                pieces[draggedPieceIndex].position = originalPosition;
            }
            else
            {
                // Constrain snappedX to the board boundaries(This also works)
                // if (snappedX < boardPosition.x) snappedX = boardPosition.x;
                // if (snappedX >= boardPosition.x + 8 * squareSize) snappedX = boardPosition.x + (8 - 1) * squareSize;
                // This is shorter form
                snappedX = std::max(boardPosition.x, std::min(snappedX, boardPosition.x + 7 * squareSize));
                snappedY = std::max(boardPosition.y, std::min(snappedY, boardPosition.y + 7 * squareSize));

                Vector2 newPosition = Vector2{snappedX, snappedY};

                // Checking the turn of the player(UI improvement)
                if (pieces[draggedPieceIndex].color == gameState->getCurrentPlayer())
                {
                    if (originalPosition.x != newPosition.x || originalPosition.y != newPosition.y)
                    {

                        // Original king position
                        Vector2 tempKingPosition;
                        if (pieces[draggedPieceIndex].type == KING)
                        {
                            tempKingPosition = (pieces[draggedPieceIndex].color == 0) ? blackKingPosition : whiteKingPosition;
                        }

                        // OPTIMIZED MOVE VALIDATION FLOW:
                        // 1. If we have cached valid moves, use quick lookup first
                        // 2. Only proceed to IsMoveValid for special move execution (castling, en passant)
                        // 3. If move not in cache, reject immediately without expensive validation

                        bool moveIsValid = false;

                        // First, quick check if the move is in our pre-calculated valid moves
                        if (!currentValidMoves.empty())
                        {
                            if (MoveValidator::IsMoveInValidMoves(newPosition, currentValidMoves))
                            {
                                // The move was pre-validated during piece pickup
                                // Still need IsMoveValid for special move execution (castling, en passant, lastMove update)
                                moveIsValid = MoveValidator::IsMoveValid(pieces[draggedPieceIndex], newPosition, pieces, originalPosition, *this);
                            }
                            else
                            {
                                // Move NOT in cached valid moves - reject immediately
                                // This saves expensive re-validation for invalid moves
                                moveIsValid = false;
                            }
                        }
                        else
                        {
                            // No cached moves (feature disabled or first move), fall back to full validation
                            moveIsValid = MoveValidator::IsMoveValid(pieces[draggedPieceIndex], newPosition, pieces, originalPosition, *this);
                        }

                        // Checking if the move is valid
                        if (moveIsValid)
                        {
                            const std::vector<Piece> piecesBeforeMove = pieces;
                            pieces[draggedPieceIndex].position = newPosition;

                            // Mark rook as moved AFTER successful move
                            if (pieces[draggedPieceIndex].type == ROOK)
                            {
                                pieces[draggedPieceIndex].hasMoved = true;
                            }

                            if (pieces[draggedPieceIndex].type == KING)
                            {
                                if (pieces[draggedPieceIndex].color == 0)
                                { // Black King
                                    blackKingPosition = newPosition;
                                }
                                else if (pieces[draggedPieceIndex].color == 1)
                                { // White King
                                    whiteKingPosition = newPosition;
                                }
                            }

                            // For capturing - simplified since move is already validated
                            bool wasCapture = false; // for notation use
                            bool wasEnPassant = false;
                            bool validCapture = true;
                            for (std::size_t i = 0; i < pieces.size(); ++i)
                            {
                                if (pieces[i].position.x == newPosition.x && pieces[i].position.y == newPosition.y && pieces[i].color != pieces[draggedPieceIndex].color)
                                {
                                    // Move was pre-validated, capture is safe
                                    wasCapture = true; // for notation use
                                    this->CapturePiece(i);
                                    break;
                                }
                            }

                            if (!wasCapture && pieces[draggedPieceIndex].type == PAWN)
                            {
                                const int fromX = static_cast<int>(std::round((originalPosition.x - boardPosition.x) / squareSize));
                                const int toX = static_cast<int>(std::round((newPosition.x - boardPosition.x) / squareSize));
                                if (std::abs(toX - fromX) == 1)
                                {
                                    wasEnPassant = true;
                                    wasCapture = true;
                                }
                            }

                            // Checkmate detection after a valid move
                            if (validCapture)
                            {
                                pieces[draggedPieceIndex].position = newPosition;

                                // After the current player moves, game-over checks must target the opponent.
                                int opponentColor = 1 - gameState->getCurrentPlayer();

                                bool opponentInCheck = MoveValidator::IsKingInCheck(pieces,
                                                                                    opponentColor == 1 ? whiteKingPosition : blackKingPosition,
                                                                                    opponentColor,
                                                                                    *this);

                                if (opponentInCheck)
                                {
                                    // 0 for black and 1 for white
                                    if (!PawnPromo && MoveValidator::IsCheckmate(pieces,
                                                                   opponentColor,
                                                                   *this))
                                    {

                                        if (gameState->getCurrentPlayer() == 1)
                                        {
                                            Cwhite = true;
                                        }
                                        Checkmate = true;
                                        return;
                                    }
                                }

                                if (!Checkmate && !PawnPromo)
                                {
                                    if (MoveValidator::IsStatemate(pieces, opponentColor, *this))
                                    {
                                        std::cout << "Stalemate! It's a draw" << std::endl;
                                        Stalemate = true;
                                        return;
                                    }
                                }

                                // This records moves after it happened and before the player is changed
                                {
                                    MoveRecord record;
                                    record.moveNumber = static_cast<int>(moveHistory.GetMoves().size()) / 2 + 1;
                                    record.pieceType = static_cast<PieceType>(pieces[draggedPieceIndex].type);
                                    record.pieceColor = pieces[draggedPieceIndex].color;
                                    record.from = originalPosition;
                                    record.to = newPosition;

                                    // Added later SAN disambiguation for same-type pieces that can also reach the target square.
                                    if (record.pieceType != PAWN)
                                    {
                                        bool needsDisambiguation = false;
                                        bool sameFileConflict = false;
                                        bool sameRankConflict = false;

                                        for (std::size_t i = 0; i < piecesBeforeMove.size(); ++i)
                                        {
                                            if (static_cast<int>(i) == draggedPieceIndex)
                                            {
                                                continue;
                                            }

                                            const Piece &candidate = piecesBeforeMove[i];
                                            if (candidate.captured || candidate.color != record.pieceColor || candidate.type != pieces[draggedPieceIndex].type)
                                            {
                                                continue;
                                            }

                                            if (MoveValidator::IsMoveLegal(candidate, newPosition, piecesBeforeMove, candidate.position, *this))
                                            {
                                                needsDisambiguation = true;

                                                const int candidateFile = static_cast<int>(std::round((candidate.position.x - boardPosition.x) / squareSize));
                                                const int candidateRank = 7 - static_cast<int>(std::round((candidate.position.y - boardPosition.y) / squareSize));
                                                const int sourceFile = static_cast<int>(std::round((originalPosition.x - boardPosition.x) / squareSize));
                                                const int sourceRank = 7 - static_cast<int>(std::round((originalPosition.y - boardPosition.y) / squareSize));

                                                if (candidateFile == sourceFile)
                                                {
                                                    sameFileConflict = true;
                                                }
                                                if (candidateRank == sourceRank)
                                                {
                                                    sameRankConflict = true;
                                                }
                                            }
                                        }

                                        if (needsDisambiguation)
                                        {
                                            const int sourceFile = static_cast<int>(std::round((originalPosition.x - boardPosition.x) / squareSize));
                                            const int sourceRank = 7 - static_cast<int>(std::round((originalPosition.y - boardPosition.y) / squareSize));

                                            if (!sameFileConflict)
                                            {
                                                record.disambiguation = std::string(1, static_cast<char>('a' + sourceFile));
                                            }
                                            else if (!sameRankConflict)
                                            {
                                                record.disambiguation = std::string(1, static_cast<char>('1' + sourceRank));
                                            }
                                            else
                                            {
                                                record.disambiguation = std::string(1, static_cast<char>('a' + sourceFile)) +
                                                                         std::string(1, static_cast<char>('1' + sourceRank));
                                            }
                                        }
                                    }

                                    record.isCapture = wasCapture;
                                    record.isEnPassant = wasEnPassant;
                                    record.isCheck = opponentInCheck && !Checkmate;

                                    // Castle detection
                                    if (pieces[draggedPieceIndex].type == KING)
                                    {
                                        float dx = std::abs(newPosition.x - originalPosition.x);
                                        if (dx > squareSize * 1.5f)
                                        {
                                            record.isCastleKing = (newPosition.x > originalPosition.x);  // e1 -> g1 or e8 -> g8
                                            record.isCastleQueen = (newPosition.x < originalPosition.x); // e1 -> c1 or e8 _> c8
                                        }
                                    }

                                    moveHistory.AddMove(record);
                                }

                                // For recording Uci moves in updatedragging
                                uciMoveList.push_back(posToUCI(originalPosition) + posToUCI(newPosition));

                                // Damn I worked Hard in this function
                                // Store the Last move for highlighting
                                gameState->setLastMove(originalPosition, newPosition);

                                // Switch player and flip board (handled by GameState)
                                gameState->switchPlayer();

                                // Cache whether the new current player's king is in check after switch.
                                kingInCheck = opponentInCheck && !Checkmate && !Stalemate;

                                ClearSelection(); // Clear the move highlight after move is made
                            }
                        }
                        else
                        {
                            pieces[draggedPieceIndex].position = originalPosition;

                            ClearSelection(); // Clear the move highlight even if move is not made

                            // Reverting king's position if the move was invalid
                            if (pieces[draggedPieceIndex].type == KING)
                            {
                                if (pieces[draggedPieceIndex].color == 0)
                                {
                                    blackKingPosition = tempKingPosition;
                                }
                                else if (pieces[draggedPieceIndex].color == 1)
                                {
                                    whiteKingPosition = tempKingPosition;
                                }
                            }
                        }
                    }
                    else
                    {
                        pieces[draggedPieceIndex].position = originalPosition;
                    }
                }
                else
                {
                    pieces[draggedPieceIndex].position = originalPosition;
                }
            }
        }
    }
}

 
bool Board::ApplyEngineMove(const EngineMove& move) 
{

    if(!move.isValid) return false; 

    // Finding the piece at move.from 
    int pieceIndex = -1; 
    for ( int i = 0; i < static_cast<int>(pieces.size()); i++) 
    {

        if(!pieces[i].captured
            && std::abs(pieces[i].position.x - move.from.x) < 1.0f
            && std::abs(pieces[i].position.y - move.from.y) < 1.0f)
        {
            pieceIndex = i;
            break;
        }
    }

    if (pieceIndex == -1) return false; // NO piece found at that square 

    Vector2 originalPos = pieces[pieceIndex].position; 
    Vector2 newPos = move.to; 

    // Validate via existing pipeline (also executes castling / enpassant) 
    if(!MoveValidator::IsMoveValid(pieces[pieceIndex], newPos, pieces, originalPos, *this))
        return false;

    // For engine moves the UI must never appear – the engine
    // handles its own promotion below, so we clear the flag unconditionally here.
    PawnPromo = false;

    pieces[pieceIndex].position = newPos;

    if (pieces[pieceIndex].type == PAWN)
    {
        int promotionRank = (pieces[pieceIndex].color == 0) ? 7 : 0;
        
        int newY = static_cast<int>(roundf((newPos.y - boardPosition.y) / squareSize));
        if (newY == promotionRank)
        {
            int promotedType = move.promotionPiece;
            if (promotedType < ROOK || promotedType > QUEEN)
            {
                promotedType = QUEEN;
            }

            pieces[pieceIndex].type = promotedType;
            pieces[pieceIndex].texture = promotionTexture[(pieces[pieceIndex].color == 0)
                                                             ? (promotedType - 1)
                                                             : (promotedType - 1 + 6)];
            PawnPromo = false;
        }
    }

    // Track king positions
    if(pieces[pieceIndex].type == KING)
    {
        if(pieces[pieceIndex].color == 0)
            blackKingPosition = newPos;
        else
            whiteKingPosition = newPos;
    }

    if(pieces[pieceIndex].type == ROOK)
        pieces[pieceIndex].hasMoved = true;

    // Handle captures
    bool wasCapture = false;

    for (int i = 0; i < static_cast<int>(pieces.size()); i++)
    {
        if(!pieces[i].captured
            && std::abs(pieces[i].position.x - newPos.x) < 1.0f
            && std::abs(pieces[i].position.y - newPos.y) < 1.0f
            && pieces[i].color != pieces[pieceIndex].color)
        {
            wasCapture = true;
            CapturePiece(i);
            break;
        }
    }

    uciMoveList.push_back(posToUCI(originalPos) + posToUCI(newPos));

    // If the engine promoted a pawn, append the piece character so Stockfish
    // receives a valid UCI history (e.g. "a2a1q" not just "a2a1").
    // ROOK=1:'r', KNIGHT=2:'n', BISHOP=3:'b', QUEEN=4:'q'
    if (move.promotionPiece >= ROOK && move.promotionPiece <= QUEEN)
    {
        static constexpr char promoChars[] = {'\0', 'r', 'n', 'b', 'q'};
        uciMoveList.back() += promoChars[move.promotionPiece];
    }

    // Check / checkmate / stalemate detection
    int opponentColor = 1 - gameState->getCurrentPlayer();
    Vector2 opponentKingPos = (opponentColor == 1 ) ? whiteKingPosition : blackKingPosition; 
    bool opponentInCheck = MoveValidator::IsKingInCheck(pieces, opponentKingPos, opponentColor, *this); 
    
    if (opponentInCheck) 
    {
        if(MoveValidator::IsCheckmate(pieces, opponentColor, *this))
        {
            if(gameState->getCurrentPlayer() == 1)
                Cwhite = true; 
            Checkmate = true; 
        }
    }

    if (!Checkmate && MoveValidator::IsStatemate(pieces, opponentColor, *this))
    {
        Stalemate = true; 
    }

    // Build and store MoveRecord 
    MoveRecord record; 

    record.moveNumber = static_cast<int>(moveHistory.GetMoves().size()) / 2 + 1; 
    record.pieceType = static_cast<PieceType>(pieces[pieceIndex].type);
    record.pieceColor = pieces[pieceIndex].color; 
    record.from = originalPos; 
    record.to = newPos; 
    record.isCapture = wasCapture; 
    record.isCheck = opponentInCheck && !Checkmate; 
    
    if(pieces[pieceIndex].type == KING)
    {
        float dx = std::abs(newPos.x - originalPos.x); 
        if(dx > squareSize * 1.5f) 
        {
            record.isCastleKing = (newPos.x > originalPos.x);
            record.isCastleQueen = (newPos.x < originalPos.x); 
        }
    }
    moveHistory.AddMove(record); 

    gameState->setLastMove(originalPos, newPos); 
    gameState->switchPlayer();
    kingInCheck = opponentInCheck && !Checkmate && !Stalemate; 

    return true; 
}


void Board::Reset()
{
    // First unload old pieces texture/ it is inefficient but safe
    UnloadPieces();
    UnloadPromotionTextures(promotionTexture, 12, {});

    pieces.clear(); // Clear the pieces vector

    Checkmate = false;
    Stalemate = false;
    PawnPromo = false;
    Cwhite = false;
    kingInCheck = false;
    dragging = false;
    draggedPieceIndex = -1;
    Resigned = false;
    resignedPlayer = -1;
    showMoveHistory = true;
    promotionPosition = {0, 0};

    // REset king position to starting position
    whiteKingPosition = {boardPosition.x + 4 * squareSize, boardPosition.y + 7 * squareSize};
    blackKingPosition = {boardPosition.x + 4 * squareSize, boardPosition.y};

    // Reset GameState (scores, current player, board flip)
    gameState->reset();

    // Reset shared lastMove state so en passant does not carry across games.
    MoveSimulation::lastMove = std::make_tuple(Piece{}, Vector2{0.0f, 0.0f}, Vector2{0.0f, 0.0f});

    // Reset captured pieces display counters
    whiteCapturedCount = 0;
    blackCapturedCount = 0;
    p1 = 1;

    moveHistory.Clear();

    uciMoveList.clear(); 

    // Clear move highlights
    ClearSelection();

    LoadPieces();
    LoadPromotionTexture();
}