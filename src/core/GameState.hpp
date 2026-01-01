#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

enum class GameMode
{
    PVP_LOCAL,
    VS_ENGINE
};

enum class GamePhase
{
    MAIN_MENU,
    PLAYING,
    PAUSED,
    CHECKMATE,
    STALEMATE,
    PROMOTION
};

class GameState
{

private:
    int currentPlayer; // 0 for black, 1 for white
    GameMode gameMode;
    GamePhase phase;
    bool isBoardFlipped_var;

    // Score Tracking
    int whiteScore;
    int blackScore;
    int whiteCapturedCount;
    int blackCapturedCount;

public:
    GameState();

    // Getters
    int getCurrentPlayer() const { return currentPlayer; }
    GameMode getGameMode() const { return gameMode; }
    GamePhase getPhase() const { return phase; }
    bool isBoardFlipped() const { return isBoardFlipped_var; }

    // Setters
    void setGameMode(GameMode mode);
    void setPhase(GamePhase newPhase);
    void switchPlayer();
    void flipBoard();

    // Score Management
    void addCapture(int capturingColor, int pieceValue);
    int getWhiteScore() const { return whiteScore; }
    int getBlackScore() const { return blackScore; }

    void reset();
};

#endif // GAMESTATE_HPP