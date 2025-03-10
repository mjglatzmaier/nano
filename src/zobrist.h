#pragma once

#ifndef ZOBRIST_H_
#define ZOBRIST_H_

#include "types.h"
#include "utils.h"

namespace Zobrist
{
    bool load();
    uint64 piece(SquareType_t square, ColorType_t color, PieceType_t piece);
    uint64 castle(ColorType_t color, uint16 bit);
    uint64 en_passant(uint8 column);
    uint64 side_to_move(ColorType_t color);
    uint64 move50(uint16 count);
    uint64 half_move_clock(uint16 count);

    // Random Zobrist key generator
    uint64 GenerateKey(unsigned int bits, Util::Rand<uint32> &rng);

    // Zobrist hash tables
    extern uint64 kPieceHashes[static_cast<int>(Square::TOTAL)][2][static_cast<int>(Piece::TOTAL)];
    extern uint64 kCastleHashes[2][16];
    extern uint64 kEnPassantHashes[8];
    extern uint64 kSideToMoveHashes[2];
    extern uint64 kMove50Hashes[512];
    extern uint64 kHalfMoveHashes[512];
}

#endif // ZOBRIST_H_
