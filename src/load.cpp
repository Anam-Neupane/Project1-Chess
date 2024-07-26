#include <raylib.h>
#include <cstddef>
#include "button.hpp"
#include <iostream>

enum GameState
{
    MAIN_MENU,
    GAME
};

int main()
{
    // Load the image for the main menu background
    Image look = LoadImage("resource\\cut.png");

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

    Button startButton{"resource\\Picture1.png", {300, 300}, 0.5};
    Button engineButton{"resource\\Picture2.png", {300, 450}, 0.5};
    Button exitButton{"resource\\Picture3.png", {300, 600}, 0.5};
    bool exit = false;

    // Game textures

    figure tpieces("resource\\figure1.png",0.4);
    Texture2D boardTexture = LoadTexture("resource\\board0.png");

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

        // Start drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

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
            // Draw the game screen
            
            DrawTexture(boardTexture, 0, 0, WHITE);
            tpieces.Draw();
        }

        // End drawing
        EndDrawing();
    }

    // Unload the textures and close the window
    UnloadTexture(background);
    UnloadTexture(boardTexture);
    CloseWindow();

    return 0;
}
