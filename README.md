# Project1 - Chess

A desktop chess game built with C++ and raylib.

This project is an in-progress chess application with:

- Local player-vs-player mode
- Local player-vs-engine mode (Integrated with Stockfish)
- Move history and undo functionality
- Menu system, difficulty slider and game state handling
- UI assets and buttons for navigation

## Tech Stack

- C++14
- raylib 5.x
- MinGW-w64 toolchain (w64devkit)
- GNU Make (mingw32-make on Windows)

## Prerequisites

The current Makefile defaults are configured for a Windows setup with these paths:

- raylib source: `C:/raylib/raylib`
- compiler tools: `C:/raylib/w64devkit/bin`

Install or prepare:

1. raylib (compatible with 5.x)
2. MinGW-w64 / w64devkit (with `g++` and `mingw32-make`)
3. Git
4. Stockfish chess engine

## Downloading Stockfish

To play against the computer, you need the Stockfish engine executable (`stockfish.exe`):

1. Go to the [Stockfish download page](https://stockfishchess.org/download/).
2. Download the appropriate version for Windows (e.g., AVX2 or POPCNT depending on your CPU).
3. Extract the downloaded ZIP file.
4. Locate the `stockfish-windows-x86-64-*.exe` file (the name might vary slightly).
5. Rename the file to `stockfish.exe`.
6. Place `stockfish.exe` in the root directory of this project (alongside `Makefile` and `README.md`).

> **Note:** The `stockfish.exe` binary should not be committed or pushed to the repository because it's a large compiled binary and platform-specific. It is already ignored by the `.gitignore` file (`*.exe`).

## Setup From Scratch (Windows)

### 1) Clone the repository from GitHub

```powershell
git clone https://github.com/Anam-Neupane/Project1-Chess.git
cd Project1-Chess
```

### 2) Install raylib and w64devkit

This project expects the following paths:

- `C:/raylib/raylib`
- `C:/raylib/w64devkit/bin`

Steps:

1. Download raylib source and place it at `C:/raylib/raylib`.
2. Download `w64devkit` and place it at `C:/raylib/w64devkit`.
3. Verify the tools:

```powershell
C:/raylib/w64devkit/bin/g++.exe --version
C:/raylib/w64devkit/bin/mingw32-make.exe --version
```

### 3) Open the project in VS Code

```powershell
code .
```

Install these extensions if they are missing:

- C/C++ (`ms-vscode.cpptools`)
- Makefile Tools (`ms-vscode.makefile-tools`)

## Build and Run From Terminal (Windows)

Open PowerShell or Command Prompt in the project root, then run:

```powershell
C:/raylib/w64devkit/bin/mingw32-make.exe RAYLIB_PATH=C:/raylib/raylib PROJECT_NAME=game BUILD_MODE=DEBUG
```

Run the game:

```powershell
.\game.exe
```

Build a release binary:

```powershell
C:/raylib/w64devkit/bin/mingw32-make.exe RAYLIB_PATH=C:/raylib/raylib PROJECT_NAME=game BUILD_MODE=RELEASE
```

Clean build artifacts:

```powershell
C:/raylib/w64devkit/bin/mingw32-make.exe clean
```

## Run and Debug in VS Code (F5)

1. Open `src/main.cpp` in the editor.
2. Press `F5`.
3. Select the `Debug` configuration.
4. VS Code runs the pre-launch task (`build debug`) and starts the debugger.

Notes:

- In this workspace, the output name used by F5 depends on the active file name.
- If `src/main.cpp` is active, the generated executable is `main.exe`.
- For a fixed output name (`game.exe`), use the terminal build command from this README.

## Notes for Other Environments

- Linux/macOS can also use this Makefile, but you must install raylib and a compatible compiler toolchain first.
- You may need to override `RAYLIB_PATH`, compiler path, and platform-specific dependencies.

## Project Structure

```text
.
|-- Makefile
|-- README.md
|-- Documentation/
|-- lib/
|   |-- libgcc_s_dw2-1.dll
|   `-- libstdc++-6.dll
|-- resource/
|   |-- board0.png
|   |-- board1.png
|   |-- cut.png
|   |-- figure1.png
|   |-- howto.png
|   |-- mainmenu.png
|   |-- New.png
|   |-- Picture1.png
|   |-- Picture2.png
|   |-- Picture3.png
|   |-- restart.png
|   `-- Start.png
`-- src/
	|-- main.cpp
	|-- core/
	|   |-- Board.cpp
	|   |-- Board.hpp
	|   |-- Constants.hpp
	|   |-- GameState.cpp
	|   |-- GameState.hpp
	|   |-- MoveHistory.cpp
	|   |-- MoveHistory.hpp
	|   |-- Piece.hpp
	|   `-- moves/
	|       |-- cpp/
	|       |   |-- MoveGeneration.cpp
	|       |   |-- MoveSimulation.cpp
	|       |   |-- MoveValidator.cpp
	|       |   `-- PieceMovement.cpp
	|       `-- hpp/
	|           |-- MoveGeneration.hpp
	|           |-- MoveSimulation.hpp
	|           |-- MoveUtils.hpp
	|           |-- MoveValidator.hpp
	|           `-- PieceMovement.hpp
	|-- engine/
	|   |-- ChessEngine.hpp
	|   |-- EngineMove.hpp
	|   |-- StockfishEngine.cpp
	|   `-- StockfishEngine.hpp
	`-- ui/
		|-- button.cpp
		|-- button.hpp
		`-- slider.hpp
```

## Common Issues

- Undefined reference/linker errors: do not override `OBJS` with only top-level files (for example `OBJS=src/*.cpp`) because this excludes required source files in subfolders.
- `raylib.h` not found: verify the `RAYLIB_PATH` value and folder contents.
- Game cannot load textures: run the executable from the project root so relative paths like `resource/...` resolve correctly.

## Current Status

This is my first project and it will be horrible I know.
I Just want to Enjoy the process

Any Help Will Be Appreciated🦞.
THANK YOU.

In progress......