# nano Development Instructions
## Dev models
- Default development model: GPT-5.3-Codex.
- Use Claude Opus for architectural review or adversarial critique.

## Purpose

nano is a minimal, professional UCI compliant chess engine written in C++ intended to be world class strength with minimal implementation complexity. It is a redesign from a more complex engine supporting parallel tree search. All code must prioritize clarity, determinism, and architectural discipline over abstraction or novelty.  Google tests are used for unit testing, and the codebase is designed to be easily testable with clear separation of concerns. The project is intended to be a clean, maintainable codebase that could be used as a reference implementation for chess engine development.

## Build and Environment Assumptions

- The C++ build environment is already created and activated.
- To build the project in Release mode, run
```sh
git clone https://github.com/mjglatzmaier/chess.git
cd chess
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
- Dependencies are outlined in the cmake configuration, including the tests
- Run the tests with
```sh
cd build
./nano_tests
```
- The codebase is designed to be portable and should not rely on platform-specific features unless explicitly justified.

When implementing tasks:
- Do NOT add environment setup steps.
- Do NOT modify pyproject.toml unless explicitly required.
- Always run tests before concluding a task.
- A task is not complete unless tests pass.

## Core Principles

 - Implementations should follow state of the art chess engine design patterns, but simplicity and clarity are paramount.
 - Do not add external frameworks or libraries unless justified.
 - Extensibility is important - for example the design should allow for future addition of NNUE evaluation or syzygy tablebases without major refactoring.
 
Preserve strict separation between:

 - UCI interface
 - Search algorithm
 - Evaluation function
 - Threading backend
 - Testing framework
 - Hooks for future extensions (e.g. NNUE, syzygy tablebases)
 - Hooks for different search algorithms (e.g. MCTS, proof-number search, GPU-based search)
 - Avoid implicit filesystem coupling.
 - Prefer explicit over magical abstractions.
 - Keep dependencies minimal.

## Code Style

 - Modern C++ (C++17 or later) with RAII and smart pointers where appropriate
 - Follow Google C++ Style Guide with some exceptions for clarity and simplicity.
 - Use snake_case for functions and variables, PascalCase for types.
 - Use descriptive names for functions and variables.
  - Avoid abbreviations unless they are widely understood in the chess programming community (e.g. "uci", "fen", "nnue").
  - Avoid deep nesting and long functions - prefer small, composable functions.
  - Use comments to explain non-obvious design decisions or algorithms, but avoid redundant comments that simply restate the code.
  - Avoid complex template metaprogramming or advanced C++ features that may reduce readability or maintainability.
  - Avoid macros except for include guards or simple constants.
  - Use constexpr and inline functions instead of macros where possible.
  - Global variables are generally discouraged, but if necessary, they must be clearly documented and encapsulated to avoid unintended side effects.
 - Avoid hidden side effects.
 - Keep functions small and composable.

## Architecture Rules

 - Strict separation of concerns between components.
 - UCI interface should be a thin layer that translates between UCI commands and internal engine state, without containing search or evaluation logic.
 - Search algorithm should be modular and not contain evaluation logic or UCI handling.
 - Evaluation function should be a separate module that can be easily swapped out or extended without affecting search or UCI logic.
 - Threading backend should be abstracted away from search and evaluation logic, allowing for different threading models (e.g. thread pool, async tasks) without major refactoring.
 - Testing framework should be completely separate from production code, with no dependencies on internal engine logic or state.
 - Evaluation should delienate between middle game and endgame evaluation, allowing for future addition of NNUE or syzygy tablebases without major refactoring, as well as interpolation between the two based on game phase.
 - Evaluation parameters should be clearly defined and documented, with a clear separation between material evaluation, positional evaluation, and endgame evaluation. Tunable parameters should be easily identifiable and modifiable without affecting core engine logic.
 
 ## Precommit Checklist
 Every change must:
- 1. Include unit tests.
- 2. Maintain or increase test coverage.
- 3. Avoid introducing unused dependencies.


## Dependency Rules

Before adding a dependency:

 - Justify its necessity.
 - Confirm it cannot be implemented cleanly in under ~200 lines.
 - Avoid framework lock-in.

## Commit Philosophy

 - Small, incremental commits.
 - One architectural change per commit.
 - No large, sweeping rewrites.

## Goal

nano should look like a clean chess engine built by a senior engineer, not a hobby project or framework experiment. It should exhibit clarity, simplicity, and maintainability.