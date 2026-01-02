#ifndef MOVEUTILS_HPP
#define MOVEUTILS_HPP

#include <raylib.h>
#include <cmath>

namespace MoveUtils
{

    // Compare two vector2 with tolerance
    inline bool Vector2Equals(const Vector2 &a, const Vector2 &b, float tolerance = 0.01f)
    {
        return (std::abs(a.x - b.x) < tolerance) && (std::abs(a.y - b.y) < tolerance);
    }

    // Convert pixel position to board coordinates
    inline int PixelToBoard(float pixel, float offset, float size)
    {
        return static_cast<int>((pixel - offset) / size);
    }

    // Convert board coordinates to pixel position
    inline float BoardToPixel(int coord, float offset, float size)
    {
        return offset + coord * size;
    }

    // Check if position is within board bounds (0-7 inclusive)
    inline bool IsInBounds(int x, int y)
    {
        return x >= 0 && x <= 7 && y >= 0 && y <= 7;
    }
}

#endif