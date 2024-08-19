#include <iostream>
#include "Board.hpp"
#include <raymath.h>
#include <algorithm>
#include "MoveValid.hpp"

//For Global variable
float squareSize = 112.6;
Vector2 boardPosition = {0, 55};

int Board::GetPieceValue(int pieceType)
{
    switch(pieceType)
    {
        case PAWN: return 1;
        case KNIGHT: return 3;
        case BISHOP: return 3;
        case ROOK: return 5;
        case QUEEN: return 9;
        default: return 0;
    }
}
//Constructor
Board::Board() : dragging(false), draggedPieceIndex(-1), CurrentPlayer(1),
    whiteScorePosition({boardPosition.x+5 , boardPosition.y + squareSize * 8 + 10}),//Initialization of the Score Position
    blackScorePosition({boardPosition.x+5 , boardPosition.y - 25}), 
    //Initialization of Player's Turn Position
    playerturnPosition({boardPosition.x + squareSize * 8 + 19, boardPosition.y + squareSize * 4 - 29})
    {}

void Board::DrawScores() {//Drawing scores
    std::string whiteScoreText = "White : " + std::to_string(whiteScore);
    std::string blackScoreText = "Black : " + std::to_string(blackScore);

    DrawText(whiteScoreText.c_str(), whiteScorePosition.x, whiteScorePosition.y, 30, WHITE);
    DrawText(blackScoreText.c_str(), blackScorePosition.x, blackScorePosition.y, 30, WHITE);
}

void Board::DrawPlayer(){
    if(CurrentPlayer)
    {
        DrawRectangle(playerturnPosition.x-5, playerturnPosition.y-6, 180, 80, BLACK);
        DrawRectangle(playerturnPosition.x-5, playerturnPosition.y, 180, 68, WHITE);
        DrawText("WHITE's \n \n  Turn", playerturnPosition.x, playerturnPosition.y, 40, BLACK);
    }
    else
    {   
        DrawRectangle(playerturnPosition.x-5, playerturnPosition.y-6, 180, 80, BLACK);
        DrawText("BLACK's \n \n  Turn", playerturnPosition.x, playerturnPosition.y, 40, WHITE);
    }
}

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

void Board::LoadPromotionTexture()
{
    Image piecesImage = LoadImage("resource/figure1.png");

    if (piecesImage.data == nullptr) {//for checking if the file is loaded correctly.
    std::cout << "Failed to load image: resource/figure1.png" << std::endl;
    return;
}


    // In figure1.png contains two rows of pieces: one for black and one for white
     pieceWidth = piecesImage.width / pieceTypes;
     pieceHeight = piecesImage.height / pieceColors;

    for (int y = 0; y < pieceColors; y++) {
        for (int x = 0; x < pieceTypes; x++) {

            Image pieceImage = ImageFromImage(piecesImage, Rectangle{static_cast<float>(x*pieceWidth),
             static_cast<float>(y*pieceHeight), static_cast<float>(pieceWidth), static_cast<float>(pieceHeight)});


            //for resizing the pieces cuz it's too biggggg....
                int newWidth = static_cast<int>(pieceWidth * 0.38);
                int newHeight = static_cast<int>(pieceHeight* 0.38);
                ImageResize(&pieceImage, newWidth, newHeight);


            promotionTexture[y * pieceTypes + x] = LoadTextureFromImage(pieceImage);
            
            UnloadImage(pieceImage);
        }
    }
    UnloadImage(piecesImage);

}

void Board::DrawPieces() {
    DrawScores();
    for (const auto& piece : pieces) {
        DrawTexture(piece.texture, piece.position.x, piece.position.y, WHITE);
    }
}

void Board::DrawPromotionMenu(Vector2 position, int color)
{
    // Draw the promotion menu background and border
    
    DrawRectangle(position.x, position.y, 4 * squareSize, squareSize, WHITE);
    if(color == 1)DrawRectangle(position.x, position.y, 4 * squareSize, squareSize, {0, 0, 0, 200});
    DrawRectangleLines(position.x, position.y, 4 * squareSize, squareSize, WHITE);
    
    
        // Draw the black promotion pieces
        for (int i = 0; i < 4; i++) {
            Vector2 piecePosition = {position.x + i * squareSize, position.y};
            if(color == 0)
            {  
                DrawTexture(promotionTexture[i], piecePosition.x, piecePosition.y, WHITE);
            }
            else
            {
                DrawTexture(promotionTexture[i+6], piecePosition.x, piecePosition.y, WHITE);
            }
        }
        HandlePawnPromotion(color, position);
}

void Board::HandlePawnPromotion(int color, Vector2 position) {
    int windowWidth = GetScreenWidth();  // Get the current window width
    int windowHeight = GetScreenHeight();  // Get the current window height

    int PosX = ((windowWidth - 4 * squareSize) / 2);
    int PosY = ((windowHeight - 40) / 2 - 30);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();

        for (int i = 0; i < 4; i++) {
            Rectangle pieceRect = {static_cast<float>(PosX) + i * squareSize, static_cast<float>(PosY), squareSize, squareSize};
            if (CheckCollisionPointRec(mousePos, pieceRect)) {
                std::cout << "Collision: " << i << std::endl;

                // Find the pawn that is being promoted
                for (auto& piece : pieces) {
                    if (Vector2Equals(piece.position, promotionPosition) && piece.type == PAWN && piece.color == color) {
                        std::cout<<"Pawn found"<<std::endl;
                        // Replace the pawn with the selected piece
                        piece.type = static_cast<PieceType>(i + 1); // i + 1 because 0: Rook, 1: Knight, 2: Bishop, 3: Queen
                        piece.texture = promotionTexture[(color == 0 ? i : i + 6)];
                        break;
                    }
                }

                PawnPromo = false;  // Close the promotion menu
                break;
            }
        }
    }
}

void Board::CapturePiece(int capturedPieceIndex)
{std::cout << "Captured piece index: " << capturedPieceIndex << std::endl;
    float offset = 92.0; // Space between pieces in the captured section
    int piecesPerRow = 7; // Number of pieces per row in the captured section
    
    int pieceValue = GetPieceValue(pieces[capturedPieceIndex].type);

    if (pieces[capturedPieceIndex].color == 1) { // Captured by black
    blackScore += pieceValue;
        int row = whiteCapturedCount / piecesPerRow;
        int col = whiteCapturedCount % piecesPerRow;

        pieces[capturedPieceIndex].position.x = boardPosition.x + 10 * squareSize + offset * col;
        pieces[capturedPieceIndex].position.y = boardPosition.y + row * squareSize + offset;

        whiteCapturedCount++;
    } else { // Captured by white
    whiteScore += pieceValue;
        int row = blackCapturedCount / piecesPerRow;
        int col = blackCapturedCount % piecesPerRow;

        pieces[capturedPieceIndex].position.x = boardPosition.x + 10 * squareSize + offset * col;
        pieces[capturedPieceIndex].position.y = boardPosition.y + 7.2 * squareSize - (row * squareSize + offset);

        blackCapturedCount++;
    }
    pieces[capturedPieceIndex].captured = true;
}

void Board::ExecuteCastling( Piece &king, bool kingside,std::vector<Piece> &pieces,const Vector2 originalPosition) {
    
    int kingY = static_cast<int>((originalPosition.y - boardPosition.y) / squareSize);
    int rookX = kingside ? 7 : 0; // Kingside or Queenside rook
    int rookY = kingY;

    // Find the rook
    Piece* rook = nullptr;
    for (auto& piece : pieces) {
        if (piece.type == ROOK && piece.color == king.color &&
            static_cast<int>((piece.position.x - boardPosition.x) / squareSize) == rookX &&
            static_cast<int>((piece.position.y - boardPosition.y) / squareSize) == rookY) {
            rook = &piece;
            break;
        }
    }
    std::cout<<"King position = "<<king.position.y<<std::endl;

    // Debugging log to track the state of the king and rook
    if (!rook) {
        std::cout << "Rook not found for castling! RookX: " << rookX << ", RookY: " << rookY << std::endl;
        return; // Rook not found
    }
     std::cout << "Rook found at (" << rook->position.x << ", " << rook->position.y << ")" << std::endl;

         // Check if the king or rook has moved
    if (king.hasMoved || rook->hasMoved) {
        std::cout << "Castling not allowed, king or rook has moved." << std::endl;
        return; // Either the king or rook has moved
    }

    // Move the king
    Vector2 newKingPos = { boardPosition.x + (kingside ? 6 : 2) * squareSize, king.position.y + boardPosition.y};
    king.position.x = newKingPos.x;
    king.position.y = newKingPos.y;

    // Move the rook
    Vector2 newRookPos = { boardPosition.x + (kingside ? 5 : 3) * squareSize, rook->position.y};
    rook->position.x = newRookPos.x;
    rook->position.y = newRookPos.y;
    
    // Mark king and rook as moved
    king.hasMoved = true;
    rook->hasMoved = true;
    // Log the positions for debugging
    std::cout << "Castling performed: King moved to (" << newKingPos.x << ", " << newKingPos.y << "), Rook moved to ("
    << newRookPos.x << ", " << newRookPos.y << ")\n";
}

void Board::ExecuteEnPassant(Piece& capturingPawn, std::vector<Piece>& pieces, const Vector2& originalPosition, const Vector2& newPosition) {

    float capturedPawnX = newPosition.x;
    float capturedPawnY = originalPosition.y; // The captured pawn is directly behind the capturing pawn

    std::cout << "Attempting to capture pawn at (" << capturedPawnX << ", " << capturedPawnY << ")" << std::endl;

    for (const auto &piece : pieces) {
            std::cout << "Piece Type: " << piece.type << ", Position: (" << piece.position.x << ", " << piece.position.y << ")" << std::endl;
        }
    for (std::size_t i = 0; i < pieces.size(); ++i) {
        
        std::cout << "Checking piece at index " << i << " with position (" << pieces[i].position.x << ", " << pieces[i].position.y << ")" << std::endl;

        if (pieces[i].position.x == capturedPawnX && pieces[i].position.y == capturedPawnY && pieces[i].type == PAWN && pieces[i].color != capturingPawn.color) {
            std::cout << "Capturing Pawn at (" << capturedPawnX << ", " << capturedPawnY << ")" << std::endl;
            this->CapturePiece(i);
            break;
        }
    }

    capturingPawn.position = newPosition; // Move the capturing pawn to the new position
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

            if (CheckCollisionPointRec(mousePos,pieceRect)){
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
                snappedY = std::max(boardPosition.y, std::min(snappedY, boardPosition.y + 7 * squareSize));

                Vector2 newPosition = Vector2 {snappedX,snappedY};

                //Checking the turn of the player(UI improvement)
                    if (pieces[draggedPieceIndex].color == CurrentPlayer) {
                    if (originalPosition.x != newPosition.x || originalPosition.y != newPosition.y) {
                        
                        if(pieces[draggedPieceIndex].type==ROOK)
                        {
                            pieces[draggedPieceIndex].hasMoved = true;
                        }


                        // Check if the move is valid
                        if (MoveValidator::IsMoveValid(pieces[draggedPieceIndex], newPosition, pieces, originalPosition, *this)) {
                            pieces[draggedPieceIndex].position = newPosition;

                        

                        //For capturing 
                        for (std::size_t i = 0; i < pieces.size(); ++i) {
                        if (pieces[i].position.x == newPosition.x && pieces[i].position.y == newPosition.y && pieces[i].color != pieces[draggedPieceIndex].color) {
                            CapturePiece(i);  // Capture the piece
                            break;
                        }
                    }

                    pieces[draggedPieceIndex].position = newPosition;
                    if(PawnPromo == true)
                    {
                        CurrentPlayer = CurrentPlayer;
                    }
                    else{
                    CurrentPlayer = (CurrentPlayer +1) % 2;
                    }

                  }
               else{ 
                    pieces[draggedPieceIndex].position = originalPosition;
                }
          } 
            else{ 
            pieces[draggedPieceIndex].position = originalPosition;
            } 
      }else{
        pieces[draggedPieceIndex].position = originalPosition;
     
    }
}}
}                    
}  

