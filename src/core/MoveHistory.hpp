#pragma once

#include <string>
#include <vector>
#include <raylib.h>
#include "Piece.hpp"

struct MoveRecord
{
    int moveNumber = 0; // Displayed as "1.","2."etc. (increments after move)

    PieceType pieceType = NONE; // Which piece moved

    int pieceColor = -1; // 0 = black , 1 = white

    Vector2 from = {0, 0}; // Source square in pixel coordinates

    Vector2 to = {0, 0}; // Destination square in pixel coordinates

    std::string disambiguation; // SAN disambiguation, e.g. "b", "6", or "b6"

    bool isCapture = false; // Was an ememy piece taken?

    bool isCheck = false; // Did this move put the opponent in check?

    bool isCastleKing = false; // Kingside castle (O-O)

    bool isCastleQueen = false; // Queenside castle (O-O-O)

    bool isEnPassant = false; // En passant pawn capture

    PieceType promotedTo = NONE; // NONE unless a pawn promoted this move
};

class MoveHistory
{
private:
    std::vector<MoveRecord> moves;
    int scrollOffsetLines = 0;

    // Convert a pixel position to algebraic file+rank, e.g. {0, 55} -> "a8"
    std::string SquareToAlgebraic(const Vector2 &pos) const;

    // Piece-type letter: Knight => "N".....
    char PieceToLetter(PieceType type) const;

public:
    void AddMove(const MoveRecord &move);
    void Clear();

    // Convert a single MoveRecord into algebaric notation, e.g "Nxf3+" or "O-O"
    std::string GetAlgebraicNotation(const MoveRecord &move) const;

    // Build the full game string: "1. e4 e5 \n 2. Nf3 Nc6"
    std::string GetFullHistory() const;

    // Read-only access to the raw records (used when buliding MoveRecord in Board.cpp)
    const std::vector<MoveRecord> &GetMoves() const { return moves; }

    // Render the panel into the side panel area using Raylib DrawText
    void DrawPanel(float panelX, float panelY, float panelWidth, float panelHeight);
};