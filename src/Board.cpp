#include <raylib.h>
#include <vector>
#include <iostream>
#include "Board.hpp"

std::vector<Piece> pieces;    

float squareSize=115;

void LoadPieces() {

    Image piecesImage = LoadImage("resource\\figure1.png");
    Image Boardszz = LoadImage("resource\\board0.png");

    if (piecesImage.data == nullptr) {//for checking if the file is loaded correctly.
    std::cout << "Failed to load image: resource\\figure1.png" << std::endl;
    return;
}
    int boardWidth = Boardszz.width;
    int boardHeight = Boardszz.height;

    // In figure1.png contains two rows of pieces: one for black and one for white
    int pieceWidth = piecesImage.width / pieceTypes;
    int pieceHeight = piecesImage.height / pieceColors;
    
    Texture2D pieceTextures[pieceTypes * pieceColors];

    for (int y = 0; y < pieceColors; y++) {
        for (int x = 0; x < pieceTypes; x++) {

            Image pieceImage = ImageFromImage(piecesImage, Rectangle{static_cast<float>(x*pieceWidth),
             static_cast<float>(y*pieceHeight), static_cast<float>(pieceWidth), static_cast<float>(pieceHeight)});


            //for resizing the pieces cuz it's too biggggg....
                int newWidth = static_cast<int>(pieceWidth * 0.4);
                int newHeight = static_cast<int>(pieceHeight* 0.4);
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
                Piece piece;
                piece.type = abs(pieceType) - 1;
                piece.color = (pieceType > 0) ? 0 : 1;
                piece.position = Vector2{((float)x*boardWidth)/8, ((float)y*boardHeight)/8};
                piece.texture = pieceTextures[piece.color * pieceTypes + piece.type];
                pieces.push_back(piece);
            }
        }
    }
}

void DrawPieces() {
    for (const auto& piece : pieces) {
        DrawTexture(piece.texture, piece.position.x, piece.position.y, WHITE);
    }
}
void UnloadPieces(){
    for (auto& piece : pieces) {
        UnloadTexture(piece.texture);
    }
}

