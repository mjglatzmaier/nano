#ifndef TYPES_H_
#define TYPES_H_

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <memory>
#include <cstdint>
#include <type_traits>

using int16             = std::int16_t;
using int32             = std::int32_t;
using uint16            = std::uint16_t;
using uint32            = std::uint32_t;
using uint64            = std::uint64_t;
using uint8             = std::uint8_t;
using MoveType_t        = int;
using PieceType_t       = int;
using ColorType_t       = int;
using SquareType_t      = int;
using RowType_t         = int;
using ColType_t         = int;
using DepthType_t       = int;
using ScoreType_t       = int;
using NodeType_t        = int;
using EndgameType_t     = int;

struct MoveType {
    constexpr static int PROMOTE_Q         = 0;
    constexpr static int PROMOTE_R         = 1;
    constexpr static int PROMOTE_B         = 2;
    constexpr static int PROMOTE_N         = 3;
    constexpr static int CAPTURE_PROMOTE_Q = 4;
    constexpr static int CAPTURE_PROMOTE_R = 5;
    constexpr static int CAPTURE_PROMOTE_B = 6;
    constexpr static int CAPTURE_PROMOTE_N = 7;
    constexpr static int CASTLE_KS         = 8;
    constexpr static int CASTLE_QS         = 9;
    constexpr static int QUIET             = 10;
    constexpr static int QUIET_CHECK       = 11;
    constexpr static int CAPTURE           = 12;
    constexpr static int CAPTURE_CHECK     = 13;
    constexpr static int EN_PASSANT        = 14;
    constexpr static int CASTLES           = 15;
    constexpr static int PSEUDO_LEGAL      = 16;
    constexpr static int PROMOTION         = 17;
    constexpr static int CAPTURE_PROMOTION = 18;
    constexpr static int NONE              = -1;
    constexpr static int TOTAL             = 19;
};

struct Move {
    uint8 from        = 0;
    uint8 to          = 0;
    MoveType_t type   = MoveType::NONE;
  
    Move() = default;
    Move(const Move& other) = default;
  
    Move& operator=(const Move& other) = default;
    bool operator==(const Move& other) const { return (from == other.from && to == other.to && type == other.type); }
    bool operator!=(const Move& other) const { return !(*this == other); }
    void set(uint8 f, uint8 t, MoveType_t mt) {
      from    = f;
      to      = t;
      type    = mt;
    }
  };

struct Piece {
    constexpr static int PAWN       = 0;
    constexpr static int KNIGHT     = 1;
    constexpr static int BISHOP     = 2;
    constexpr static int ROOK       = 3;
    constexpr static int QUEEN      = 4;
    constexpr static int KING       = 5;
    constexpr static int NONE       = -1;
    constexpr static int TOTAL      = 6;
};

struct Color {
    constexpr static int WHITE      = 0;
    constexpr static int BLACK      = 1;
    constexpr static int NONE       = -1;
    constexpr static int TOTAL      = 2;
};

struct Square {
    constexpr static int A1 = 0,  B1 = 1,  C1 = 2,  D1 = 3,  E1 = 4,  F1 = 5,  G1 = 6,  H1 = 7;
    constexpr static int A2 = 8,  B2 = 9,  C2 = 10, D2 = 11, E2 = 12, F2 = 13, G2 = 14, H2 = 15;
    constexpr static int A3 = 16, B3 = 17, C3 = 18, D3 = 19, E3 = 20, F3 = 21, G3 = 22, H3 = 23;
    constexpr static int A4 = 24, B4 = 25, C4 = 26, D4 = 27, E4 = 28, F4 = 29, G4 = 30, H4 = 31;
    constexpr static int A5 = 32, B5 = 33, C5 = 34, D5 = 35, E5 = 36, F5 = 37, G5 = 38, H5 = 39;
    constexpr static int A6 = 40, B6 = 41, C6 = 42, D6 = 43, E6 = 44, F6 = 45, G6 = 46, H6 = 47;
    constexpr static int A7 = 48, B7 = 49, C7 = 50, D7 = 51, E7 = 52, F7 = 53, G7 = 54, H7 = 55;
    constexpr static int A8 = 56, B8 = 57, C8 = 58, D8 = 59, E8 = 60, F8 = 61, G8 = 62, H8 = 63;
    constexpr static int TOTAL = 64;
    constexpr static int NONE = -1;
};

struct Row {
    constexpr static int R1         = 0;
    constexpr static int R2         = 1;
    constexpr static int R3         = 2;
    constexpr static int R4         = 3;
    constexpr static int R5         = 4;
    constexpr static int R6         = 5;
    constexpr static int R7         = 6;
    constexpr static int R8         = 7;
    constexpr static int TOTAL      = 8;
    constexpr static int NONE       = -1;
};

struct Col {
    constexpr static int A          = 0;
    constexpr static int B          = 1;
    constexpr static int C          = 2;
    constexpr static int D          = 3;
    constexpr static int E          = 4;
    constexpr static int F          = 5;
    constexpr static int G          = 6;
    constexpr static int H          = 7;
    constexpr static int TOTAL      = 8;
    constexpr static int NONE       = -1;
};

enum class Depth { ZERO = 0, MAX_PLY = 64 };
enum class Score { INF = 10000, NINF = -10000, MATE = INF - 1, MATED = NINF + 1, MATE_MAX_PLY = MATE - 64, MATED_MAX_PLY = MATED + 64, DRAW = 0 };
enum class Nodetype { ROOT, PV, NON_PV, SEARCHING = 128 };
//enum class OrderPhase { hash_move, mate_killer1, mate_killer2, good_captures, killer1, killer2, bad_captures, quiets, end };

enum class EndgameType {
  NONE = -1,
  KP_K = 0,
  KN_K = 1,
  KB_K = 2,
  KNN_K = 17,
  KNB_K = 18,
  KBB_K = 34,
  UNKNOWN = 137
};

// Constants
constexpr std::array<const char*, 64> SanSquares = {
  "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
  "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
  "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
  "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
  "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
  "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
  "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
  "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};

#endif  // TYPES_H_