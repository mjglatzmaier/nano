
#ifndef BITS_H
#define BITS_H

#ifdef _MSC_VER
#ifdef _64BIT
#include <smmintrin.h>
#include <intrin.h>
#define builtin_popcount _mm_popcnt_uint64
#define builtin_lsb _BitScanForward64
#else
#include <intrin.h>
#define builtin_popcount __popcnt
#define builtin_lsb _BitScanForward
#endif
#else
#define builtin_popcount __builtin_popcountll
#endif

#include <iostream>

#include "types.h"
#include "bitboards.h"

namespace Bits
{

    inline void Print(const uint64 &b)
    {
        printf("+---+---+---+---+---+---+---+---+\n");
        for (RowType_t r = Row::R8; r >= Row::R1; --r)
        {
            for (ColType_t c = Col::A; c <= Col::H; ++c)
            {
                uint64 s = Bitboards::square_masks[8 * r + c];
                std::cout << (b & s ? "| X " : "|   ");
            }
            std::cout << "|" << std::endl;
            std::cout << "+---+---+---+---+---+---+---+---+" << std::endl;
        }
        std::cout << "  a   b   c   d   e   f   g   h" << std::endl;
    }

    inline int count(const uint64 &b) { return builtin_popcount(b); }

    inline int lsb(uint64 &b)
    {
#ifdef _MSC_VER
        unsigned long r = 0;
        builtin_lsb(&r, b);
        return r;
#else
        return __builtin_ffsll(b) - 1; // needs benchmarking
#endif
    }

    inline int pop_lsb(uint64 &b)
    {
        const int s = lsb(b);
        b &= (b - 1);
        return s;
    }

    inline bool more_than_one(const uint64 &b)
    {
        return b & (b - 1);
    }
}

#endif
