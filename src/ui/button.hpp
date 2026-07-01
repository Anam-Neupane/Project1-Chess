#pragma once
#include <raylib.h>

class Button
{

private:
    Texture2D texture;
    Vector2 position;

public:
    Button(const char *imagePath, Vector2 imagePosition, float scale);
    ~Button();
    void Draw();
    bool isPressed(Vector2 mousePos, bool mousePressed);
    void SetPosition(Vector2 newPosition); // Change position after creation
    Vector2 GetSize();                     // Get width/height for calculations
    bool isHovered(Vector2 mousePos);      // Check if mouse is over button
    void DrawWithHover(Vector2 mousePos);  // Draw with hover effect
    static void UpdateMenuButtonPosition(Button &start, Button &engine, Button &exit, int screenWidth, int screenHeight);
    static void UpdateThreeButtonPositions( Button & btn1, Button&btn2,Button& btn3, int screenWidth, int screenHeight, int startY);
};
