#ifndef CHESS_ENGINE_HPP
#define CHESS_ENGINE_HPP

#include "EngineMove.hpp"
#include <string> 
#include <vector> 

// ChessEngine - abstract interface for any chess engine. 

class ChessEngine{

    public: 

        virtual ~ChessEngine() = default; // Virtual destructor -required for correct cleanup 

        // Start up the engine (e.g. spawn a process, load a model).
        // Returns true on success, false if the engine could not be started. 
        virtual bool init() = 0; 

        // Signal that a new game has started. 
        // For UCI engines this sends "ucinewgame".
        virtual void newGame() = 0;

        // Set difficulty. Meaning depends on the engine:
        // - Stockfish: skill level 1-20
        // - Random bot: unused
        virtual void setDifficulty(int level) = 0;

        // Ask the engine for its best move given the current position. 
        // moveHistory: the complete move list in UCI format, e.g. {"e2e4", "e7e5"}
        virtual EngineMove getMove(const std::vector<std::string> & moveHistory) = 0; 

        // Reset to the state after init() + newGame() - called on Restart. 
        virtual void reset() = 0; 

        // Clean shutdown - close processes, free handles, release resources. 
        virtual void shutdown() = 0; 

        // Engine name for UI display. 
        virtual std::string getName() const = 0; 

};

#endif