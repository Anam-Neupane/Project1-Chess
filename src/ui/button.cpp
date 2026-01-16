#include "button.hpp"

Button::Button(const char *imagePath, Vector2 imagePosition, float scale)
{
    Image image = LoadImage(imagePath);

    int originalWidth = image.width;
    int originalHeight = image.height;

    int newWidth = static_cast<int>(originalWidth * scale);
    int newHeight = static_cast<int>(originalHeight * scale);

    ImageResize(&image, newWidth, newHeight);
    texture = LoadTextureFromImage(image);
    UnloadImage(image);
    position = imagePosition;
}

Button::~Button()
{
    UnloadTexture(texture);
}

void Button::Draw()
{
    DrawTextureV(texture, position, WHITE);
}

bool Button::isPressed(Vector2 mousePos, bool mousePressed)
{
    Rectangle rect = {position.x, position.y, static_cast<float>(texture.width), static_cast<float>(texture.height)};

    if (CheckCollisionPointRec(mousePos, rect) && mousePressed)
    {
        return true;
    }

    return false;
}

void Button::SetPosition(Vector2 newPosition)
{
    position = newPosition;
}

Vector2 Button::GetSize()
{
    return {static_cast<float>(texture.width),
            static_cast<float>(texture.height)};
}

bool Button::isHovered(Vector2 mousePos)
{
    Rectangle rect = {position.x, position.y,
                      static_cast<float>(texture.width),
                      static_cast<float>(texture.height)};
    return CheckCollisionPointRec(mousePos, rect);
}

void Button::DrawWithHover(Vector2 mousePos)
{
    Color tint = isHovered(mousePos) ? LIGHTGRAY : WHITE;
    DrawTextureV(texture, position, tint);
}

void Button::UpdateMenuButtonPosition(Button &start, Button &engine, Button &exit, int screenWidth, int screenHeight)
{

    // Get Button sizes
    Vector2 startSize = start.GetSize();
    Vector2 engineSize = engine.GetSize();
    Vector2 exitSize = exit.GetSize();

    // Defining Spacing
    int spacing = 10;

    // Calculate total height of all buttons + spacing
    int totalHeight = startSize.y + engineSize.y + exitSize.y + (2 * spacing);

    // Calculate total starting Y (centered, with offset for title)
    int startY = (screenHeight - totalHeight) / 2 ;

    // Center each button horizontally and stack vertically
    start.SetPosition({(screenWidth - startSize.x) / 2.0f, (float)startY});

    engine.SetPosition({(screenWidth - engineSize.x) / 2.0f, startY + startSize.y + spacing});

    exit.SetPosition({(screenWidth - exitSize.x) / 2.0f, startY + startSize.y + engineSize.y + 2 * spacing});
}

void Button::UpdateThreeButtonPositions(Button& btn1, Button& btn2,Button& btn3, int screenWidth, int screenHeight, int startY)
{
    Vector2 size1 = btn1.GetSize();
    Vector2 size2 = btn2.GetSize();
    Vector2 size3 = btn3.GetSize();

    int spacing = 10;

    btn1.SetPosition({(screenWidth - size1.x) / 2.0f, static_cast<float>(startY)});
    btn2.SetPosition({(screenWidth - size2.x) / 2.0f, static_cast<float>(startY + size1.y + spacing)});
    btn3.SetPosition({(screenWidth - size3.x) / 2.0f, static_cast<float>(startY + size1.y + size2.y + spacing*2)});
}