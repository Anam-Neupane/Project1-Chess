#ifndef STOCKFISH_ENGINE_HPP
#define STOCKFISH_ENGINE_HPP

#include "ChessEngine.hpp"
#include "../core/Constants.hpp"
#include "../core/Piece.hpp"
#include <iostream>
#include <sstream>
#include <cmath>

#ifdef _WIN32
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
#else
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#endif

class StockfishEngine : public ChessEngine
{
private:
#ifdef _WIN32
    HANDLE hProcess;
    HANDLE hChildStdinRead;
    HANDLE hChildStdinWrite;
    HANDLE hChildStdoutRead;
    HANDLE hChildStdoutWrite;
#else
    int hProcess;
    int hChildStdinRead;
    int hChildStdinWrite;
    int hChildStdoutRead;
    int hChildStdoutWrite;
#endif

    int skillLevel;
    int moveTimeMs;

    bool sendCommand(const std::string &cmd);
    std::string readLine();
    std::string readUntil(const std::string &keyword);
    EngineMove uciToEngineMove(const std::string &uciStr) const;

public:
    StockfishEngine();
    ~StockfishEngine() override;

    bool init() override;
    void newGame() override;
    void setDifficulty(int level) override;
    EngineMove getMove(const std::vector<std::string> &moveHistory) override;
    void reset() override;
    void shutdown() override;
    std::string getName() const override { return "Stockfish"; }
};

#endif
