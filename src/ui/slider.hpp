#pragma once

#include <raylib.h>

class Slider
{
private:
    Rectangle track;
    float handleRadius;
    int minValue;
    int maxValue;
    int value;
    bool dragging;

    float GetNormalizedValue() const
    {
        return static_cast<float>(value - minValue) / static_cast<float>(maxValue - minValue);
    }

    Vector2 GetHandleCenter() const
    {
        float handleX = track.x + GetNormalizedValue() * track.width;
        return {handleX, track.y + track.height * 0.5f};
    }

public:
    Slider(Vector2 position, float width, int minValue, int maxValue, int initialValue)
        : track{position.x, position.y, width, 10.0f},
          handleRadius(12.0f),
          minValue(minValue),
          maxValue(maxValue),
          value(initialValue),
          dragging(false)
    {
        SetValue(initialValue);
    }

    void SetPosition(Vector2 position)
    {
        track.x = position.x;
        track.y = position.y;
    }

    void SetValue(int newValue)
    {
        if (newValue < minValue)
            value = minValue;
        else if (newValue > maxValue)
            value = maxValue;
        else
            value = newValue;
    }

    int GetValue() const
    {
        return value;
    }

    bool HandleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased)
    {
        bool consumed = false;
        Rectangle hitArea = {track.x - handleRadius, track.y - handleRadius,
                             track.width + handleRadius * 2.0f,
                             track.height + handleRadius * 2.0f};

        if (mousePressed && CheckCollisionPointRec(mousePos, hitArea))
        {
            dragging = true;
            consumed = true;
        }

        if (dragging)
        {
            consumed = true;

            float clampedX = mousePos.x;
            if (clampedX < track.x)
                clampedX = track.x;
            if (clampedX > track.x + track.width)
                clampedX = track.x + track.width;

            float normalized = (clampedX - track.x) / track.width;
            SetValue(minValue + static_cast<int>(roundf(normalized * (maxValue - minValue))));
        }

        if (mouseReleased)
        {
            dragging = false;
        }

        return consumed;
    }

    void Draw(const char *label) const
    {
        Vector2 handleCenter = GetHandleCenter();
        float fillWidth = handleCenter.x - track.x;

        DrawText(label, static_cast<int>(track.x), static_cast<int>(track.y - 30), 22, RAYWHITE);
        DrawRectangleRec(track, Fade(LIGHTGRAY, 0.35f));
        DrawRectangle(static_cast<int>(track.x), static_cast<int>(track.y), static_cast<int>(fillWidth), static_cast<int>(track.height), MAROON);
        DrawCircleV(handleCenter, handleRadius, dragging ? BEIGE : RAYWHITE);
        DrawCircleLines(static_cast<int>(handleCenter.x), static_cast<int>(handleCenter.y), handleRadius, BROWN);

        const char *valueText = TextFormat("%d", value);
        int valueWidth = MeasureText(valueText, 20);
        DrawText(valueText,
                 static_cast<int>(track.x + track.width - valueWidth),
                 static_cast<int>(track.y - 30),
                 20,
                 RAYWHITE);
    }
};