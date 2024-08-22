#include <raylib.h>
#include <cstddef>
#include "button.hpp"
#include "Board.hpp"
#include <iostream>

enum GameState
{
    MAIN_MENU,
    GAME
};

bool Paused = false;

void ToggleFullScreenWindow(int WindowWidth,int Windowheight)
{
    if(!IsWindowFullscreen())
    {
        int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor),GetMonitorHeight(monitor));
        ToggleFullscreen();
    }
    else{
        ToggleFullscreen();
        SetWindowSize(WindowWidth,Windowheight);
    }
}

int main()
{
    // Load the image for the main menu background
    Image look = LoadImage("resource/cut.png");

    // Check if the image loaded successfully
    if (look.data == nullptr)
    {
        // Image failed to load
        return -1;
    }

    // Set screen dimensions based on the image size
    int screenHeight = look.height;
    int screenWidth = look.width;

    // Initialize the window with the image size
    InitWindow(screenWidth, screenHeight, "♟️ Chess");

    // Set the target FPS
    SetTargetFPS(60);

    // Load the texture from the image
    Texture2D background = LoadTextureFromImage(look);
    UnloadImage(look); // Unload the image from RAM as we have the texture in VRAM now

    Button startButton{"resource/Picture1.png", {300, 300}, 0.5};
    Button engineButton{"resource/Picture2.png", {300, 450}, 0.5};
    Button exitButton{"resource/Picture3.png", {300, 600}, 0.5};
    bool exit = false;

    // Game textures

    // figure tpieces("resource\\figure1.png",0.4);
    Texture2D boardTexture = LoadTexture("resource/board0.png");
    Board B1,B2;
    B1.LoadPieces();
    B1.LoadPromotionTexture();
    // Current game state
    GameState gameState = MAIN_MENU;

    while (!WindowShouldClose() && exit == false)
    {

        
        Vector2 mousePosition = GetMousePosition();
        bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        // Handle input based on the current game state
        if (gameState == MAIN_MENU)
        {
            if (startButton.isPressed(mousePosition, mousePressed))
            {
                std::cout << "Start Button Pressed" << std::endl;
                gameState = GAME; // Switch to the game state
            }

            if (engineButton.isPressed(mousePosition, mousePressed))
            {
                std::cout << "Engine Start Bhruuuhuuhhuhu😂" << std::endl;
            }

            if (exitButton.isPressed(mousePosition, mousePressed))
            {
                exit = true;
            }
        }
        
        if(gameState == GAME)
        {        
               if(IsKeyPressed(KEY_F))
            {   
                ToggleFullScreenWindow(screenWidth,screenHeight);
            }
            if(IsKeyPressed(KEY_SPACE))
            {
                Paused = !Paused;
            }
        
        }

        // Start drawing
        BeginDrawing();
        ClearBackground(BLACK);

        mainmenu:
        if (gameState == MAIN_MENU)
        {
            // Draw the main menu background and buttons
            DrawTexture(background, 0, 0, WHITE);
            startButton.Draw();
            engineButton.Draw();
            exitButton.Draw();
        }
        else if (gameState == GAME)
        {

            if(Paused)
            {
                int windowWidth = GetScreenWidth();  // Get the current window width
                int windowHeight = GetScreenHeight();  // Get the current window height
                
                const char* pausedText = "Paused";
                const char* resumeText = "Press Space to Resume.......";
                
                int pausedTextWidth = MeasureText(pausedText, 40);
                int resumeTextWidth = MeasureText(resumeText, 40);
                
                int pausedTextX = (windowWidth - pausedTextWidth) / 2;
                int pausedTextY = (windowHeight - 40) / 2 - 30;
                
                int resumeTextX = (windowWidth - resumeTextWidth) / 2;
                int resumeTextY = pausedTextY + 50;

                DrawText(pausedText, pausedTextX, pausedTextY, 40, GREEN);
                DrawText(resumeText, resumeTextX, resumeTextY, 40, GRAY);

            }
            // Draw the game screen
            if(!Paused){

                if(B1.PawnPromo)
                {
                    int windowWidth = GetScreenWidth();  // Get the current window width
                    int windowHeight = GetScreenHeight();  // Get the current window height

                    int PosX = ((windowWidth - 4* squareSize) / 2);
                    int PosY = ((windowHeight - 40) /2 -30);

                    DrawRectangle(0, 0, windowWidth, windowHeight, Fade(BEIGE, 0.8f));

                    const char* Promotiontxt = "Choose a Piece for Promotion";
                    int PromotiontxtWidth = MeasureText(Promotiontxt, 40);
                    int PromotiontxtX = (windowWidth - PromotiontxtWidth) / 2;
                    int PromotiontxtY = (windowHeight - 40) / 2 - 80;
                    DrawText(Promotiontxt, PromotiontxtX, PromotiontxtY, 40, WHITE);
                   
                    B1.DrawPromotionMenu({static_cast<float>(PosX),static_cast<float>(PosY)},B1.p1);

                }

                else if (B1.Checkmate) {
                    int windowWidth = GetScreenWidth();
                    int windowHeight = GetScreenHeight();

                    int PosX = ((windowWidth - 4 * squareSize) / 2);
                    int PosY = ((windowHeight - 40) / 2 - 30);
                    DrawTextPro(Font(), "Checkmate", {static_cast<float>(PosX-55), static_cast<float>(PosY - 35)}, {200, 5}, -45, 70, 5, WHITE);
                    B1.Cwhite ? DrawText("White Wins", PosX, PosY, 40, WHITE) : DrawText("Black Wins", PosX, PosY, 40, WHITE);
                    DrawText("Press R to Restart", PosX, PosY + 50, 40, WHITE);

                    if (IsKeyPressed(KEY_R)) {
                        B1.Reset();  // Reset the game state
                        gameState = MAIN_MENU;  // Return to the main menu
                        goto mainmenu;
                    }
                }

                else{

                     DrawTexture(boardTexture, 0, 55, WHITE);
                     DrawRectangle(914,55,180,910,BROWN);
                     DrawRectangle(1100,55,700,455,DARKBROWN);
                     DrawRectangle(1100,510,700,455,BEIGE);
                     B1.UpdateDragging();
                     B1.DrawPlayer(); 
                     B1.DrawPieces(); 
                }
            }

        }

        // End drawing
        EndDrawing();
    }

    // Unload the textures and close the window
    UnloadTexture(background);
    UnloadTexture(boardTexture);
    B1.UnloadPieces();

    CloseWindow();

    return 0;
}
