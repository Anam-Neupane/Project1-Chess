#include "StockfishEngine.hpp"


extern float squareSize;
extern Vector2 boardPosition;

StockfishEngine::StockfishEngine() : hProcess(INVALID_HANDLE_VALUE),
                                     hChildStdinRead(INVALID_HANDLE_VALUE),
                                     hChildStdinWrite(INVALID_HANDLE_VALUE),
                                     hChildStdoutRead(INVALID_HANDLE_VALUE),
                                     hChildStdoutWrite(INVALID_HANDLE_VALUE),
                                     skillLevel(10),
                                     moveTimeMs(300)
{
}

StockfishEngine::~StockfishEngine()
{
    shutdown();
}

// spawn stockfish

bool StockfishEngine::init()
{
    // Security_attributes with bInheritHandle=True lets the child process
    // inherit the pipe ends we designate as inherited.
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE; // pipe handles inheritable
    sa.lpSecurityDescriptor = NULL;

    // Create pipe; child's stdin (host writes -> child reads)
    if (!CreatePipe(&hChildStdinRead, &hChildStdinWrite, &sa, 0))
    {
        std::cerr << "StockfishEngine: CreatePipe (stdin) failed. Error:" << GetLastError() << std::endl;
        return false;
    }

    // Create pipe: child's stdout (child writes -> host reads)

    if (!CreatePipe(&hChildStdoutRead, &hChildStdoutWrite, &sa, 0))
    {
        std::cerr << "StockfishEngine: CreatePipe (stdout) failed. Error: " << GetLastError() << std::endl;
        CloseHandle(hChildStdinRead);
        CloseHandle(hChildStdinWrite);
        return false;
    }

    // The Parents sides of the pipes must not be inherited by the child.
    // If the child inherited the write-end of its own stdout pipe, it would
    // hold a copy open o- which would case Readfile on the parent side to
    // never return EOF, causing a deadlock.
    SetHandleInformation(hChildStdinWrite, HANDLE_FLAG_INHERIT, 0); // Parent writes here
    SetHandleInformation(hChildStdoutRead, HANDLE_FLAG_INHERIT, 0); // parent reads here

    // Startupinfo: tell windows to redirect stdin/stdout/stderr for the child
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput = hChildStdinRead;    // Child reads from this end
    si.hStdOutput = hChildStdoutWrite; // Merge stderr into stdout
    si.hStdError = hChildStdoutWrite;
    si.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    // Spawn stockfish.exe - it must be in the working directory or Path.
    // The second argument must be a non-const char* (Windows API requirement).
    char exeName[] = "stockfish.exe";
    if (!CreateProcess(
            NULL,    // Use command line to find executable
            exeName, // Command Line
            NULL,    // Defalut process security
            NULL,    // Defalut thread security
            TRUE,    // Inherit handles (critical for the pipe ends)
            0,       // No creation flags
            NULL,    // Inherit enviroment
            NULL,    // Inherit working directory
            &si,
            &pi))
    {
        std::cerr << "StockfishEngine: Failed to launch stockfish.exe. "
                  << "Is it in the project root directory? Error:"
                  << GetLastError() << std::endl;
        return false;
    }

    // Close the child-side ends in the parent process. 
    // The child has inherited copies - the parent should not hold them open. 

    CloseHandle(hChildStdoutWrite); hChildStdoutWrite = INVALID_HANDLE_VALUE;
    CloseHandle(hChildStdinRead); hChildStdinRead = INVALID_HANDLE_VALUE;

    hProcess = pi.hProcess; 
    CloseHandle(pi.hThread); 

    // Uci handshake
    sendCommand("uci\n"); 
    readUntil("uciok"); // Block until engine sends "uciok"

    sendCommand("isready\n"); 
    readUntil("readyok"); // Block until engine is fully loaded 

    std::cout << "StockfishEngine: Ready." << std::endl; 

    return true; 

}


bool StockfishEngine::sendCommand(const std::string& cmd) 
{
    DWORD written = 0; 
    BOOL ok = WriteFile(hChildStdinWrite, cmd.c_str(), static_cast<DWORD>(cmd.size()),
                            &written, NULL); // Synchronous (non-overlapped)
    
    
    return ok != FALSE;
}

std::string StockfishEngine::readLine()
{
    std::string line; 
    char c = '\0';
    DWORD bytesRead = 0; 

    while (true) 
    {
        if(!ReadFile(hChildStdoutRead, &c, 1, &bytesRead, NULL) || bytesRead == 0)
            break;      // Pipe closed or error
        if(c == '\n')
            break;      // End of Line
        if(c != '\r')   // Ignore Windows carriage returns
            line += c;  
    }

    return line; 
}

std::string StockfishEngine::readUntil(const std::string& keyword) {

    std::string line; 

    while (true)
    {
        line = readLine(); 
        if (line.substr(0, keyword.size()) == keyword) // Line starts with keyword
            break;
    }
    return line; 
}


void StockfishEngine::newGame()
{
    sendCommand("ucinewgame\n");
    sendCommand("isready\n");
    readUntil("readyok");
}

void StockfishEngine::setDifficulty(int level)
{
    if(level < 1) level = 1; 
    if(level > 20) level = 20; 
    skillLevel = level; 

    // Stockfish Skill Level: 0 (weak) to 20 (full strength)
    sendCommand("setoption name Skill Level value " + std::to_string(skillLevel) + "\n");
}

// getmove() : build position, ask for best move, parse response 
EngineMove StockfishEngine::getMove(const std::vector<std::string>& moveHistory)
{
    std::string posCmd = "position startpos";
    if (!moveHistory.empty())
    {
        posCmd += " moves";
        for(const auto& m : moveHistory)
            posCmd += " " + m; 
    }
    posCmd += "\n"; 
    sendCommand(posCmd); 

    // Ask engine to think for moveTimeMs milliseconds 
    sendCommand("go movetime " + std::to_string(moveTimeMs) + "\n"); 

    // Read Lines until we get one starting with "bestmove"
    std::string line = readUntil("bestmove");

    if(line.find("bestmove (none)") != std::string::npos || line.size() < 9) // "bestmove" = 9 chars minimum
    {
        return EngineMove(); // isValid = false
    }

    // Extract just the move string (4-5 chars), ignore "ponder ..." tail 
    std::string moveStr = line.substr(9); // Skip "bestmove "
    std::size_t spacePos = moveStr.find(' '); 
    if (spacePos != std::string::npos)
        moveStr = moveStr.substr(0, spacePos); 

    return uciToEngineMove(moveStr); 
}

//uciToEngineMove() -convert "e2e4" to pixel positions

EngineMove StockfishEngine::uciToEngineMove(const std::string& uciStr) const
{
    EngineMove em; 

    if (uciStr.size() < 4) return em; // Invalid string 

    // UCI file letters: 'a' = col 0, 'b' = col 1, 
    // UCI rank digits: '8' = row 0 (top), '1' = row 7 (bottom)
    int fromCol = uciStr[0] - 'a'; // 0-7
    int fromRow = '8' - uciStr[1]; // 0-7 (e.g. '2' -> row 6)

    int toCol = uciStr[2] - 'a'; 
    int toRow = '8' - uciStr[3]; 
    
    // Convert board coords -> pixel positions (same layout as piece positions in Board) 
    em.from.x = boardPosition.x + fromCol * squareSize;
    em.from.y = boardPosition.y + fromRow * squareSize; 
    em.to.x = boardPosition.x + toCol * squareSize; 
    em.to.y = boardPosition.y + toRow * squareSize; 

    // Optional promotion piece (5th character: 'q', 'r', 'b', 'n') 
    if(uciStr.size() >=5) 
    {
        switch (uciStr[4]) 
        {
            case'q':
                em.promotionPiece = QUEEN;
                break; 
            case 'r': em.promotionPiece = ROOK; break; 
            case 'b': em.promotionPiece = BISHOP; break; 
            case 'n': em.promotionPiece = KNIGHT; break; 
            default: em.promotionPiece = 0; break; 
        }
    }

    em.isValid = true; 
    return em; 
}

void StockfishEngine::reset()
{
    newGame(); // UCI "ucinewgame" resets internal state
}

void StockfishEngine::shutdown()
{
    if(hChildStdinWrite != INVALID_HANDLE_VALUE)
    {
        sendCommand("quit\n"); // Politely ask stockfish to exit
    }

    // Wait up to 2 seconds for Stockfish to exit gracefully 
    if (hProcess != INVALID_HANDLE_VALUE)
    {
        WaitForSingleObject(hProcess, 2000); 
        TerminateProcess(hProcess , 0); // Force - kill if it didn't exit
        CloseHandle(hProcess); 
        hProcess = INVALID_HANDLE_VALUE;
    }

    // Close all remaining pipe handles
    auto safeClose = [](HANDLE& h)
    {
        if (h != INVALID_HANDLE_VALUE)
        {
            CloseHandle(h); 
            h = INVALID_HANDLE_VALUE; 
        }
    }; 

    safeClose(hChildStdinRead);
    safeClose(hChildStdinWrite);
    safeClose(hChildStdoutRead); 
    safeClose(hChildStdoutWrite); 
}