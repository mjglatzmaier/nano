#pragma once

#ifndef MAGICS_H_
#define MAGICS_H_

#include <memory>
#include <algorithm>
#include <random>

#include "types.h"
#include "bits.h"
#include "utils.h"

namespace Magics {

    template <PieceType_t p>
    uint64 gen_attacks(const uint64 &occ, const SquareType_t &s);

    template <PieceType_t p>
    uint64 attacks(const uint64 &occ, const SquareType_t &s);

    uint64 next_magic(const unsigned int &bits, Util::Rand<unsigned int> &r);

    bool load();
}

#endif
