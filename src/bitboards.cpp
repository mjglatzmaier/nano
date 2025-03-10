
#include "bitboards.h"
#include "bits.h"

namespace Bitboards
{

    // **Global Bitboard Data**
    uint64 row_masks[NUM_RANKS];
    uint64 col_masks[NUM_FILES];
    uint64 bishop_masks[NUM_SQUARES];                       // Bishop mask (trimmed outer board edges)
    uint64 pawn_masks[PAWN_DIRS];                           // 2nd - 6th rank mask for pawns
    uint64 pawn_attacks[PAWN_DIRS][NUM_SQUARES];            // Pawn step attacks
    uint64 pawn_mask_left[PAWN_DIRS];                       // Pawn captures (left)
    uint64 pawn_mask_right[PAWN_DIRS];                      // Pawn captures (right)
    uint64 knight_masks[NUM_SQUARES];                       // Knight move masks
    uint64 king_masks[NUM_SQUARES];                         // King move masks
    uint64 king_checks[5][NUM_SQUARES];                     // King check masks
    uint64 king_zone[NUM_SQUARES];                          // Large zone around king (for evaluation)
    uint64 king_flanks[8];                                  // 3-row squares (including king square) for pawn cover detection
    uint64 rook_masks[NUM_SQUARES];                         // Rook mask (trimmed outer board edges)
    uint64 king_pawn_storms[PAWN_DIRS][2];                  // Detect enemy pawn storms
    uint64 bishop_attacks[NUM_SQUARES];                     // Precomputed bishop attack bitboards
    uint64 rook_attacks[NUM_SQUARES];                       // Precomputed rook attack bitboards
    uint64 small_center_mask;                               // Small center region mask
    uint64 big_center_mask;                                 // Large center region mask
    uint64 square_masks[NUM_SQUARES];                       // Single square bitboards
    uint64 diagonal_masks[NUM_SQUARES];                     // Diagonal masks
    uint64 between_squares[NUM_SQUARES][NUM_SQUARES];       // Squares between aligned squares
    uint64 passed_pawn_masks[PAWN_DIRS][NUM_SQUARES];       // Passed pawn masks
    uint64 neighbor_columns[NUM_FILES];                     // Neighboring column bitboards
    uint64 colored_squares[2];                              // Bitboards for light/dark squares
    uint64 front_regions[PAWN_DIRS][NUM_SQUARES];           // Front-region bitboards for pawn structure
    uint64 pawn_majority_masks[3];                          // Pawn majority bitboards (1 for each flank)
    unsigned reduction_table[2][2][NUM_SQUARES][NUM_SQUARES]; // Max search depth reduction table
    uint64 board_edges;                                     // Mask for board edges
    uint64 board_corners;                                   // Mask for board corners

} // namespace bitboards

void Bitboards::load()
{
    std::vector<std::vector<int>> steps = {
        {},                                 // pawn
        {10, -6, -10, 6, 17, 15, -15, -17}, // knight
        {-7, -9, 7, 9},                     // bishop
        {-1, 1, 8, -8},                     // rook
        {},                                 // queen
        {-1, 1, 8, -8, -9, -7, 9, 7}        // king
    };

    // king zone steps
    std::vector<int> zsteps = {
        -1, 1, 8, -8, -9, -7, 9, 7, // normal kmask
        -2, 2, -2 + 8, -2 - 8, 2 + 8, 2 - 8, -2 - 16, -2 + 16, 2 - 16, 2 + 16,
        -16, 16, -16 - 1, -16 + 1, 16 + 1, 16 - 1};

    for (SquareType_t s = Square::A1; s <= Square::H8; ++s)
        square_masks[s] = (1ULL << s);

    // row/col masks
    for (RowType_t r = Row::R1; r <= Row::R8; ++r)
    {
        uint64 rw = 0ULL;
        uint64 cl = 0ULL;
        for (ColType_t c = Col::A; c <= Col::H; ++c)
        {
            cl |= square_masks[c * 8 + r];
            rw |= square_masks[r * 8 + c];
        }
        row_masks[r] = rw;
        col_masks[r] = cl;
    }

    // pawn majority masks
    pawn_majority_masks[0] = col_masks[Col::A] | col_masks[Col::B] | col_masks[Col::C];
    pawn_majority_masks[1] = col_masks[Col::D] | col_masks[Col::E];
    pawn_majority_masks[2] = col_masks[Col::F] | col_masks[Col::G] | col_masks[Col::H];

    // helpful definitions for board corners/edges
    board_edges = row_masks[Row::R1] | col_masks[Col::A] | row_masks[Row::R8] | col_masks[Col::H];
    board_corners = square_masks[Square::A1] | square_masks[Square::H1] | square_masks[Square::H8] | square_masks[Square::A8];

    // pawn masks for captures/promotions
    pawn_masks[Color::WHITE] = row_masks[Row::R2] | row_masks[Row::R3] | row_masks[Row::R4] | row_masks[Row::R5] | row_masks[Row::R6];
    pawn_masks[Color::BLACK] = row_masks[Row::R3] | row_masks[Row::R4] | row_masks[Row::R5] | row_masks[Row::R6] | row_masks[Row::R7];
    for (ColorType_t color = Color::WHITE; color <= Color::BLACK; ++color)
    {
        pawn_mask_left[color] = 0ULL;
        pawn_mask_right[color] = 0ULL;
        for (int r = (color == Color::WHITE ? 1 : 2); r <= (color == Color::WHITE ? 5 : 6); ++r)
        {
            for (int c = 0; c <= 6; ++c)
                pawn_mask_left[color] |= square_masks[r * 8 + c];
            for (int c = 1; c <= 7; ++c)
                pawn_mask_right[color] |= square_masks[r * 8 + c];
        }
    }

    // central control masks
    big_center_mask =   square_masks[Square::C3] | square_masks[Square::D3] | square_masks[Square::E3] | square_masks[Square::F3] |
                        square_masks[Square::C4] | square_masks[Square::D4] | square_masks[Square::E4] | square_masks[Square::F4] |
                        square_masks[Square::C5] | square_masks[Square::D5] | square_masks[Square::E5] | square_masks[Square::F5] |
                        square_masks[Square::C6] | square_masks[Square::D6] | square_masks[Square::E6] | square_masks[Square::F6];
    small_center_mask = square_masks[Square::C4] | square_masks[Square::C5] | square_masks[Square::D4] |
                        square_masks[Square::D5] | square_masks[Square::E4] | square_masks[Square::E5];

    // king flank masks
    uint64 roi = ~(row_masks[Row::R1] | row_masks[Row::R7]);
    for (ColType_t c = Col::A; c <= Col::H; ++c)
    {
        king_flanks[c] = 0ULL;
        int lidx = (c - 1 < 0 ? 0 : c - 1);
        int ridx = (c + 1 > Col::H ? Col::H : c + 1);
        uint64 mask = c <= Col::C || c >= Col::F ? 
            (col_masks[lidx] | col_masks[c] | col_masks[ridx]) : 
            (col_masks[lidx - 1] | col_masks[lidx] | col_masks[c] | col_masks[ridx] | col_masks[ridx + 1]);
        king_flanks[c] |= roi & mask;
    }

    for (SquareType_t s = Square::A1; s <= Square::H8; ++s)
    {
        if ((Util::row(s) % 2 == 0) && (s % 2 == 0))
            colored_squares[Color::BLACK] |= square_masks[s];
        else if ((Util::row(s) % 2) != 0 && (s % 2 != 0))
            colored_squares[Color::BLACK] |= square_masks[s];
        else
            colored_squares[Color::WHITE] |= square_masks[s];

        // Search reduction array
        // Index assignment [pv_node][improving][depth][move count]
        for (int sd = 0; sd < 64; ++sd) {
            for (int mc = 0; mc < 64; ++mc) {
                // pv nodes
                double small_r  = log(double(sd + 1)) * log(double(mc + 1)) / 2.0;
                double big_r    = 0.25 + log(double(sd + 1)) * log(double(mc + 1)) / 1.5;

                // pv-nodes
                reduction_table[1][0][sd][mc] = int(big_r >= 1.0 ? big_r + 0.5 : 0);
                reduction_table[1][1][sd][mc] = int(small_r >= 1.0 ? small_r + 0.5 : 0);

                // non-pv nodes
                reduction_table[0][0][sd][mc] = reduction_table[1][0][sd][mc] + 1;
                reduction_table[0][1][sd][mc] = reduction_table[1][1][sd][mc] + 1;
            }
        }

        // knight step attacks
        uint64 bm = 0ULL;
        for (auto &step : steps[Piece::KNIGHT])
        {
            int to = s + step;
            if (Util::on_board(to) &&
                Util::col_dist(s, to) <= 2 &&
                Util::row_dist(s, to) <= 2)
                bm |= square_masks[to];
        }
        knight_masks[s] = bm;

        // king step attacks
        bm = 0ULL;
        for (auto &step : steps[Piece::KING])
        {
            int to = s + step;
            if (Util::on_board(to) &&
                Util::col_dist(s, to) <= 1 &&
                Util::row_dist(s, to) <= 1)
                bm |= square_masks[to];
        }
        king_masks[s] = bm;

        // king zone bitboard (for eval)
        bm = 0ULL;
        for (auto &step : zsteps)
        {
            int to = s + step;
            if (Util::on_board(to) &&
                Util::col_dist(s, to) <= 2 &&
                Util::row_dist(s, to) <= 2)
                bm |= square_masks[to];
        }
        king_zone[s] = bm;

        // pawn attack masks for each color
        int pawn_steps[2][2] = {{9, 7}, {-7, -9}};
        for (ColorType_t c = Color::WHITE; c <= Color::BLACK; ++c)
        {
            pawn_attacks[c][s] = 0ULL;
            for (auto &step : pawn_steps[c])
            {
                int to = s + step;
                if (Util::on_board(to) &&
                    Util::row_dist(s, to) < 2 &&
                    Util::col_dist(s, to) < 2)
                {
                    pawn_attacks[c][s] |= square_masks[to];
                }
            }
        }

        // front region for each square
        for (ColorType_t c = Color::WHITE; c <= Color::BLACK; ++c)
        {
            front_regions[c][s] = 0ULL;
            if (c == Color::WHITE)
                for (auto r = Util::row(s) + 1; r <= Row::R8; ++r)
                    front_regions[c][s] |= row_masks[r];
            else
                for (auto r = Util::row(s) - 1; r >= Row::R1; --r)
                    front_regions[c][s] |= row_masks[r];
        }

        // between bitboard
        for (SquareType_t s2 = Square::A1; s2 <= Square::H8; ++s2)
        {
            if (s != s2)
            {
                uint64 btwn = 0ULL;
                int delta = 0;

                if (Util::col_dist(s, s2) == 0)
                    delta = (s < s2 ? 8 : -8);
                else if (Util::row_dist(s, s2) == 0)
                    delta = (s < s2 ? 1 : -1);
                else if (Util::on_diagonal(s, s2))
                {
                    if (s < s2 && Util::col(s) < Util::col(s2))
                        delta = 9;
                    else if (s < s2 && Util::col(s) > Util::col(s2))
                        delta = 7;
                    else if (s > s2 && Util::col(s) < Util::col(s2))
                        delta = -7;
                    else
                        delta = -9;
                }

                if (delta != 0)
                {
                    int iter = 0;
                    int sq = 0;
                    do
                    {
                        sq = s + iter * delta;
                        btwn |= square_masks[sq];
                        iter++;
                    } while (sq != s2);
                }
                between_squares[s][s2] = btwn;
            }
        }

        // passed pawn masks
        uint64 roi = ~(row_masks[0] | row_masks[7]);
        if (square_masks[s] & roi) {
            neighbor_columns[Util::col(s)] = 0ULL;
            for (ColorType_t c = Color::WHITE; c <= Color::BLACK; ++c) {
                passed_pawn_masks[c][s] = 0ULL;

                uint64 neighbors =
                    (king_masks[s] & row_masks[Util::row(s)]) | square_masks[s];

                while (neighbors) {
                    int sq = Bits::pop_lsb(neighbors);
                    passed_pawn_masks[c][s] |=
                        Util::squares_infront(col_masks[Util::col(sq)], c, sq);
                    if (s != sq)
                        neighbor_columns[Util::col(s)] |= col_masks[Util::col(sq)];
                }
            }
        }

        // bishop diagonal masks (outer bits trimmed)
        bm = 0ULL;
        uint64 trim = 0ULL;
        for (auto &step : steps[Piece::BISHOP])
        {
            int j = 0;
            while (true)
            {
                int to = s + (j++) * step;
                if (Util::on_board(to) && Util::on_diagonal(s, to))
                    bm |= square_masks[to];
                else
                    break;
            }
        }
        trim = square_masks[s] | (bm & board_edges);
        bishop_attacks[s] = bm;
        bm ^= trim;
        bishop_masks[s] = bm;

        // rook masks (outer-bits trimmed)
        bm = (row_masks[Util::row(s)] | col_masks[Util::col(s)]);
        rook_attacks[s] = bm;
        trim = square_masks[s] | square_masks[8 * Util::row(s)] | square_masks[8 * Util::row(s) + 7] |
                square_masks[Util::col(s)] | square_masks[Util::col(s) + 56];
        bm ^= trim;
        rook_masks[s] = bm;

        // king check mask
        king_checks[Piece::KNIGHT][s]    = knight_masks[s];
        king_checks[Piece::BISHOP][s]    = bishop_attacks[s];
        king_checks[Piece::ROOK][s]      = rook_attacks[s];
        king_checks[Piece::QUEEN][s]     = bishop_attacks[s] | rook_attacks[s];
    }

    // King pawn storm detection
    king_pawn_storms[Color::WHITE][0] = between_squares[Square::F2][Square::F5] |
                                        between_squares[Square::G2][Square::G5] |
                                        between_squares[Square::H2][Square::H5];
    king_pawn_storms[Color::WHITE][1] = between_squares[Square::A2][Square::A5] |
                                        between_squares[Square::B2][Square::B5] |
                                        between_squares[Square::C2][Square::C5];
    king_pawn_storms[Color::BLACK][0] = between_squares[Square::F7][Square::F4] |
                                        between_squares[Square::G7][Square::G4] |
                                        between_squares[Square::H7][Square::H4];
    king_pawn_storms[Color::BLACK][1] = between_squares[Square::A7][Square::A4] |
                                        between_squares[Square::B7][Square::B4] |
                                        between_squares[Square::C7][Square::C4];
}
