#pragma once

#ifndef BITBOARDS_H_
#define BITBOARDS_H_

#include <cstdint>
#include "types.h"
#include "utils.h"

namespace Bitboards {

    constexpr int NUM_RANKS = 8;
    constexpr int NUM_FILES = 8;
    constexpr int NUM_SQUARES = 64;
    constexpr int PAWN_DIRS = 2;

    extern uint64 row_masks[NUM_RANKS];
    extern uint64 col_masks[NUM_FILES];
    extern uint64 pawn_masks[PAWN_DIRS];                           // 2nd - 6th rank mask for pawns
    extern uint64 pawn_mask_left[PAWN_DIRS];                       // Pawn captures (left)
    extern uint64 pawn_mask_right[PAWN_DIRS];                      // Pawn captures (right)
    extern uint64 pawn_attacks[PAWN_DIRS][NUM_SQUARES];            // Step attacks for pawns
    extern uint64 knight_masks[NUM_SQUARES];                       // Knight move masks
    extern uint64 king_masks[NUM_SQUARES];                         // King move masks
    extern uint64 king_checks[5][NUM_SQUARES];                     // King check masks
    extern uint64 king_flanks[8];                                  // 3-row squares (including king square) for pawn cover detection
    extern uint64 king_zone[NUM_SQUARES];                          // King's safety zone
    extern uint64 bishop_masks[NUM_SQUARES];                       // Bishop masks (trimmed outer board edges)
    extern uint64 rook_masks[NUM_SQUARES];                         // Rook masks (trimmed outer board edges)
    extern uint64 square_masks[NUM_SQUARES];                       // Single square bitboards
    extern uint64 bishop_attacks[NUM_SQUARES];                     // Precomputed bishop attack bitboards
    extern uint64 rook_attacks[NUM_SQUARES];                       // Precomputed rook attack bitboards
    extern uint64 king_pawn_storms[PAWN_DIRS][2];                  // Detect enemy pawn storms vs. king
    extern uint64 between_squares[NUM_SQUARES][NUM_SQUARES];       // Bitboards between aligned squares
    extern uint64 board_edges;                                     // Mask for board edges
    extern uint64 board_corners;                                   // Mask for board corners
    extern uint64 small_center_mask;                               // Small center mask
    extern uint64 big_center_mask;                                 // Big center mask
    extern uint64 pawn_majority_masks[3];                          // Pawn majority bitboards
    extern uint64 passed_pawn_masks[PAWN_DIRS][NUM_SQUARES];       // Passed pawn masks
    extern uint64 neighbor_columns[NUM_FILES];                     // Neighboring column bitboards
    extern uint64 colored_squares[2];                              // Bitboards for light/dark squares
    extern uint64 front_regions[PAWN_DIRS][NUM_SQUARES];           // Front-region bitboards for pawn structure
    extern unsigned reduction_table[2][2][NUM_SQUARES][NUM_SQUARES]; // Max search depth reduction table

    void load();

} // namespace bitboards

#endif // BITBOARDS_H_
