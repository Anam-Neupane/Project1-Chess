#ifndef NEURAL_ENGINE_HPP
#define NEURAL_ENGINE_HPP

#include "ChessEngine.hpp"

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
#ifndef SW_HIDE
#define SW_HIDE 0
#endif
#endif

#include "../../lib/httplib.h"
#include <string> 
#include <vector> 

class NeuralEngine : public ChessEngine
{
    private: 
        httplib::Client client; 
        std::string serverHost; 
        int serverPort; 
        bool serverReady; 
    
    #ifdef _WIN32
        HANDLE hServerProcess; 
    #endif

        bool waitForServer(int timeoutMs); 
        EngineMove uciToEngineMove(const std::string &uciStr) const; 
    
    public: 
        NeuralEngine(const std::string &host = "127.0.0.1", int port = 5000); 
        ~NeuralEngine() override; 

        bool init() override; 
        void newGame() override; 
        void setDifficulty(int level) override; 
        EngineMove getMove(const std::vector<std::string> &moveHistory) override; 
        void reset() override; 
        void shutdown() override; 
        std::string getName() const override { return "NeuralBot"; }
};

#endif