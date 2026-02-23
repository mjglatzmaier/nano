# nano Architecture (current state)

## Overview
`nano` is organized around a staged architecture where low-level deterministic primitives are in active use, and higher-level search/UCI layers are present but not yet fully wired into the active build.

## Active build graph
- **Executable**: `src/nano.cpp`
  - Initializes global precomputed tables and exits.
- **Static library (`libnano`)**
  - `src/bitboards.cpp`: precomputes board masks, attack masks, and helper lookup tables.
  - `src/magics.cpp`: loads rook/bishop magic lookup structures and attack tables.
  - `src/zobrist.cpp`: initializes zobrist random keys used for hashing.
- **Tests**
  - `tests/test_magics.cpp`: validates bishop/rook attacks with and without blockers and includes basic speed smoke tests.

## Data flow at startup
1. `main()` calls `Zobrist::load()`.
2. `main()` calls `Bitboards::load()`.
3. `main()` calls `Magics::load()`.
4. Process exits.

This explicit initialization order keeps all lookup tables deterministic and easy to validate in isolation.

## Present-but-not-yet-wired modules
- `uci.*`: UCI command parsing and protocol handling.
- `position.*`, `move.*`, `hashtable.*`, `threads.*`: gameplay/search support modules.

These files represent the intended full-engine direction but are intentionally not all compiled into the current target graph yet.

## Performance-focused engineering opportunities
1. **Reduce dynamic allocations in table generation**
   - Replace several `std::vector<std::vector<...>>` structures in `magics.cpp` and `bitboards.cpp` with fixed-size `std::array`-based storage where bounds are known.
2. **Tighten cache locality in magic indexing**
   - Flatten index tables and avoid nested vectors for `ridx`/`bidx` to reduce pointer chasing.
3. **Hoist static step tables**
   - Use `constexpr std::array` for move-step constants to avoid per-call temporary vector construction in hot helper paths.
4. **Target-scoped compile options**
   - Move compiler flags from global `CMAKE_CXX_FLAGS` to `target_compile_options(...)` for cleaner control and safer optimization tuning.
5. **Benchmark isolation**
   - Keep microbenchmarks separate from correctness tests so perf tracking can run independently and consistently.

## Near-term architecture goals
- Wire a thin UCI loop into the executable (without embedding search/eval logic in the protocol layer).
- Introduce a minimal legal move pipeline around `Position` and `Movegen`.
- Add deterministic unit tests for FEN parsing, move application/undo, and hash consistency.
