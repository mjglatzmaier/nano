#pragma once

#ifndef POSITION_H_
#define POSITION_H_

#include <thread>
#include <algorithm>
#include <vector>
#include <iostream>
#include <array>
#include <string>
#include <sstream>
#include <cstring>
#include <memory>

#include "types.h"
#include "utils.h"
#include "bitboards.h"
#include "magics.h"
#include "zobrist.h"
// #include "order.h"
// #include "parameter.h" // just for parameter reference (todo: refactor)
// #include "pawns.h"
// #include "material.h"

struct Move;

struct Info
{
    uint64 checkers;
    uint64 pinned[2];
    uint64 key;
    uint64 mkey;
    uint64 pawnkey;
    uint64 repkey;
    uint16 hmvs;
    uint16 cmask;
    uint8 move50;
    ColorType_t stm;
    SquareType_t eps;
    SquareType_t ks[2];
    bool has_castled[2];
    PieceType_t captured;
    bool incheck;
};

/// <summary>
/// Rootmove idea taken from Stockfish
/// Allows easy mgmt of pv/multi-pv/time mgmt
/// </summary>
struct Rootmove
{
    Rootmove(const Move &m) : pv(1, m) {}
    std::vector<Move> pv;
    int selDepth = 0;
    ScoreType_t score = Score::ninf;     // score for pv[0] == root
    ScoreType_t prevScore = Score::ninf; // last score for pv[0] == root
    bool operator==(const Move &m) const
    {
        return pv[0] == m;
    }
    bool operator<(const Rootmove &m) const
    {
        return m.score != score ? m.score < score : m.prevScore < prevScore;
    }
};
typedef std::vector<Rootmove> Rootmoves;

struct piece_data
{
    std::array<uint64, 2> bycolor;
    std::array<SquareType_t, 2> king_sq;
    std::array<ColorType_t, Square::TOTAL> color_on;
    std::array<SquareType_t, Square::TOTAL> piece_on;
    std::array<std::array<int, Piece::TOTAL>, 2> number_of;
    std::array<std::array<uint64, Square::TOTAL>, Color::TOTAL> bitmap;
    std::array<std::array<std::array<int, Square::TOTAL>, Piece::TOTAL>, 2> piece_idx;
    std::array<std::array<std::array<SquareType_t, 11>, Piece::TOTAL>, 2> square_of;

    piece_data() {};
    piece_data(const piece_data &pd);
    piece_data &operator=(const piece_data &pd);

    void clear();
    void set(const ColorType_t& c, const PieceType_t &p, const SquareType_t &s, Info &ifo);
    inline void do_quiet(const ColorType_t& c, const PieceType_t &p, const SquareType_t &f, const SquareType_t &t, Info &ifo);

    template <ColorType_t c>
    inline void do_castle(const bool &kingside);

    template <ColorType_t c>
    inline void undo_castle(const bool &kingside);

    inline void do_cap(const ColorType_t& c, const PieceType_t &p, const SquareType_t &f, const SquareType_t &t, Info &ifo);
    inline void do_ep(const ColorType_t& c, const SquareType_t &f, const SquareType_t &t, Info &ifo);
    inline void do_promotion(const ColorType_t& c, const PieceType_t &p,
                             const SquareType_t &f, const SquareType_t &t, Info &ifo);
    inline void do_promotion_cap(const ColorType_t& c,
                                 const PieceType_t &p, const SquareType_t &f, const SquareType_t &t, Info &ifo);
    inline void do_castle_ks(const ColorType_t& c, const SquareType_t &f, const SquareType_t &t, Info &ifo);
    inline void do_castle_qs(const ColorType_t& c, const SquareType_t &f, const SquareType_t &t, Info &ifo);
    inline void remove_piece(const ColorType_t& c, const PieceType_t &p, const SquareType_t &s, Info &ifo);
    inline void add_piece(const ColorType_t& c, const PieceType_t &p, const SquareType_t &s, Info &ifo);
};

class Position
{
    uint16 thread_id;
    Info history[1024];
    //haVoc::Movehistory stats;
    Info ifo;
    piece_data pcs;
    uint64 hidx;
    uint64 nodes_searched;
    uint64 qnodes_searched;

public:
    Position() {}
    Position(std::istringstream &s);
    Position(const std::string &fen);
    Position(const Position &p, const std::thread &t);
    Position(const Position &p);
    Position(const Position &&p);
    Position &operator=(const Position &p);
    Position &operator=(const Position &&);
    ~Position() {}

    double elapsed_ms;
    std::string bestmove;
    parameters params;
    bool debug_search = false;
    Rootmoves root_moves;

    void setup(std::istringstream &fen);
    std::string to_fen() const;
    void clear();
    void set_piece(const char &p, const SquareType_t &s);
    void print() const;
    void do_move(const Move &m);
    void undo_move(const Move &m);
    void do_null_move();
    void undo_null_move();
    int see_move(const Move &m) const;
    int see(const Move &m) const;

    inline void stats_update(const Move &m,
                             const Move &previous,
                             const int16 &depth,
                             const Score &score,
                             const std::vector<Move> &quiets,
                             Move *killers)
    {
        stats.update(*this, m, previous, depth, score, quiets, killers);
    }
    //const haVoc::Movehistory *history_stats() const { return &stats; }
    // Returns true if square 's' owned by 'us' is attacked by 'them'.
    bool is_attacked(const SquareType_t &s, const ColorType_t &us, const ColorType_t &them, uint64 m = 0ULL) const;
    uint64 attackers_of2(const SquareType_t &s, const ColorType_t& c) const;
    uint64 attackers_of(const SquareType_t &s, const uint64 &bb) const;
    uint64 checkers() const { return ifo.checkers; }
    bool in_check() const;
    bool in_dangerous_check();
    bool gives_check(const Move &m);
    bool quiet_gives_dangerous_check(const Move &m);
    bool is_legal(const Move &m);
    uint64 pinned(const ColorType_t& us);
    bool is_draw();
    inline SquareType_t eps() const { return ifo.eps; }
    inline ColorType_t to_move() const { return ifo.stm; }
    inline uint64 key() { return ifo.key; }
    inline uint64 repkey() { return ifo.repkey; }
    inline uint64 pawnkey() const { return ifo.pawnkey; }
    inline uint64 material_key() const { return ifo.mkey; }
    inline uint64 all_pieces() const { return pcs.bycolor[Color::WHITE] | pcs.bycolor[Color::BLACK]; }
    inline unsigned number_of(const ColorType_t& c, const PieceType_t &p) const { return pcs.number_of[c][p]; }
    inline PieceType_t piece_on(const SquareType_t &s) const { return pcs.piece_on[s]; }
    inline SquareType_t king_square(const ColorType_t c) const { return ifo.ks[c]; }
    inline SquareType_t king_square() const { return ifo.ks[ifo.stm]; }
    inline ColorType_t color_on(const SquareType_t &s) const { return pcs.color_on[s]; }
    inline uint16 id() { return thread_id; }
    inline void set_id(uint16 id) { thread_id = id; }
    inline void set_nodes_searched(uint64 n) { nodes_searched = n; }
    inline void set_qnodes_searched(uint64 qn) { qnodes_searched = qn; }
    bool is_cap_promotion(const MoveType_t &mt);
    bool is_promotion(const uint8 &mt);
    inline bool is_master() { return thread_id == 0; }
    inline uint64 nodes() const { return nodes_searched; }
    inline uint64 qnodes() const { return qnodes_searched; }
    inline void adjust_nodes(const uint64 &dn) { nodes_searched += dn; }
    inline void adjust_qnodes(const uint64 &dn) { qnodes_searched += dn; }

    inline bool can_castle_ks() const
    {
        return ((ifo.cmask & (ifo.stm == Color::WHITE ? wks : bks))) == (ifo.stm == Color::WHITE ? wks : bks);
    }

    inline bool can_castle_qs() const
    {
        return (((ifo.cmask & (ifo.stm == Color::WHITE ? wqs : bqs))) == (ifo.stm == Color::WHITE ? wqs : bqs));
    }

    template <ColorType_t c>
    inline bool can_castle_ks() const
    {
        return (((ifo.cmask & (c == Color::WHITE ? wks : bks))) == (c == Color::WHITE ? wks : bks));
    }

    template <ColorType_t c>
    inline bool can_castle_qs() const
    {
        return (((ifo.cmask & (c == Color::WHITE ? wqs : bqs))) == (c == Color::WHITE ? wqs : bqs));
    }

    template <ColorType_t c>
    inline bool can_castle() const
    {
        return can_castle_ks<c>() || can_castle_qs<c>();
    }

    template <ColorType_t c>
    inline bool has_castled() const { return ifo.has_castled[c]; }

    template <ColorType_t c>
    inline uint64 pinned() const { return ifo.pinned[c]; }

    inline bool pawns_near_promotion() const
    {
        return ((get_pieces<Color::WHITE, Piece::PAWN>() & Bitboards::row_masks[Row::R7]) != 0ULL) ||
               ((get_pieces<Color::BLACK, Piece::PAWN>() & Bitboards::row_masks[Row::R2]) != 0ULL);
    }

    inline bool pawns_on_7th() const
    {
        return ifo.stm == Color::WHITE ? 
            ((get_pieces<Color::WHITE, Piece::PAWN>() & Bitboards::row_masks[Row::R7]) != 0ULL) : 
            ((get_pieces<Color::BLACK, Piece::PAWN>() & Bitboards::row_masks[Row::R2]) != 0ULL);
    }

    template <ColorType_t c>
    inline bool non_pawn_material() const
    {
        return ((get_pieces<c, Piece::KNIGHT>() |
                 get_pieces<c, Piece::BISHOP>() |
                 get_pieces<c, Piece::ROOK>() |
                 get_pieces<c, Piece::QUEEN>()) != 0ULL);
    }

    template <ColorType_t c, PieceType_t p>
    inline uint64 get_pieces() const { return pcs.bitmap[c][p]; }

    template <ColorType_t c>
    inline uint64 get_pieces() const { return pcs.bycolor[c]; }

    template <ColorType_t c, PieceType_t p>
    inline SquareType_t *squares_of() const
    {
        return const_cast<SquareType_t *>(pcs.square_of[c][p].data() + 1);
    }
};

inline void piece_data::clear()
{
    std::fill(bycolor.begin(), bycolor.end(), 0);
    std::fill(king_sq.begin(), king_sq.end(), Square::no_square);
    std::fill(color_on.begin(), color_on.end(), Color::no_color);
    std::fill(piece_on.begin(), piece_on.end(), Piece::no_piece);

    for (auto &v : number_of)
        std::fill(v.begin(), v.end(), 0);
    for (auto &v : bitmap)
        std::fill(v.begin(), v.end(), 0ULL);
    for (auto &v : piece_idx)
    {
        for (auto &w : v)
        {
            std::fill(w.begin(), w.end(), 0);
        }
    }
    for (auto &v : square_of)
    {
        for (auto &w : v)
        {
            std::fill(w.begin(), w.end(), Square::no_square);
        }
    }
}

inline void piece_data::do_quiet(const ColorType_t& c, const PieceType_t &p,
                                 const SquareType_t &f, const SquareType_t &t, Info &ifo)
{
    auto fto = Bitboards::square_masks[f] | Bitboards::square_masks[t];
    int idx = piece_idx[c][p][f];
    piece_idx[c][p][f] = 0;
    piece_idx[c][p][t] = idx;

    bycolor[c] ^= fto;
    bitmap[c][p] ^= fto;

    square_of[c][p][idx] = t;
    color_on[f] = no_color;
    color_on[t] = c;

    piece_on[t] = p;
    piece_on[f] = no_piece;

    ifo.key = ifo.key ^ Zobrist::piece(f, c, p);
    ifo.key = ifo.key ^ Zobrist::piece(t, c, p);

    ifo.repkey = ifo.repkey ^ Zobrist::piece(f, c, p);
    ifo.repkey = ifo.repkey ^ Zobrist::piece(t, c, p);

    if (p == Piece::pawn)
    {
        ifo.pawnkey = ifo.pawnkey ^ Zobrist::piece(f, c, p);
        ifo.pawnkey = ifo.pawnkey ^ Zobrist::piece(t, c, p);
    }
}

inline void piece_data::do_cap(const ColorType_t& c, const PieceType_t &p,
                               const SquareType_t &f, const SquareType_t &t, Info &ifo)
{
    constexpr ColorType_t them = (c == Color::WHITE ? Color::BLACK);
    auto cap = piece_on[t];
    remove_piece(them, cap, t, ifo);
    do_quiet(c, p, f, t, ifo);
}

inline void piece_data::do_promotion(const ColorType_t& c, const PieceType_t &p,
                                     const SquareType_t &f, const SquareType_t &t, Info &ifo)
{
    remove_piece(c, Piece::PAWN, f, ifo);
    add_piece(c, p, t, ifo);
}

inline void piece_data::do_ep(const ColorType_t& c, const SquareType_t &f, const SquareType_t &t, Info &ifo)
{
    constexpr ColorType_t them = (c == Color::WHITE ? Color::BLACK);
    Square cs = Square(them == Color::WHITE ? t + 8 : t - 8);
    remove_piece(them, Piece::PAWN, cs, ifo);
    do_quiet(c, Piece::PAWN, f, t, ifo);
}

inline void piece_data::do_promotion_cap(const ColorType_t& c, const PieceType_t &p,
                                         const SquareType_t &f, const SquareType_t &t, Info &ifo)
{
    constexpr ColorType_t them = (c == Color::WHITE ? Color::BLACK);
    auto cap = piece_on[t];
    remove_piece(them, cap, t, ifo);
    remove_piece(c, Piece::PAWN, f, ifo);
    add_piece(c, p, t, ifo);
}

inline void piece_data::do_castle_ks(const ColorType_t& c, const SquareType_t &f, const SquareType_t &t, Info &ifo)
{
    SquareType_t rf = (c == Color::WHITE ? Squares::H1 : Squares::H8);
    SquareType_t rt = (c == Color::WHITE ? Squares::F1 : Squares::F8);
    do_quiet(c, king, f, t, ifo);
    do_quiet(c, rook, rf, rt, ifo);
}

inline void piece_data::do_castle_qs(const ColorType_t& c, const SquareType_t &f, const SquareType_t &t, Info &ifo)
{
    Square rf = (c == Color::WHITE ? Squares::A1 : Squares::A8);
    Square rt = (c == Color::WHITE ? Squares::D1 : Squares::D8);
    do_quiet(c, king, f, t, ifo);
    do_quiet(c, rook, rf, rt, ifo);
}

inline void piece_data::remove_piece(const ColorType_t& c, const PieceType_t &p, const SquareType_t &s, Info &ifo)
{
    auto sq = Bitboards::square_masks[s];
    bycolor[c] ^= sq;
    bitmap[c][p] ^= sq;

    // carefully remove this piece so when we add it back in undo, we
    // do not overwrite an existing piece index
    int tmp_idx = piece_idx[c][p][s];
    int max_idx = number_of[c][p];
    auto tmp_sq = square_of[c][p][max_idx];
    square_of[c][p][tmp_idx] = square_of[c][p][max_idx];
    square_of[c][p][max_idx] = no_square;
    piece_idx[c][p][tmp_sq] = tmp_idx;
    number_of[c][p] -= 1;
    piece_idx[c][p][s] = 0;
    color_on[s] = no_color;
    piece_on[s] = no_piece;
    ifo.key     ^= Zobrist::piece(s, c, p);
    ifo.mkey    ^= Zobrist::piece(s, c, p);
    ifo.repkey  ^= Zobrist::piece(s, c, p);
    if (p == Piece::pawn)
        ifo.pawnkey ^= Zobrist::piece(s, c, p);
}

inline void piece_data::add_piece(const ColorType_t& c, const PieceType_t &p, const SquareType_t &s, Info &ifo)
{
    auto sq = Bitboards::square_masks[s];
    bycolor[c] |= sq;
    bitmap[c][p] |= sq;

    number_of[c][p] += 1;
    square_of[c][p][number_of[c][p]] = s;
    piece_on[s] = p;
    piece_idx[c][p][s] = number_of[c][p];
    color_on[s] = c;
    ifo.key     ^= Zobrist::piece(s, c, p);
    ifo.mkey    ^= Zobrist::piece(s, c, p);
    ifo.repkey  ^= Zobrist::piece(s, c, p);
    if (p == Piece::pawn)
        ifo.pawnkey ^= Zobrist::piece(s, c, p);
}

inline void piece_data::set(const ColorType_t& c, const PieceType_t &p, const SquareType_t &s, Info &ifo)
{
    bitmap[c][p] |= Bitboards::square_masks[s];
    bycolor[c]   |= Bitboards::square_masks[s];
    color_on[s] = c;
    number_of[c][p] += 1;
    piece_idx[c][p][s] = number_of[c][p];
    square_of[c][p][number_of[c][p]] = s;
    piece_on[s] = p;
    if (p == Piece::king)
        king_sq[c] = s;

    ifo.key     ^= Zobrist::piece(s, c, p);
    ifo.mkey    ^= Zobrist::piece(s, c, p);
    ifo.repkey  ^= Zobrist::piece(s, c, p);
    if (p == Piece::pawn)
        ifo.pawnkey ^= Zobrist::piece(s, c, p);
}
#endif