#ifndef STOCKFISH_ENGINE_HPP
#define STOCKFISH_ENGINE_HPP

#include "ChessEngine.hpp"
#include "../core/Constants.hpp"
#include "core/Piece.hpp"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOGDI
#define NOGDI
#endif
#ifndef NOUSER
#define NOUSER
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <iostream>
#include <sstream>
#include <cmath>

// StockfishEngine - Communicates with a Stockfish child process via studio pipes.
// Inherits ChessEngine (abstract) and implements the UCI protocol. (This is done so that we can try to add new bot ourself)

class StockfishEngine : public ChessEngine
{
private:
    void *hProcess;          // Stockfish child process
    void *hChildStdinRead;   // Read-end of stdin pipe (child reads from this)
    void *hChildStdinWrite;  // Write-end of stdin pipe (parent writes to this)
    void *hChildStdoutRead;  // Read-end of stdout pipe (parent reads from this)
    void *hChildStdoutWrite; // Write-end of stdout pipe (child writes to this)

    int skillLevel; // Current Skill Level (1-20)
    int moveTimeMs; // Milliseconds to think per move

    bool sendCommand(const std::string &cmd);
    std::string readLine();                            // Blocking read of one output line
    std::string readUntil(const std::string &keyword); // Read Lines until a starts with keyword

    // Convert a UCI move string like "e2e4" into a EngineMove with pixel positions
    EngineMove uciToEngineMove(const std::string &uciStr) const;

public:
    StockfishEngine();
    ~StockfishEngine() override;

    bool init() override;
    void newGame() override;
    void setDifficulty(int level) override; // lvl of stock fish from 1 to 20
    EngineMove getMove(const std::vector<std::string> &moveHistory) override;
    void reset() override;
    void shutdown() override;
    std::string getName() const override { return "Stockfish"; }
};

#endif