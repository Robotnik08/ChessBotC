# Chess Engine Project

## Overview
This project is a chess engine written in C. It includes a core engine, evaluation functions, an opening book, and a DLL for integration. The project is organized for easy building and extension.

## Directory Structure
- `main.c` — Entry point for the engine.
- `src/` — Source files for engine logic, evaluation, search, and more.
- `include/` — Header files for the engine modules.
- `opening_book/` — Opening book data and conversion tools.
- `build/` — Compiled DLL and binaries.

## Building
To build the project, ensure you have a C compiler (e.g., GCC or MSVC) installed. Use the provided `Makefile`:

```sh
make
```

This will compile the engine and produce the necessary binaries and DLLs in the `build/` directory.

## Usage
- Run the engine executable after building:
  ```sh
  ./build/engine.exe engine
  ```
- To use the engine with a chess GUI, use the [ChessInterface](https://github.com/Robotnik08/ChessInterface) app I made.

## Opening Book
The `opening_book/` directory contains:
- `book.txt` — Raw opening book data.
- `convert_opening_book_to_compressed.to` — Tool for converting the book. (written in Dosato)

## Engine Features
- **Search Algorithm**: Implements a basic search algorithm with alpha-beta pruning.
- **Quiescence Search**: Enhances the search by evaluating quiet positions to avoid horizon effects.
- **Transposition Tables**: Uses transposition tables to store previously evaluated positions.
- **Move ordering**: Implements MVV-LVA move ordering to improve search efficiency.
- **Iterative Deepening**: Supports iterative deepening for better time management and better move ordering.
- **Opening book**: Uses a simple opening book to improve early game performance and introduce variety in openings.

## Evaluation Functions
- **Material Evaluation**: Evaluates positions based on material balance.
- **Positional Evaluation**: Evaluates positions based on piece placement and control of key squares.
- **Endgame Evaluation**: Special evaluation for endgame positions, considering king safety.
- **Passed Pawns**: Evaluates the strength of passed pawns in the position.