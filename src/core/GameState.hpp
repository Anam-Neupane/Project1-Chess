#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP
#include <raylib.h>

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

    // Last move Tracking;
    Vector2 lastMoveFrom; //Source squre of last move
    Vector2 lastMoveTo; // Destination square of last move
    bool hasLastMove; // Flag to check if a move has been made

public:
    GameState();

    // Getters
    int getCurrentPlayer() const { return currentPlayer; }
    GameMode getGameMode() const { return gameMode; }
    GamePhase getPhase() const { return phase; }
    bool isBoardFlipped() const { return isBoardFlipped_var; }
    Vector2 getLastMoveFrom() const { return lastMoveFrom;}
    Vector2 getLastMoveTo() const { return lastMoveTo; }
    bool getHasLastMove() { return hasLastMove; }

    // Setters
    void setGameMode(GameMode mode);
    void setPhase(GamePhase newPhase);
    void setLastMove(Vector2 from, Vector2 to); //Setter for last move
    void switchPlayer();
    void flipBoard();

    // Score Management
    void addCapture(int capturingColor, int pieceValue);
    int getWhiteScore() const { return whiteScore; }
    int getBlackScore() const { return blackScore; }

    void reset();
};

#endif // GAMESTATE_HPP