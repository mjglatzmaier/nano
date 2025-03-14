#pragma once

#ifndef POSITION_H_
#define POSITION_H_

#include <vector>
#include <array>
#include <string>

#include "types.h"
#include "utils.h"
#include "bitboards.h"
#include "magics.h"
#include "zobrist.h"

struct Move;

/// Stores position-related metadata used during evaluation and move generation.
struct PositionInfo {
    uint64 checkers = 0;
    uint64 pinned[2] = {0, 0};
    uint64 positionKey = 0;
    uint64 materialKey = 0;
    uint64 pawnKey = 0;
    uint64 repetitionKey = 0;
    uint16 halfmoveClock = 0;
    uint16 castlingRights = 0;
    uint8 move50 = 0;
    ColorType_t sideToMove;
    SquareType_t enPassantSquare;
    SquareType_t kingSquare[2];
    bool hasCastled[2] = {false, false};
    PieceType_t capturedPiece;
    bool isInCheck = false;
};

/// Stores principal variation moves for multi-PV and time management.
struct RootMove {
    explicit RootMove(const Move &m) : pv{m} {}

    std::vector<Move> pv;
    int searchDepth = 0;
    ScoreType_t score = Score::NINF;
    ScoreType_t previousScore = Score::NINF;

    bool operator==(const Move &m) const { return pv[0] == m; }
    bool operator<(const RootMove &m) const {
        return score != m.score ? score < m.score : previousScore < m.previousScore;
    }
};

using RootMoves = std::vector<RootMove>;

/// Stores piece-related data for board representation.
struct PieceData {
    uint64 piecesByColor[2] = {0, 0};
    SquareType_t kingSquare[2];
    ColorType_t colorOn[Square::TOTAL];
    SquareType_t pieceOn[Square::TOTAL];
    int numberOf[2][Piece::TOTAL] = {};
    uint64 pieceBitmaps[Color::TOTAL][Square::TOTAL] = {};
    int pieceIndex[2][Piece::TOTAL][Square::TOTAL] = {};
    SquareType_t squareOf[2][Piece::TOTAL][11] = {};

    void Clear();
    void SetPiece(ColorType_t color, PieceType_t piece, SquareType_t square, PositionInfo &info);
    void DoQuietMove(ColorType_t color, PieceType_t piece, SquareType_t from, SquareType_t to, PositionInfo &info);
    void DoCastleMove(ColorType_t color, bool kingside);
    void UndoCastleMove(ColorType_t color, bool kingside);
    void DoEPMove(ColorType_t c, SquareType_t f, SquareType_t t, PositionInfo &ifo);
    void DoPromotionMove(ColorType_t c, PieceType_t p, SquareType_t f, SquareType_t t, PositionInfo &ifo);
    void DoPromotionCaptureMove(ColorType_t c, PieceType_t p, SquareType_t f, SquareType_t t, PositionInfo &ifo);
    void DoCaptureMove(ColorType_t color, PieceType_t piece, SquareType_t from, SquareType_t to, PositionInfo &info);
    void RemovePiece(ColorType_t color, PieceType_t piece, SquareType_t square, PositionInfo &info);
    void AddPiece(ColorType_t color, PieceType_t piece, SquareType_t square, PositionInfo &info);
};

/// Represents the core chess position, tracking board state and legal moves.
class Position {
public:
    Position();
    explicit Position(std::istringstream& fen);
    Position(const Position &other);
    Position(Position &&other) noexcept;
    Position &operator=(const Position &other);
    Position &operator=(Position &&other) noexcept;
    ~Position() = default;

    void SetupFromFen(std::istringstream& fen);
    std::string ToFen() const;
    void Clear();
    void SetPiece(const char &p, const SquareType_t &s);
    void Print() const;
    void DoMove(const Move &m);
    void UndoMove(const Move &m);
    void DoNullMove();
    void UndoNullMove();
    bool IsAttacked(SquareType_t square, ColorType_t attacker, ColorType_t defender) const;
    bool IsInCheck() const;
    bool IsDraw() const;
    int SeeMove(const Move &m) const;
    int See(const Move &m) const;
    void UpdateStats(const Move &m, const Move &previous, const int16 &depth,
        const ScoreType_t &score, const std::vector<Move> &quiets, Move *killers);
    bool IsAttacked(const SquareType_t &s, const ColorType_t &us, const ColorType_t &them, uint64 m = 0ULL) const;
    uint64 AttackersOf(const SquareType_t &s, const ColorType_t& c) const;
    uint64 AttackersOf(const SquareType_t &s, const uint64 &bb) const;
    bool InCheck() const;
    bool IsDangerousCheck();
    bool GivesCheck(const Move &m);
    bool QuietGivesDangerousCheck(const Move &m);
    bool IsLegal(const Move &m);
    uint64 Pinned(const ColorType_t& us);
    bool IsDraw();
    bool IsCapPromotion(const MoveType_t &mt);
    bool IsPromotion(const uint8 &mt);

    /// Getters
    uint64 Checkers() const { return info_.checkers; }
    inline SquareType_t EnPassantSquare() const { return info_.enPassantSquare; }
    inline ColorType_t SideToMove() const { return info_.sideToMove; }
    inline uint64 Key() const { return info_.positionKey; }
    inline uint64 RepetitionKey() const { return info_.repetitionKey; }
    inline uint64 PawnKey() const { return info_.pawnKey; }
    inline uint64 MaterialKey() const { return info_.materialKey; }
    inline uint64 AllPieces() const { return pieces_.piecesByColor[Color::WHITE] | pieces_.piecesByColor[Color::BLACK]; }
    inline unsigned NumberOf(ColorType_t color, PieceType_t piece) const { return pieces_.pieceCount[color][piece]; }
    inline PieceType_t PieceOn(SquareType_t square) const { return pieces_.pieceOn[square]; }
    inline SquareType_t KingSquare(ColorType_t color) const { return info_.kingSquare[color]; }
    inline ColorType_t ColorOn(const SquareType_t &s) const { return pieces_.colorOn[s]; }
    inline uint16 Id() { return threadId_; }
    inline bool IsMaster() { return threadId_ == 0; }
    inline uint64 NodesSearched() const { return nodesSearched_; }
    inline uint64 QNodesSearched() const { return qNodesSearched_; }
 
    //  Setters
    inline void SetId(uint16 id) { threadId_ = id; }
    inline void SetNodesSearched(uint64 n) { nodesSearched_ = n; }
    inline void SetQNodesSearched(uint64 qn) { qNodesSearched_ = qn; }
    inline void AdjustNodes(const uint64 &dn) { nodesSearched_ += dn; }
    inline void AdjustQNodes(const uint64 &dn) { qNodesSearched_ += dn; }

    inline bool CanCastleKingside(ColorType_t color) const {
        return info_.castlingRights & (color == Color::WHITE ? kWhiteKingside : kBlackKingside);
    }

    inline bool CanCastleQueenside(ColorType_t color) const {
        return info_.castlingRights & (color == Color::WHITE ? kWhiteQueenside : kBlackQueenside);
    }

private:
    uint16 threadId_;
    PositionInfo info_;
    PieceData pieces_;
    uint64 nodesSearched_ = 0;
    uint64 qNodesSearched_ = 0;
    RootMoves rootMoves_;
};

#endif  // POSITION_H_
