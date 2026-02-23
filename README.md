# nano

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

A professional, minimal C++ chess engine project targeting UCI compliance and clear architecture.

## Features
- Bitboard-based move generation
- Precomputed magic-bitboard attack tables (rook/bishop)
- Zobrist hashing tables for position keys
- GoogleTest coverage for core attack-generation behavior

## Current Status
- The current build graph compiles core table-generation modules (`bitboards`, `magics`, `zobrist`).
- The executable currently initializes precomputed tables and exits.
- Additional modules (UCI/search/position/threading) exist in `src/` and are being staged into the active build incrementally.

See [`docs/architecture.md`](docs/architecture.md) for module boundaries and next engineering priorities.

## Build

### Prerequisites
- **C++ Compiler**: Requires C++20 (GCC, Clang, or MSVC)
- **CMake**: Version 3.10+ is recommended

### Build (Release)
```sh
git clone https://github.com/mjglatzmaier/chess.git
cd chess
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Test
```sh
cd build
./nano_tests
```

## Run
```sh
cd build
./nano
```

## UCI Options (planned/legacy surface)
```txt
setoption name threads value 4
setoption name hash value 1000
```
