#include "NeuralEngine.hpp"
#include "../../lib/json.hpp"
#include "../core/Piece.hpp"

#include <iostream>
#include <chrono>
#include <thread>

extern float squareSize;
extern Vector2 boardPosition;

#ifdef _WIN32
namespace 
{
    std::string GetApplicationDirectoryy()
    {
        char executablePath[MAX_PATH] = {}; 
        DWORD length = GetModuleFileNameA(nullptr, executablePath, MAX_PATH); 

        if (length == 0 || length == MAX_PATH) 
        {
            return std::string(); 
        }

        std::string fullPath(executablePath, length);
        std::size_t separator = fullPath.find_last_of("\\/"); 
        return separator == std::string::npos ? std::string() : fullPath.substr(0, separator); 
    }
}
#endif

NeuralEngine::NeuralEngine(const std::string &host, int port)
    : client(host, port),
      serverHost(host),
      serverPort(port),
      serverReady(false)
#ifdef _WIN32
      ,
      hServerProcess(NULL)
#endif
{
    client.set_connection_timeout(3);
    client.set_read_timeout(5);
}

NeuralEngine::~NeuralEngine()
{
    shutdown();
}

bool NeuralEngine::init()
{
#ifdef _WIN32
    const std::string appDirectory = GetApplicationDirectoryy(); 
    const std::string serverDirectory = appDirectory + "\\engines\\neural-bot"; 
    const std::string serverExecutable = serverDirectory + "\\neural-bot.exe";

    DWORD attributes = GetFileAttributesA(serverExecutable.c_str()); 
    if( attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) 
    {
        std::cerr << "NeuralEngine: bundled neural bot was not found at: "
                    << serverExecutable << std::endl; 
        return false; 
    }

    std::string commandLine = "\"" + serverExecutable + "\" --host " + 
                                serverHost + " --port " + std::to_string(serverPort); 

    std::vector<char> commandBuffer(commandLine.begin(), commandLine.end()); 
    commandBuffer.push_back('\0'); 

    STARTUPINFOA si; 
    ZeroMemory(&si, sizeof(si)); 
    si.cb = sizeof(si); 
    si.dwFlags = STARTF_USESHOWWINDOW; 
    si.wShowWindow = SW_HIDE; 

    PROCESS_INFORMATION pi; 
    ZeroMemory(&pi, sizeof(pi)); 

    if(!CreateProcessA(serverExecutable.c_str(), commandBuffer.data(), nullptr, nullptr, 
            FALSE, CREATE_NO_WINDOW, nullptr, serverDirectory.c_str(), &si, &pi))
    {
        std::cerr<< "NeuralEngine: failed to start bundled neural bot. Windows error: " << GetLastError() << std::endl; 
        return false;
    }

    hServerProcess = pi.hProcess; 
    CloseHandle(pi.hThread); 
#endif

    return waitForServer(30000);
}

bool NeuralEngine::waitForServer(int timeoutMs)
{
    httplib::Client probe(serverHost, serverPort); 
    probe.set_connection_timeout(1); 
    probe.set_read_timeout(1); 

    auto start = std::chrono::steady_clock::now(); 
    
    #ifdef _WIN32
        if (hServerProcess != NULL && WaitForSingleObject(hServerProcess, 0) == WAIT_OBJECT_0)
        {
            DWORD exitCode = 0; 
            GetExitCodeProcess(hServerProcess, &exitCode); 
            std::cerr << "NeuralEngine: bundled neural bot exited during startup. Exit code: "
                        << exitCode << std::endl; 
            return false; 
        }
    #endif

    while (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count() < timeoutMs
    )
    {
        if (auto res = probe.Get("/health"))
        {
            if( res -> status == 200)
            {
                serverReady = true; 
                std::cout << "NeuralEngine: Python server ready." << std::endl; 
                return true; 
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
    }

    std::cerr <<"NeuralEngine: timed out waiting for the Python server." << std::endl; 
    return false; 
}

void NeuralEngine::newGame()
{

}

void NeuralEngine::setDifficulty(int level) 
{
    
}

EngineMove NeuralEngine::getMove(const std::vector<std::string> &moveHistory)
{
    if(!serverReady)    
        return EngineMove(); 

    nlohmann::json body; 
    body["moves"] = moveHistory; 

    auto res = client.Post("/get_move", body.dump(), "application/json"); 
    
    if(!res || res-> status!=200)
    {
        std::cerr << "NeuralEngine: /get_move request failed";
        if (res)
        {
            std::cerr << " (HTTP " << res->status << "): " << res->body;
        }
        std::cerr << std::endl;
        return EngineMove(); 
    }
    
    try
    {
        nlohmann::json reply = nlohmann::json::parse(res->body);
        if(reply.contains("move") && !reply["move"].is_null())
            return uciToEngineMove(reply["move"].get<std::string>());
    }
    catch(const std::exception& e)
    {
        std::cerr<<"There is no move"<<std::endl;
    }

    return EngineMove(); 
}

EngineMove NeuralEngine::uciToEngineMove(const std::string &uciStr) const 
{
    EngineMove em; 

    if (uciStr.size() < 4) return em; 

    int fromCol = uciStr[0] - 'a'; 
    int fromRow = '8' - uciStr[1]; 
    int toCol = uciStr[2] - 'a'; 
    int toRow = '8' - uciStr[3]; 

    em.from.x = boardPosition.x + fromCol * squareSize; 
    em.from.y = boardPosition.y + fromRow * squareSize; 
    em.to.x = boardPosition.x + toCol * squareSize; 
    em.to.y = boardPosition.y + toRow * squareSize; 

    if (uciStr.size() >= 5) 
    {
        switch (uciStr[4])
        {
            case 'q': em.promotionPiece = QUEEN; break;
            case 'r': em.promotionPiece = ROOK; break;
            case 'b': em.promotionPiece = BISHOP; break;
            case 'n': em.promotionPiece = KNIGHT; break;
            default: em.promotionPiece = 0; break;

        }
    }

    em.isValid = true; 
    return em; 
}

void NeuralEngine::reset()
{
    // it's stateless server
}

void NeuralEngine::shutdown()
{
    serverReady = false; 
    #ifdef _WIN32
        if (hServerProcess != NULL)
        {
            TerminateProcess(hServerProcess, 0);
            WaitForSingleObject(hServerProcess, 2000); 
            CloseHandle(hServerProcess); 
            hServerProcess = NULL; 
        }

        #endif
}
