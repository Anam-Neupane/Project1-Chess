
#include <iostream>
#include "Board.hpp"
#include <raymath.h>

   
float squareSize=112.6;

Board::Board() : dragging(false),draggedPieceIndex(-1) , boardPosition{0,55} {}

Board::~Board(){
    UnloadPieces();
}

void Board::LoadPieces() {

    Image piecesImage = LoadImage("resource/figure1.png");

    if (piecesImage.data == nullptr) {//for checking if the file is loaded correctly.
    std::cout << "Failed to load image: resource/figure1.png" << std::endl;
    return;
}


    // In figure1.png contains two rows of pieces: one for black and one for white
     pieceWidth = piecesImage.width / pieceTypes;
     pieceHeight = piecesImage.height / pieceColors;
    
    Texture2D pieceTextures[pieceTypes * pieceColors];

    for (int y = 0; y < pieceColors; y++) {
        for (int x = 0; x < pieceTypes; x++) {

            Image pieceImage = ImageFromImage(piecesImage, Rectangle{static_cast<float>(x*pieceWidth),
             static_cast<float>(y*pieceHeight), static_cast<float>(pieceWidth), static_cast<float>(pieceHeight)});


            //for resizing the pieces cuz it's too biggggg....
                int newWidth = static_cast<int>(pieceWidth * 0.38);
                int newHeight = static_cast<int>(pieceHeight* 0.38);
                ImageResize(&pieceImage, newWidth, newHeight);


            pieceTextures[y * pieceTypes + x] = LoadTextureFromImage(pieceImage);
            
            UnloadImage(pieceImage);
        }
    }

    UnloadImage(piecesImage);

    // Initialize pieces according to the initial board
    for (int y = 0; y < boardSize; y++) {
        for (int x = 0; x < boardSize; x++) {
            int pieceType = initialBoard[y][x];
            if (pieceType != 0) {

                int type = abs(pieceType) - 1;
                int color = (pieceType > 0) ? 0 : 1;
                Vector2 position = {((x * squareSize) + boardPosition.x), ((y * squareSize) + boardPosition.y)};
                Texture2D texture = pieceTextures[color * pieceTypes + type];
                pieces.emplace_back(type, color, position,texture);
            }
        }
    }
}

void Board::DrawPieces() {
    for (const auto& piece : pieces) {
        DrawTexture(piece.texture, piece.position.x, piece.position.y, WHITE);
    }
}
void Board::UnloadPieces(){
    for (auto& piece : pieces) {
        UnloadTexture(piece.texture);
    }
}
//For Drag and Drop (Basically moving)
void Board::UpdateDragging() {
    mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (std::size_t i = 0; i < pieces.size(); i++) {
            if (CheckCollisionPointRec(mousePos, Rectangle{pieces[i].position.x +boardPosition.x,
                 pieces[i].position.y +boardPosition.y, squareSize, squareSize})) {
                dragging = true;
                draggedPieceIndex = i;
                offset = Vector2Subtract(mousePos, pieces[i].position);
                break;
            }
        }
    }

    if (dragging) {
        pieces[draggedPieceIndex].position = Vector2Subtract(mousePos, offset);
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            dragging = false;
            // Snap to grid
            pieces[draggedPieceIndex].position.x = (int)((pieces[draggedPieceIndex].position.x - boardPosition.x ) / squareSize) * squareSize + boardPosition.x;
            pieces[draggedPieceIndex].position.y = (int)((pieces[draggedPieceIndex].position.y - boardPosition.y ) / squareSize) * squareSize + boardPosition.y;

            if (pieces[draggedPieceIndex].position.x < boardPosition.x) 
                pieces[draggedPieceIndex].position.x = boardPosition.x;

            if (pieces[draggedPieceIndex].position.y < boardPosition.y) 
                pieces[draggedPieceIndex].position.y = boardPosition.y;

            if (pieces[draggedPieceIndex].position.x >= boardPosition.x + 8 * squareSize) 
                pieces[draggedPieceIndex].position.x = boardPosition.x + (8 - 1) * squareSize;

            if (pieces[draggedPieceIndex].position.y >= boardPosition.y + 8 * squareSize) 
                pieces[draggedPieceIndex].position.y = boardPosition.y + (8 - 1) * squareSize;
        }
    }
}
