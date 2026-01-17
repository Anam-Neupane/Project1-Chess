#include "ui/button.hpp"
#include "core/Board.hpp"
#include "core/GameState.hpp"
#include <raylib.h>
#include <iostream>
#include "raymath.h"
// #include <cstddef>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

// Renamed to avoid conflict with GameState class
enum AppState
{
    MAIN_MENU,
    GAME,
    ENGINE_GAME
};

bool Paused = false;


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
    // Added extra width for side panel (player turn, captured pieces display)
    int sidePanelWidth = 200;
    int gameScreenWidth = screenWidth + sidePanelWidth;
    int gameScreenHeight = screenHeight;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight-150, "♟️ Chess");

    SetWindowMinSize(800, 600); // minimum window size

    SetTargetFPS(60);

    // Render texture for scaling - all game content is drawn here at fixed resolution
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    // Loading the texture from the image
    Texture2D background = LoadTextureFromImage(New);

    UnloadImage(look);
    UnloadImage(New);

    Button startButton{"resource/Picture1.png", {0, 0}, 0.45};
    Button engineButton{"resource/Picture2.png", {0, 0}, 0.45};
    Button exitButton{"resource/Picture3.png", {0, 0}, 0.45};
    Button restartButton{"resource/restart.png", {0, 0}, 0.45};
    Button menuButton{"resource/mainmenu.png", {0, 0}, 0.45};
    Button howtoplayButton{"resource/howto.png", {0,0},0.45};

    bool exit = false;

    Texture2D boardTexture = LoadTexture("resource/board1.png");

    // Create GameState object and pass to Board
    GameState chessGameState;
    Board B1(&chessGameState);
    B1.LoadPieces();
    B1.LoadPromotionTexture();

    // Current app state (menu navigation)
    AppState appState = MAIN_MENU;

    while (!WindowShouldClose() && exit == false)
    {
        // Calculate scaling factor to fit game in window while maintaining aspect ratio
        float scale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);

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

        // Handle input based on the current app state
        if (appState == MAIN_MENU)
        {
            if (startButton.isPressed(mousePosition, mousePressed))
            {
                std::cout << "Starting of 1v1 Game" << std::endl;
                chessGameState.setGameMode(GameMode::PVP_LOCAL);
                appState = GAME; // Switching to the game state
            }

            if (engineButton.isPressed(mousePosition, mousePressed))
            {
                std::cout << "Starting Engine Game" << std::endl;
                chessGameState.setGameMode(GameMode::VS_ENGINE);
                appState = GAME; // Switching to the game state
            }

            if (exitButton.isPressed(mousePosition, mousePressed))
            {
                exit = true;
            }
        }

        if (appState == GAME || appState == ENGINE_GAME)
        {
            if (IsKeyPressed(KEY_SPACE) && !B1.Checkmate)
            {
                Paused = !Paused;
            }

            // Toggle valid move highlights with 'M' key
            if(IsKeyPressed(KEY_M) && !Paused && !B1.Checkmate)
            {
                B1.ToggleShowValidMoves();
            }
        }

        if((appState == GAME || appState == ENGINE_GAME) && B1.Checkmate && !Paused) {
            if(restartButton.isPressed(mousePosition, mousePressed)){
                B1.Reset();
            }
            if (menuButton.isPressed(mousePosition, mousePressed)) {
                B1.Reset();
                appState = MAIN_MENU;
            }
            if(exitButton.isPressed(mousePosition,mousePressed)){
                exit = true;
            }
        }

        if((appState == GAME || appState == ENGINE_GAME) && Paused)
        {
            if(restartButton.isPressed(mousePosition, mousePressed)){
                B1.Reset();
                Paused = !Paused;
            }
            if (menuButton.isPressed(mousePosition, mousePressed)) {
                B1.Reset();
                appState = MAIN_MENU;
                Paused = !Paused;
            }
            if(exitButton.isPressed(mousePosition,mousePressed)){
                exit = true;
            }
        }



        // Draw everything to the render texture at fixed resolution
        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (appState == MAIN_MENU)
        {
            // Drawing main menu background and buttons
            DrawTexture(background, 0, 0, WHITE);
            const char *title = "Chess";
            int titleWidth = MeasureText(title, 100);
            int titleX = (gameScreenWidth - titleWidth) / 2;
            DrawText(title, titleX, 130, 100, BLACK);

            // Update button positions every frame
            Button::UpdateMenuButtonPosition(startButton, engineButton, exitButton, gameScreenWidth, gameScreenHeight);

            // Draw buttons with hover effect
            startButton.DrawWithHover(mousePosition);
            engineButton.DrawWithHover(mousePosition);
            exitButton.DrawWithHover(mousePosition);
        }

        else if (appState == GAME || appState == ENGINE_GAME)
        {

            if (Paused)
            {
                int windowWidth = gameScreenWidth;   // Virtual window width
                int windowHeight = gameScreenHeight; // Virtual window height

                const char *pausedText = "Paused";
                const char *resumeText = "Press Space to Resume.......";

                int pausedTextWidth = MeasureText(pausedText, 40);
                int resumeTextWidth = MeasureText(resumeText, 40);

                int pausedTextX = (windowWidth - pausedTextWidth) / 2;
                int pausedTextY = (windowHeight - 40) / 4 - 30;

                int resumeTextX = (windowWidth - resumeTextWidth) / 2;
                int resumeTextY = pausedTextY + 50;

                DrawText(pausedText, pausedTextX, pausedTextY, 40, GREEN);
                DrawText(resumeText, resumeTextX, resumeTextY, 40, GRAY);

                int buttonStartY = resumeTextY + 50 ;
                Button::UpdateThreeButtonPositions(
                    restartButton, menuButton, exitButton, windowWidth, windowHeight, buttonStartY
                );

                //Draw buttons
                restartButton.DrawWithHover(mousePosition);
                menuButton.DrawWithHover(mousePosition);
                exitButton.DrawWithHover(mousePosition);

            }

            // Drawing game screen
            if (!Paused)
            {

                if (B1.PawnPromo)
                {
                    int windowWidth = gameScreenWidth;   // Virtual window width
                    int windowHeight = gameScreenHeight; // Virtual window height

                    int PosX = ((windowWidth - 4 * squareSize) / 2);
                    int PosY = ((windowHeight - 40) / 2 - 30);

                    DrawRectangle(0, 0, windowWidth, windowHeight, Fade(BEIGE, 0.8f));

                    const char *Promotiontxt = "Choose a Piece for Promotion";
                    int PromotiontxtWidth = MeasureText(Promotiontxt, 40);
                    int PromotiontxtX = (windowWidth - PromotiontxtWidth) / 2;
                    int PromotiontxtY = (windowHeight - 40) / 2 - 80;
                    DrawText(Promotiontxt, PromotiontxtX, PromotiontxtY, 40, WHITE);

                    B1.DrawPromotionMenu({static_cast<float>(PosX), static_cast<float>(PosY)}, B1.p1);
                }

                else if (B1.Checkmate)
                {
                    int windowWidth = gameScreenWidth;   // Virtual window width
                    int windowHeight = gameScreenHeight; // Virtual window height

                    //Overlay
                    DrawRectangle(0,0, windowWidth, windowHeight, Fade(MAROON, 0.8f));

                    //Title-Centered
                    const char * title = "CHECKMATE";
                    int titileSize = 100;
                    int titleWidth = MeasureText(title, titileSize);
                    int titleX = (windowWidth - titleWidth) / 2;
                    int titleY = windowHeight / 8; 
                    DrawText ( title , titleX, titleY, titileSize, WHITE);

                    //Winner- centered
                    const char* winner = B1.Cwhite? "White Wins!" : "Black Wins!";
                    int winnerSize = 50;
                    int winnerWidth = MeasureText(winner, winnerSize);
                    int winnerX = (windowWidth - winnerWidth) / 2;
                    int winnerY = titleY + titileSize + 30;
                    DrawText(winner, winnerX, winnerY, winnerSize, BEIGE);

                    //Position buttons
                    int buttonStartY = winnerY + winnerSize + 60;
                    Button::UpdateThreeButtonPositions(
                        restartButton, menuButton,exitButton, windowWidth, windowHeight, buttonStartY
                    );

                    //Draw buttons
                    restartButton.DrawWithHover(mousePosition);
                    menuButton.DrawWithHover(mousePosition);
                    exitButton.DrawWithHover(mousePosition);
                }

                else
                {
                    DrawTexture(boardTexture, 0, 55, WHITE); // Drawing the Board

                    B1.DrawLastMoveHightlight(); // Draw highlight for last move of piece  

                    B1.DrawValidMoveHighlights(); // Draw valid move highlight

                    // Side panel
                    DrawRectangle(914, 55, sidePanelWidth + 180, 910, BROWN);

                    B1.UpdateDragging();
                    B1.DrawPlayer();
                    B1.DrawPieces();
                }
            }
        }

        EndTextureMode();

        // Now draw the render texture to the actual screen, scaled and centered
        BeginDrawing();
        ClearBackground(BLACK); // Letterbox color

        // Draw render texture scaled to fit window
        DrawTexturePro(target.texture,
                       (Rectangle){0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height},
                       (Rectangle){offsetX, offsetY, (float)gameScreenWidth * scale, (float)gameScreenHeight * scale},
                       (Vector2){0, 0}, 0.0f, WHITE);
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
