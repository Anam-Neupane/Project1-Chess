#include "button.hpp"
#include "Board.hpp"
#include <raylib.h>
#include <iostream>
#include "raymath.h"
// #include <cstddef>

#define MIN(a, b) ((a)<(b)? (a) : (b))

enum GameState
{
    MAIN_MENU,
    GAME,
    ENGINE_GAME
};

bool Paused = false;

GameMode currentGameMode = PVP_LOCAL;

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
    
    Image look = LoadImage("resource/cut.png");
    Image New = LoadImage("resource/New.png");

    // Checking if the image loaded successfully
    if (look.data == nullptr)
    {
        return -1;
    }

    // Screen width and height of the image used as window's dimensions
    int screenHeight = look.height;
    int screenWidth = look.width;
    
    std::cout << "Width: " << screenWidth << ", Height: " << screenHeight << std::endl;

    // These are the fixed game resolution (virtual resolution)
    // Add extra width for side panel (player turn, captured pieces display)
    int sidePanelWidth = 200;
    int gameScreenWidth = screenWidth + sidePanelWidth;
    int gameScreenHeight = screenHeight;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "♟️ Chess");

    SetWindowMinSize(800, 600);//minimum window size 

    SetTargetFPS(60);

    // Render texture for scaling - all game content is drawn here at fixed resolution
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    // Loading the texture from the image
    Texture2D background = LoadTextureFromImage(New);

    UnloadImage(look);
    UnloadImage(New);

    Button startButton{"resource/Picture1.png", {300, 300}, 0.5};
    Button engineButton{"resource/Picture2.png", {300, 450}, 0.5};
    Button exitButton{"resource/Picture3.png", {300, 600}, 0.5};
    bool exit = false;

    Texture2D boardTexture = LoadTexture("resource/board0.png");
    Board B1;
    B1.LoadPieces();
    B1.LoadPromotionTexture();

    // Current game state
    GameState gameState = MAIN_MENU;

    while (!WindowShouldClose() && exit == false)
    {
        // Calculate scaling factor to fit game in window while maintaining aspect ratio
        float scale = MIN((float)GetScreenWidth()/gameScreenWidth, (float)GetScreenHeight()/gameScreenHeight);
        
        // Calculate offset to center the game in the window (letterboxing)
        float offsetX = (GetScreenWidth() - (gameScreenWidth * scale)) * 0.5f;
        float offsetY = (GetScreenHeight() - (gameScreenHeight * scale)) * 0.5f;

        // Apply mouse transformation so GetMousePosition() returns virtual coordinates
        // This makes Board::UpdateDragging() work correctly without any changes
        SetMouseOffset((int)(-offsetX), (int)(-offsetY));
        SetMouseScale(1.0f / scale, 1.0f / scale);

        // Now GetMousePosition() automatically returns virtual game coordinates
        Vector2 mousePosition = GetMousePosition();
        bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        // Handle input based on the current game state
        if (gameState == MAIN_MENU)
        {
            if (startButton.isPressed(mousePosition, mousePressed))
            {
                std::cout << "Start of 1v1 Game" << std::endl;
                currentGameMode = PVP_LOCAL;
                B1.SetGameMode(PVP_LOCAL);
                gameState = GAME; // Switching to the game state

            }

            if (engineButton.isPressed(mousePosition, mousePressed))
            {
                std::cout << "Starting Engine Game" << std::endl;
                currentGameMode = VS_ENGINE;
                B1.SetGameMode(VS_ENGINE);
                gameState = GAME; // Switching to the game state
            }

            if (exitButton.isPressed(mousePosition, mousePressed))
            {
                exit = true;
            }
        }
        
        if(gameState == GAME || gameState == ENGINE_GAME)
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

        // Draw everything to the render texture at fixed resolution
        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (gameState == MAIN_MENU)
        {
            // Drawing main menu background and buttons
            DrawTexture(background, 0, 0, WHITE);
            DrawText("Chess", 380, 130, 100, BLACK);
            startButton.Draw();
            engineButton.Draw();
            exitButton.Draw();
        }
        else if (gameState == GAME || gameState == ENGINE_GAME)
        {

            if(Paused)
            {
                int windowWidth = gameScreenWidth;  // Virtual window width
                int windowHeight = gameScreenHeight ;  // Virtual window height
                
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
            // Drawing game screen
            if(!Paused){

                if(B1.PawnPromo)
                {
                    int windowWidth = gameScreenWidth;  // Virtual window width
                    int windowHeight = gameScreenHeight;  // Virtual window height

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
                    int windowWidth = gameScreenWidth;  // Virtual window width
                    int windowHeight = gameScreenHeight;  // Virtual window height

                    int PosX = ((windowWidth - 4 * squareSize) / 2);
                    int PosY = ((windowHeight - 40) / 2 - 30);
                    DrawRectangle(0, 0, windowWidth, windowHeight, Fade(MAROON, 0.6f));
                    DrawText("Checkmate ¬_¬ ", PosX - 80, PosY - 130, 90, WHITE);
                    B1.Cwhite ? DrawText("White Wins", PosX + 35, PosY, 60, BEIGE) : DrawText("Black Wins", PosX, PosY, 60, BEIGE);

                }

                else{

                     DrawTexture(boardTexture, 0, 55, WHITE);
                    // Side panel - fits within the extended screen width
                     DrawRectangle(914,55, sidePanelWidth + 180,910,BROWN);
                    //  DrawRectangle(1100,55,700,455,DARKBROWN);
                    //  DrawRectangle(1100,510,700,455,BEIGE);

                     B1.UpdateDragging();
                     B1.DrawPlayer(); 
                     B1.DrawPieces(); 
                }
            }

        }

        EndTextureMode();

        // Now draw the render texture to the actual screen, scaled and centered
        BeginDrawing();
            ClearBackground(BLACK);  // Letterbox color
            
            // Draw render texture scaled to fit window
            DrawTexturePro(target.texture, 
                (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                (Rectangle){ offsetX, offsetY, (float)gameScreenWidth * scale, (float)gameScreenHeight * scale },
                (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndDrawing();
    }

    // Unloading textures and closing the window
    UnloadRenderTexture(target);
    UnloadTexture(background);
    UnloadTexture(boardTexture);
    B1.UnloadPieces();

    CloseWindow();

    return 0;
}
