#include "MoveHistory.hpp"
#include "Constants.hpp"
#include <cmath>
#include <string>

// These are defined in Board.cpp and declared extern in Constants.hpp
extern float squareSize;
extern Vector2 boardPosition;

void MoveHistory::AddMove(const MoveRecord &move)
{
    moves.push_back(move);
}

void MoveHistory::Clear()
{
    moves.clear();
    scrollOffsetLines = 0;
}

std::string MoveHistory::SquareToAlgebraic(const Vector2 &pos) const
{

    // Convert to pixel to board indices
    int file = static_cast<int>(std::round((pos.x - boardPosition.x) / squareSize)); // 0 - 7 (a-h)
    int rank = 7 - static_cast<int>(std::round((pos.y - boardPosition.y) / squareSize));

    char fileChar = static_cast<char>('a' + file); // a b c d e f g h
    char rankChar = static_cast<char>('1' + rank); // 1 2 3 4 5 6 7 8

    return std::string(1, fileChar) + std::string(1, rankChar);
}

char MoveHistory::PieceToLetter(PieceType type) const
{
    switch (type)
    {
    case KING:
        return 'K';
    case QUEEN:
        return 'Q';
    case ROOK:
        return 'R';
    case BISHOP:
        return 'B';
    case KNIGHT:
        return 'N';
    default:
        return ' '; // For pawns
    }
}

std::string MoveHistory::GetAlgebraicNotation(const MoveRecord &move) const
{

    // special case first
    if (move.isCastleKing)
        return "O-O";
    if (move.isCastleQueen)
        return "O-O-O";

    std::string notation;
    const bool isCapture = move.isCapture || move.isEnPassant;

    // For move piece prefix

    if (move.pieceType != PAWN)
    {
        char letter = PieceToLetter(move.pieceType);
        if (letter != ' ')
        {
            notation += letter;
        }

        notation += move.disambiguation;
    }

    // Pawn captures include their exd5

    if (move.pieceType == PAWN && isCapture)
    {
        notation += SquareToAlgebraic(move.from)[0]; // Only the file letter
    }

    if (isCapture)
    {
        notation += "x";
    }

    notation += SquareToAlgebraic(move.to);

    // Pawn Promotion

    if (move.pieceType == PAWN && move.promotedTo != NONE && move.promotedTo != PAWN)
    {
        notation = "=";
        char promotionLetter = PieceToLetter(move.promotedTo);
        if (promotionLetter != ' ')
        {
            notation += promotionLetter;
        }
    }

    if (move.isCheck)
    {
        notation += "+";
    }

    // Can add for checkmate "#"

    return notation;
}

std::string MoveHistory::GetFullHistory() const
{
    std::string history;
    int displayMoveNum = 1;

    for (std::size_t i = 0; i < moves.size(); i++)
    {
        if (moves[i].pieceColor == 1) // White always starts a numbered entry
        {
            history += std::to_string(displayMoveNum) + ". ";
        }

        history += GetAlgebraicNotation(moves[i]);

        if (moves[i].pieceColor == 0) // After black move start new line
        {
            history += "\n";
            displayMoveNum++;
        }
        else
        {
            history += "  "; // Space Between White and Black on the same line
        }
    }

    return history;
}

// Side Panel renderer

void MoveHistory::DrawPanel(float panelX, float panelY, float panelWidth, float panelHeight)
{

    const int titleSize = 35;
    const int fontSize = 25;
    const int lineHeight = 34;
    const float contentTop = panelY + 54.0f;
    const float contentBottom = panelY + panelHeight - 16.0f;
    const int maxVisible = std::max(0, static_cast<int>((contentBottom - contentTop) / lineHeight));
    const float innerX = panelX + 12.0f;
    const float blackColumnX = panelX + 150.0f;
    const Rectangle panelBounds = {panelX, panelY, panelWidth, panelHeight};

    DrawRectangle(static_cast<int>(panelX),
                  static_cast<int>(panelY),
                  static_cast<int>(panelWidth),
                  static_cast<int>(panelHeight),
                  Fade(BEIGE, 0.92f));
    DrawRectangleLines(static_cast<int>(panelX),
                       static_cast<int>(panelY),
                       static_cast<int>(panelWidth),
                       static_cast<int>(panelHeight),
                       BLACK);

    // Panel title
    DrawText("Move History", static_cast<int>(innerX), static_cast<int>(panelY + 10), titleSize, BLACK);

    if (moves.empty())
    {
        DrawText("No moves yet.",
                 static_cast<int>(innerX),
                 static_cast<int>(panelY + 52),
                 fontSize, GRAY);

        return;
    }

    std::vector<std::pair<std::string, std::string>> lines;
    int moveNum = 1;
    std::string whiteMove;

    for (std::size_t i = 0; i < moves.size(); i++)
    {
        if (moves[i].pieceColor == 1) // White
        {
            whiteMove = std::to_string(moveNum) + ". " + GetAlgebraicNotation(moves[i]);
        }
        else // Black
        {
            lines.emplace_back(whiteMove, GetAlgebraicNotation(moves[i]));
            whiteMove.clear();
            moveNum++;
        }
    }

    // If White just moved and Black has not yet responded, show the partial line
    if (!whiteMove.empty())
    {
        lines.emplace_back(whiteMove, "");
    }

    const int maxScroll = std::max(0, static_cast<int>(lines.size()) - maxVisible);

    if (CheckCollisionPointRec(GetMousePosition(), panelBounds))
    {
        float wheelMove = GetMouseWheelMove();
        if (wheelMove != 0.0f)
        {
            scrollOffsetLines += static_cast<int>(wheelMove);
            if (scrollOffsetLines < 0)
                scrollOffsetLines = 0;
            if (scrollOffsetLines > maxScroll)
                scrollOffsetLines = maxScroll;
        }
    }

    if (scrollOffsetLines > maxScroll)
    {
        scrollOffsetLines = maxScroll;
    }

    // scrollOffsetLines = 0 shows the newest lines; higher values reveal older history.
    int startIdx = static_cast<int>(lines.size()) - maxVisible - scrollOffsetLines;
    if (startIdx < 0)
        startIdx = 0;

    int endIdx = startIdx + maxVisible;
    if (endIdx > static_cast<int>(lines.size()))
        endIdx = static_cast<int>(lines.size());

    int yOffset = static_cast<int>(contentTop - panelY);
    for (std::size_t i = static_cast<std::size_t>(startIdx); i < static_cast<std::size_t>(endIdx); ++i)
    {
        // Highlight the latest move line, whether it is a partial white move or a completed pair.
        Color textColor = (i == lines.size() - 1) ? YELLOW : WHITE;
        DrawText(lines[i].first.c_str(),
                 static_cast<int>(innerX),
                 static_cast<int>(panelY + yOffset),
                 fontSize, textColor);

        if (!lines[i].second.empty())
        {
            DrawText(lines[i].second.c_str(),
                     static_cast<int>(blackColumnX),
                     static_cast<int>(panelY + yOffset),
                     fontSize, textColor);
        }

        yOffset += lineHeight;
    }
}