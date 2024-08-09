
#include <iostream>
#include "Board.hpp"
#include <raymath.h>
#include <algorithm>
#include "MoveValid.hpp"
//for global
float squareSize = 112.6;
Vector2 boardPosition = {0, 55};

Board::Board() : dragging(false),draggedPieceIndex(-1), CurrentPlayer(1) {}

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

                int type = abs(pieceType);
                int color = (pieceType > 0) ? 0 : 1;
                Vector2 position = {((x * squareSize) + boardPosition.x), ((y * squareSize) + boardPosition.y)};
                Texture2D texture = pieceTextures[color * pieceTypes + (type-1)];
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

            // For Skiping Captured Pieces 
            if(pieces[i].captured)
            {
                continue;
            }
            /*---------------------------------------------*/

            Rectangle pieceRect = {pieces[i].position.x,
                 pieces[i].position.y , squareSize, squareSize}; 

            if (CheckCollisionPointRec(mousePos,pieceRect)&&(pieces[i].color == CurrentPlayer) ){
                dragging = true;
                draggedPieceIndex = i;
                offset = Vector2Subtract(mousePos, pieces[i].position);
                originalPosition = pieces[i].position;

                // Move the dragged piece to the back of the vector so it is drawn last
                Piece draggedPiece = pieces[i];
                pieces.erase(pieces.begin() + i);
                pieces.push_back(draggedPiece);
                draggedPieceIndex = pieces.size() - 1; // Update the index after moving the piece
                break;
            }
        }
    }

    if (dragging) {
        pieces[draggedPieceIndex].position = Vector2Subtract(mousePos, offset);
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            dragging = false;
            
            float snappedX = roundf((pieces[draggedPieceIndex].position.x - boardPosition.x) / squareSize) * squareSize + boardPosition.x;
            float snappedY = roundf((pieces[draggedPieceIndex].position.y - boardPosition.y) / squareSize) * squareSize + boardPosition.y;


            bool outOfBounds = (snappedX < boardPosition.x || snappedX >= (boardPosition.x + 8* squareSize) ||
                                 snappedY < boardPosition.y || snappedY >= (boardPosition.y + 8 * squareSize));

            if(outOfBounds){
                pieces[draggedPieceIndex].position = originalPosition;
            }                     
            else{
                // Constrain snappedX to the board boundaries(This also works)
                // if (snappedX < boardPosition.x) snappedX = boardPosition.x;
                // if (snappedX >= boardPosition.x + 8 * squareSize) snappedX = boardPosition.x + (8 - 1) * squareSize;
                //This is shorter form 
                snappedX = std::max(boardPosition.x, std::min(snappedX, boardPosition.x + 7 * squareSize));

                // Constrain snappedY to the board boundaries  
                // if (snappedY < boardPosition.y) snappedY = boardPosition.y;
                // if (snappedY >= boardPosition.y + 8 * squareSize) snappedY = boardPosition.y + (8 - 1) * squareSize;
                //This is shorter form 
                snappedY = std::max(boardPosition.y, std::min(snappedY, boardPosition.y + 7 * squareSize));

                Vector2 newPosition = Vector2 {snappedX,snappedY};

                if(originalPosition.x != newPosition.x || originalPosition.y != newPosition.y){
                    //Checking if move is valid.
                    if(MoveValidator::IsMoveValid(pieces[draggedPieceIndex], newPosition, pieces, originalPosition)){
                    
                        //For capturing 
                        for (std::size_t i = 0; i < pieces.size(); ++i) {
                        if (pieces[i].position.x == newPosition.x && pieces[i].position.y == newPosition.y && pieces[i].color != pieces[draggedPieceIndex].color) {
                            CapturePiece(i);  // Capture the piece
                            break;
                        }
                    }

                    pieces[draggedPieceIndex].position = newPosition;
                    CurrentPlayer = (CurrentPlayer +1) % 2;
                  }
               else{ 
                    pieces[draggedPieceIndex].position = originalPosition;
                }
          } 
            else{ 
            pieces[draggedPieceIndex].position = originalPosition;
            } 
      }
    }
  }
}

void Board::CapturePiece(int capturedPieceIndex)
{
        float offset = 20.0f; 
    //It will move piece outside

            if (pieces[capturedPieceIndex].color == 1) { // Captured by black
            pieces[capturedPieceIndex].position.x = boardPosition.x + 7 * squareSize + squareSize + offset * whiteCapturedCount;
            pieces[capturedPieceIndex].position.y = boardPosition.y + offset;

            // pieces[capturedPieceIndex].position.x = boardPosition.x + 8.7 * squareSize + offset;
            // pieces[capturedPieceIndex].position.y = boardPosition.y + squareSize + squareSize + offset*whiteCapturedCount; 

            whiteCapturedCount++;
            } 
            else { // Captured by white
            pieces[capturedPieceIndex].position.x = boardPosition.x + 7 * squareSize + squareSize + offset * blackCapturedCount;
            pieces[capturedPieceIndex].position.y = boardPosition.y + 7 * squareSize + offset;
            // pieces[capturedPieceIndex].position.x = boardPosition.x + 8 * squareSize + offset;
            // pieces[capturedPieceIndex].position.y = boardPosition.y + squareSize + squareSize + offset*blackCapturedCount;

            blackCapturedCount++;
            }
            pieces[capturedPieceIndex].captured = true;

}


            