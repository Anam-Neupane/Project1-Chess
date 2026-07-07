#include "ui/button.hpp"
#include "core/Board.hpp"
#include "core/GameState.hpp"
#include <iostream>
#include <raylib.h>
#include "raymath.h"
#include "engine/StockfishEngine.hpp"
#include "ui/slider.hpp"
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

int engineColor = 0; // 0 = black, 1 = white

int main()
{

    // Fixed game dimensions
    int screenHeight = 1025;
    int screenWidth = 1100;

    // These are the fixed game resolution (virtual resolution)
    // Added extra width for side panel (player turn, captured pieces display)
    int sidePanelWidth = 200;
    int gameScreenWidth = screenWidth + sidePanelWidth;
    int gameScreenHeight = screenHeight;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight - 155, "♟️ Chess");

    SetWindowMinSize(800, 600); // minimum window size

    SetTargetFPS(60);

    // Render texture for scaling - all game content is drawn here at fixed resolution
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);



    Button startButton{"resource/Picture1.png", {0, 0}, 0.45};
    Button engineButton{"resource/Picture2.png", {0, 0}, 0.45};
    Button exitButton{"resource/Picture3.png", {0, 0}, 0.45};
    Button restartButton{"resource/restart.png", {0, 0}, 0.45};
    Button menuButton{"resource/mainmenu.png", {0, 0}, 0.45};
    Button howtoplayButton{"resource/howto.png", {0, 0}, 0.45};
    Button resignButton{"resource/resign.png", {0, 0}, 0.35};
    Button whitePlayer{"resource/white.png", {0, 0}, 0.35};
    Button blackPlayer{"resource/black.png", {0, 0}, 0.35};
    Slider engineDifficultySlider({0.0f, 0.0f}, 360.0f, 1, 20, 10);

    bool exit = false;

    Texture2D boardTexture = LoadTexture("resource/board1.png");
    Texture2D backgroundTexture = LoadTexture("resource/background.jpg");
    SetTextureFilter(backgroundTexture, TEXTURE_FILTER_BILINEAR);

    // Create GameState object and pass to Board
    GameState chessGameState;
    Board B1(&chessGameState);
    B1.LoadPieces();
    B1.LoadPromotionTexture();

    // Current app state (menu navigation)
    AppState appState = MAIN_MENU;

    StockfishEngine *engine = nullptr;
    bool engineLaunchFailed = false;
    bool enginePlayerselect = false;
    std::string engineLaunchErrorMessage;
    std::size_t lastObservedUciMoveCount = 0;
    bool deferEngineMoveOneFrame = false;

    auto shutdownEngine = [&]()
    {
        if (engine != nullptr)
        {
            engine->shutdown();
            delete engine;
            engine = nullptr;
        }
    };

    auto launchEngineGame = [&](int playerColor)
    {
        engineColor = 1 - playerColor;

        shutdownEngine();
        engineLaunchFailed = false;
        engineLaunchErrorMessage.clear();
        enginePlayerselect = false;

        chessGameState.setGameMode(GameMode::VS_ENGINE);
        if (playerColor == 0)
        {
            chessGameState.flipBoard();
        }

        engine = new StockfishEngine();
        if (!engine->init())
        {
            std::cerr << "Failed to start Stockfish. Is stockfish.exe in project root?" << std::endl;
            shutdownEngine();
            chessGameState.setGameMode(GameMode::PVP_LOCAL);
            appState = MAIN_MENU;
            engineLaunchFailed = true;
            engineLaunchErrorMessage = "Engine unavailable. You can still play local multiplayer or exit from the menu.";
            return;
        }

        engine->setDifficulty(engineDifficultySlider.GetValue());
        engine->newGame();
        appState = ENGINE_GAME;
    };

    auto drawHint = [&](const char *hintText, int centerX, int y, int fontSize, Color tint)
    {
        int hintWidth = MeasureText(hintText, fontSize);
        DrawText(hintText, centerX - hintWidth / 2, y, fontSize, tint);
    };

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
        bool anyMousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
                               IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) ||
                               IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE);

        if (B1.uciMoveList.size() != lastObservedUciMoveCount)
        {
            lastObservedUciMoveCount = B1.uciMoveList.size();
            deferEngineMoveOneFrame = true;
        }

        // Handle input based on the current app state
        if (appState == MAIN_MENU)
        {
            bool engineLaunchAttemptedThisFrame = false;
            bool suppressMenuButtons = false;
            bool sliderConsumedThisFrame = false;

            if (enginePlayerselect)
            {
                Vector2 whiteSize = whitePlayer.GetSize();
                Vector2 blackSize = blackPlayer.GetSize();
                int gap = 50;
                float buttonsY = 420.0f;
                float totalWidth = whiteSize.x + gap + blackSize.x;
                float startX = (gameScreenWidth - totalWidth) / 2.0f;
                float sliderX = (gameScreenWidth - 360.0f) / 2.0f;
                float sliderY = 377.0f;

                engineDifficultySlider.SetPosition({sliderX, sliderY});
                sliderConsumedThisFrame = engineDifficultySlider.HandleInput(mousePosition,
                                                                            mousePressed,
                                                                            IsMouseButtonReleased(MOUSE_BUTTON_LEFT));

                whitePlayer.SetPosition({startX, buttonsY});
                blackPlayer.SetPosition({startX + whiteSize.x + gap, buttonsY});

                bool clickedWhite = whitePlayer.isPressed(mousePosition, mousePressed);
                bool clickedBlack = blackPlayer.isPressed(mousePosition, mousePressed);

                if (clickedWhite)
                {
                    std::cout << "Starting Engine Game as White" << std::endl;
                    engineLaunchAttemptedThisFrame = true;
                    suppressMenuButtons = true;
                    launchEngineGame(1);
                }
                else if (clickedBlack)
                {
                    std::cout << "Starting Engine Game as Black" << std::endl;
                    engineLaunchAttemptedThisFrame = true;
                    suppressMenuButtons = true;
                    launchEngineGame(0);
                }
                else if (anyMousePressed && !sliderConsumedThisFrame)
                {
                    enginePlayerselect = false;
                    suppressMenuButtons = true;
                }
            }

            bool dismissEngineError = engineLaunchFailed &&
                                      !engineLaunchAttemptedThisFrame &&
                                      (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
                                       IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) ||
                                       IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE));

            if (dismissEngineError)
            {
                engineLaunchFailed = false;
                engineLaunchErrorMessage.clear();
                suppressMenuButtons = true;
            }

            if (!engineLaunchFailed && !enginePlayerselect && !suppressMenuButtons && !engineLaunchAttemptedThisFrame)
            {
                startButton.SetDrawScale(0.9f);
                restartButton.SetDrawScale(1.0f);
                engineButton.SetDrawScale(0.9f);
                exitButton.SetDrawScale(0.9f);

                float btnStartY = 365.0f;
                Button::UpdateThreeButtonPositionsInPanel(startButton, engineButton, exitButton, 350.0f, 600.0f, btnStartY);

                if (startButton.isPressed(mousePosition, mousePressed))
                {
                    std::cout << "Starting of 1v1 Game" << std::endl;
                    chessGameState.setGameMode(GameMode::PVP_LOCAL);
                    appState = GAME; // Switching to the game state
                }

                if (engineButton.isPressed(mousePosition, mousePressed))
                {
                    std::cout << "Choosing engine side" << std::endl;
                    enginePlayerselect = true;
                    engineLaunchFailed = false;
                    engineLaunchErrorMessage.clear();
                }

                if (exitButton.isPressed(mousePosition, mousePressed))
                {
                    exit = true;
                }
            }
        }

        if (appState == GAME || appState == ENGINE_GAME)
        {
            if (IsKeyPressed(KEY_SPACE) && !B1.Checkmate && !B1.Stalemate)
            {
                Paused = !Paused;
            }

            // Toggle valid move highlights with 'M' key
            if (IsKeyPressed(KEY_M) && !Paused && !B1.Checkmate && !B1.Stalemate)
            {
                B1.ToggleShowValidMoves();
            }

            //  The trigger key can be changed later
            if (IsKeyPressed(KEY_H) && !Paused && !B1.Checkmate && !B1.Stalemate)
            {
                B1.showMoveHistory = !B1.showMoveHistory;
            }

            if (!B1.Checkmate && !B1.Stalemate && !B1.Resigned && resignButton.isPressed(mousePosition, mousePressed))
            {
                B1.Resigned = true;
                B1.resignedPlayer = chessGameState.getCurrentPlayer();
                mousePressed = false; // Consume click so it doesn't also trigger exit button
            }
        }

        if ((appState == GAME || appState == ENGINE_GAME) && (B1.Checkmate || B1.Stalemate || B1.Resigned) && !Paused)
        {
            // position buttons in the size panel
            restartButton.SetDrawScale(0.70f);
            menuButton.SetDrawScale(0.70f);
            exitButton.SetDrawScale(0.70f);

            float panelButtonStartY = 320.f;
            Button::UpdateThreeButtonPositionsInPanel(restartButton,
                 menuButton, exitButton, 914.0f, 380.0f, panelButtonStartY);

            if (restartButton.isPressed(mousePosition, mousePressed))
            {
                B1.Reset();
                if (appState == ENGINE_GAME && engineColor == 1)
                    chessGameState.flipBoard();
                if (engine != nullptr)
                    engine->reset(); // sends "ucinewgame" - reuses the process
            }
            if (menuButton.isPressed(mousePosition, mousePressed))
            {
                B1.Reset();
                appState = MAIN_MENU;
                if (engine != nullptr)
                {
                    engine->shutdown();
                    delete engine;
                    engine = nullptr; // null so that we don't call getmove in a dead engine
                }
            }
            if (exitButton.isPressed(mousePosition, mousePressed))
            {
                exit = true;
            }
        }

        if ((appState == GAME || appState == ENGINE_GAME) && Paused)
        {

            restartButton.SetDrawScale(1.0f);
            menuButton.SetDrawScale(1.0f);
            exitButton.SetDrawScale(1.0f);

            int windowWidth = gameScreenWidth;
            int windowHeight = gameScreenHeight;

            int resumeTextY = ((windowHeight - 40) / 4 - 30) + 50;
            int buttonStartY = resumeTextY + 50;

            Button::UpdateThreeButtonPositions(
                restartButton, menuButton, exitButton, windowWidth, windowHeight, buttonStartY);

            if (restartButton.isPressed(mousePosition, mousePressed))
            {
                B1.Reset();
                if (appState == ENGINE_GAME && engineColor == 1)
                    chessGameState.flipBoard();
                if (engine != nullptr)
                    engine->reset();

                Paused = !Paused;
            }
            if (menuButton.isPressed(mousePosition, mousePressed))
            {
                B1.Reset();
                appState = MAIN_MENU;
                if (engine != nullptr)
                {
                    engine->shutdown();
                    delete engine;
                    engine = nullptr;
                }
                Paused = !Paused;
            }
            if (exitButton.isPressed(mousePosition, mousePressed))
            {
                exit = true;
            }
        }

        // Draw everything to the render texture at fixed resolution
        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (appState == MAIN_MENU)
        {
            DrawTexturePro(backgroundTexture,
                           (Rectangle){0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height},
                           (Rectangle){0, 0, (float)gameScreenWidth, (float)gameScreenHeight},
                           (Vector2){0, 0}, 0.0f, WHITE);

            // Modern main menu with centered card
            float panelX = 350.0f, panelY = 190.0f, panelW = 600.0f, panelH = 600.0f;
            DrawRectangleRounded({panelX, panelY, panelW, panelH}, 0.06f, 8, Fade(WHITE, 0.85f));


            DrawRectangleRounded({panelX + 10, panelY + 15, panelW - 20, 4}, 0.5f, 4, BROWN);

            const char *menuTitle = "CHESS";
            int titleW = MeasureText(menuTitle, 56);
            DrawText(menuTitle, panelX + (panelW - titleW) / 2, panelY + 55, 56, Color{35, 55, 30, 255});

            const char *subtitle = "Choose your game mode";
            int subW = MeasureText(subtitle, 20);
            DrawText(subtitle, panelX + (panelW - subW) / 2, panelY + 125, 20, BROWN);

            startButton.SetDrawScale(1.0f);
            engineButton.SetDrawScale(1.0f);
            exitButton.SetDrawScale(1.0f);

            float btnStartY = panelY + 175.0f;
            Button::UpdateThreeButtonPositionsInPanel(startButton, engineButton, exitButton, panelX, panelW, btnStartY);

            startButton.DrawWithHover(mousePosition);
            engineButton.DrawWithHover(mousePosition);
            exitButton.DrawWithHover(mousePosition);

            if (enginePlayerselect)
            {
                DrawRectangle(0, 0, gameScreenWidth, gameScreenHeight, Fade(BLACK, 0.35f));

                int panelWidth = 860;
                int panelHeight = 420;
                int panelX = (gameScreenWidth - panelWidth) / 2;
                int panelY = 185;
                DrawRectangle(panelX, panelY, panelWidth, panelHeight, Fade(DARKGRAY, 0.94f));
                DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, RAYWHITE);

                const char *title = "Choose your color";
                int titleSize = 40;
                int titleWidth = MeasureText(title, titleSize);
                DrawText(title, panelX + (panelWidth - titleWidth) / 2, panelY + 20, titleSize, RAYWHITE);

                const char *subtitle = "Click white to play first, or black to let the engine start.";
                int subtitleSize = 22;
                int subtitleWidth = MeasureText(subtitle, subtitleSize);
                DrawText(subtitle, panelX + (panelWidth - subtitleWidth) / 2, panelY + 70, subtitleSize, LIGHTGRAY);

                engineDifficultySlider.Draw("Engine difficulty");

                Vector2 whiteSize = whitePlayer.GetSize();
                Vector2 blackSize = blackPlayer.GetSize();
                int gap = 40;
                float buttonsY = panelY + 235.0f;
                float totalWidth = whiteSize.x + gap + blackSize.x;
                float startX = (gameScreenWidth - totalWidth) / 2.0f;

                whitePlayer.SetPosition({startX, buttonsY});
                blackPlayer.SetPosition({startX + whiteSize.x + gap, buttonsY});

                whitePlayer.DrawWithHover(mousePosition);
                blackPlayer.DrawWithHover(mousePosition);

                drawHint("Click anywhere to remove this overlay", panelX + panelWidth / 2, panelY + panelHeight - 34, 20, RAYWHITE);
            }
            else if (engineLaunchFailed)
            {
                DrawRectangle(0, 0, gameScreenWidth, gameScreenHeight, Fade(BLACK, 0.35f));

                int panelWidth = 920;
                int panelHeight = 150;
                int panelX = (gameScreenWidth - panelWidth) / 2;
                int panelY = 210;
                DrawRectangle(panelX, panelY, panelWidth, panelHeight, Fade(MAROON, 0.92f));
                DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, RAYWHITE);

                const char *errorTitle = "Engine unavailable";
                int errorTitleSize = 38;
                int errorTitleWidth = MeasureText(errorTitle, errorTitleSize);
                DrawText(errorTitle, panelX + (panelWidth - errorTitleWidth) / 2, panelY + 18, errorTitleSize, RAYWHITE);

                int errorTextSize = 24;
                int errorTextWidth = MeasureText(engineLaunchErrorMessage.c_str(), errorTextSize);
                DrawText(engineLaunchErrorMessage.c_str(), panelX + (panelWidth - errorTextWidth) / 2, panelY + 70, errorTextSize, WHITE);

                drawHint("Click anywhere to remove this overlay", panelX + panelWidth / 2, panelY + panelHeight - 34, 20, RAYWHITE);
            }
        }

        else if (appState == GAME || appState == ENGINE_GAME)
        {

            if (Paused)
            {
                restartButton.SetDrawScale(1.0f);
                menuButton.SetDrawScale(1.0f);
                exitButton.SetDrawScale(1.0f);

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

                int buttonStartY = resumeTextY + 50;
                Button::UpdateThreeButtonPositions(
                    restartButton, menuButton, exitButton, windowWidth, windowHeight, buttonStartY);

                // Draw buttons
                restartButton.DrawWithHover(mousePosition);
                menuButton.DrawWithHover(mousePosition);
                exitButton.DrawWithHover(mousePosition);
            }

            // Drawing game screen
            if (!Paused)
            {
                DrawTexture(boardTexture, 0, 55, WHITE); // Drawing the Board

                B1.DrawLastMoveHightlight();  // Draw highlight for last move of piece
                B1.DrawCheckHighlight();      // When King is in Check
                B1.DrawValidMoveHighlights(); // Draw valid move highlight

                // Side panel
                DrawRectangle(914, 55, sidePanelWidth + 180, 910, BROWN);
                bool gameOver = B1.Checkmate || B1.Stalemate || B1.Resigned;
                if (!gameOver)
                {                
                    B1.DrawPlayer();
                }

                B1.DrawPieces();

                if (B1.PawnPromo)
                {
                    int windowWidth = gameScreenWidth;   // Virtual window width
                    int windowHeight = gameScreenHeight; // Virtual window height

                    int PosX = ((windowWidth - 4 * squareSize) / 2);
                    int PosY = ((windowHeight - 40) / 2 - 30);

                    float cardX = windowWidth / 4.0f;
                    float cardY = windowHeight / 2.8f; 
                    float cardW = windowWidth / 2.0f; 
                    float cardH = windowHeight / 4.0f;

                    DrawRectangleRounded({cardX, cardY, cardW, cardH}, 0.06f, 8, Fade(LIGHTGRAY, 0.55f));

                    const char *Promotiontxt = "Choose a Piece for Promotion";
                    int PromotiontxtWidth = MeasureText(Promotiontxt, 40);
                    int PromotiontxtX = (windowWidth - PromotiontxtWidth) / 2;
                    int PromotiontxtY = (windowHeight - 40) / 2 - 80;
                    DrawText(Promotiontxt, PromotiontxtX, PromotiontxtY, 40, WHITE);

                    B1.DrawPromotionMenu({static_cast<float>(PosX), static_cast<float>(PosY)}, B1.p1);
                }

                else if (B1.Checkmate)
                {
                    const char *winTitle = "CHECKMATE"; 
                    const char *winner = B1.Cwhite ? "White Wins!" : "Black Wins!";
                    Color accent = {255, 203, 107, 220}; 

                    DrawRectangleRounded({922, 70, 364, 180}, 0.06f, 8, Fade(BLACK, 0.55f));
                    DrawRectangleRoundedLines({922, 70, 364, 180}, 0.06f, 8, Fade(RAYWHITE, 0.15f));
                    DrawRectangleRounded({932, 80, 344, 4}, 0.5f, 4, accent); 

                    int titleW = MeasureText(winTitle, 36);
                    DrawText(winTitle, 1104 - titleW / 2, 110, 36, RAYWHITE); 
                    int winnerW = MeasureText(winner, 24); 
                    DrawText(winner, 1104 - winnerW / 2, 165, 24, BEIGE); 

                    // Buttons
                    restartButton.SetDrawScale(0.70f); 
                    menuButton.SetDrawScale(0.70f); 
                    exitButton.SetDrawScale(0.70f); 
                    Button::UpdateThreeButtonPositionsInPanel(restartButton, menuButton, exitButton, 914.0f, 380.0f, 320.0f);
                    restartButton.DrawWithHover(mousePosition); 
                    menuButton.DrawWithHover(mousePosition); 
                    exitButton.DrawWithHover(mousePosition);  
                }

                else if (B1.Stalemate)
                {
                    const char *title = "DRAW";
                    Color accent = {180, 180, 180, 220}; 

                    DrawRectangleRounded({922, 70, 364, 180}, 0.06f, 8, Fade(BLACK, 0.55f));
                    DrawRectangleRoundedLines({922, 70, 364, 180}, 0.06f, 8 , Fade(RAYWHITE, 0.15f));
                    DrawRectangleRounded({932, 80, 344, 4} , 0.5f , 4, accent); 
                    
                    int titleW = MeasureText(title, 36); 
                    DrawText(title, 1104 - titleW / 2, 135, 36, RAYWHITE); 

                    restartButton.SetDrawScale(0.70f);
                    menuButton.SetDrawScale(0.70f); 
                    exitButton.SetDrawScale(0.70f); 
                    
                    Button::UpdateThreeButtonPositionsInPanel(
                        restartButton, menuButton, exitButton, 914.0f, 380.0f, 320.0f);
                    // Draw buttons
                    restartButton.DrawWithHover(mousePosition);
                    menuButton.DrawWithHover(mousePosition);
                    exitButton.DrawWithHover(mousePosition);
                }

                else if (B1.Resigned)
                {
                    const char *title = "RESIGNATION";                    
                    const char *winner = (B1.resignedPlayer == 1) ? "Black Wins!" : "White Wins!";
                    Color accent = {220, 80, 80, 220}; 
                    
                    DrawRectangleRounded({922, 70, 364, 180}, 0.06f, 8, Fade(BLACK, 0.55f));
                    DrawRectangleRoundedLines({922, 70, 364, 180}, 0.06f, 8, Fade(RAYWHITE, 0.15f));
                    DrawRectangleRounded({932, 80, 344, 4}, 0.5f, 4, accent); 

                    int titleW = MeasureText(title, 30);
                    DrawText(title, 1104 - titleW/ 2, 110, 30, RED);
                    int winnerW = MeasureText(winner, 24);
                    DrawText(winner, 1104 - winnerW / 2, 165, 24, BEIGE); 

                    restartButton.SetDrawScale(0.70f); 
                    menuButton.SetDrawScale(0.70f); 
                    exitButton.SetDrawScale(0.70f); 

                    Button::UpdateThreeButtonPositionsInPanel(restartButton,
                                                       menuButton,exitButton,914.0f,380.0f,320.0f);
                    restartButton.DrawWithHover(mousePosition);
                    menuButton.DrawWithHover(mousePosition);
                    exitButton.DrawWithHover(mousePosition);
                }

            else
                {    
                    B1.DrawMoveHistory();
                    resignButton.SetDrawScale(1.0f);
                    resignButton.SetPosition({1030.0f, 940.0f});
                    resignButton.DrawWithHover(mousePosition);
                    B1.UpdateDragging();

                    if (appState == ENGINE_GAME && engine != nullptr && !B1.Checkmate && !B1.Stalemate && !B1.PawnPromo && !B1.Resigned && !Paused && chessGameState.getCurrentPlayer() == engineColor)
                    {
                        if (deferEngineMoveOneFrame)
                        {
                            deferEngineMoveOneFrame = false;
                        }
                        else
                        {
                            EngineMove em = engine->getMove(B1.uciMoveList);
                            if (em.isValid)
                            {
                                B1.ApplyEngineMove(em);
                            }
                        }
                    }

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
    shutdownEngine();
    UnloadRenderTexture(target);
    UnloadTexture(boardTexture);

    CloseWindow();

    return 0;
}
