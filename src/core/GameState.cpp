#include "GameState.hpp"

GameState::GameState()
    : currentPlayer(1) // White starts
    , gameMode(GameMode::PVP_LOCAL)
    , phase(GamePhase::MAIN_MENU)
    , isBoardFlipped_var(false)
    , whiteScore(0)
    , blackScore(0)
    , whiteCapturedCount(0)
    , blackCapturedCount(0)

{}

void GameState::setGameMode(GameMode mode) {
    gameMode = mode;
    isBoardFlipped_var = false;
}

void GameState::setPhase(GamePhase newPhase) {
    phase = newPhase;
}

void GameState::switchPlayer() {
    currentPlayer = 1 - currentPlayer;
    if(gameMode == GameMode::PVP_LOCAL) {
        isBoardFlipped_var = !isBoardFlipped_var;
    }
}

void GameState::flipBoard() {
    isBoardFlipped_var = !isBoardFlipped_var;
}

void GameState::addCapture(int capturingColor, int pieceValue) {
    if(capturingColor == 1) { // White captures
        whiteScore += pieceValue;
        whiteCapturedCount++;
    } else{ // Black captures
        blackScore += pieceValue;
        blackCapturedCount++;
    }
}

void GameState::reset() {
    currentPlayer = 1;
    phase = GamePhase::PLAYING;
    isBoardFlipped_var = false;
    whiteScore = 0;
    blackScore = 0;
    whiteCapturedCount = 0;
    blackCapturedCount = 0;
}



