# nano Task List (v0.1)

## Goal
Build a public, professional uci chess engine: 
 - performant using modern C++
 - incorporates a threading backend
 - extensible to nnue, mcts methods, gpu searching and other experiments
 - evaluation is modular, testable and exposes tunable parameters clearly and cleanly

## Conventions
- Write in modern C++17 or later using best practices
 - Follow Google C++ Style Guide with some exceptions for clarity and simplicity.
 - Use snake_case for functions and variables, PascalCase for types.
 - Use descriptive names for functions and variables.
  - Avoid abbreviations unless they are widely understood in the chess programming community (e.g. "uci", "fen", "nnue").
  - Avoid deep nesting and long functions - prefer small, composable functions.
  - Use comments to explain non-obvious design decisions or algorithms, but avoid redundant comments that simply restate the code.

## Phase 0: Repo hygiene and docs
 - 0.0 Understand current state of repository, cmake files, readme, and algorithms
 - 0.1 Create docs/architecture.md 
 - 0.2 Suggest any improvements to README.md 
 - 0.3 Examine current code and suggest improvements (modern C++ best practices) heavily weight those changes that improve runtime performance of the engine
 - 0.4 Add light comments and linting where appropriate (select 4-5 files most benefiting from more professional comments)
 - 0.5 Suggest concrete next steps to add to this TASKS.md for Phase 1

## Phase 1: Engine core integration
 - 1.0 Wire UCI loop behind a small `main` control path and keep protocol logic thin
 - 1.1 Bring `position` + `move` modules into active CMake targets with strict compile hygiene
 - 1.2 Add deterministic FEN parse/serialize tests (startpos + representative edge positions)
 - 1.3 Add make/undo move correctness tests (quiet, capture, promotion, castling, en-passant)
 - 1.4 Add Zobrist consistency tests for incremental updates vs recompute
 - 1.5 Introduce a minimal legal move generation API boundary suitable for search integration
 - 1.6 Add benchmark target(s) separate from unit tests for movegen and hash-table probes
