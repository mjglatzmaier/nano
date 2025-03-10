#include "zobrist.h"
#include "bits.h"
#include "zobristrands.h"

namespace Zobrist
{
    uint64 piece_rands[Square::TOTAL][2][Piece::TOTAL];
    uint64 castle_rands[2][16];
    uint64 ep_rands[8];
    uint64 stm_rands[2];
    uint64 move50_rands[512];
    uint64 hmv_rands[512];
}

U64 Zobrist::GenerateKey(unsigned int bits, Util::Rand<uint32> &r)
{
    U64 res = 0ULL;
    for (unsigned int i = 0; i < bits; ++i)
        res |= (1ULL << (r.next() & 63));
    return res;
}

const unsigned _bits = 25;

bool Zobrist::load()
{

    const unsigned int N = 1835;
    std::vector<U64> rands;
    Util::Rand<uint32> R;

    // bool ok = true;
    // unsigned count = 0;
    // while (rands.size() < N) {

    //  U64 rand = R.next(); // gen(bits, R);
    //
    //  // filter zobrist keys to be unique
    //  // and favor smaller values (bits distributed near lower half)
    //  if (bits::count(rand) > 4) continue;
    //
    //  U64 filter_low = (rand >> 20); // 0x000000FFFFFFFF;
    //  if (bits::count(filter_low) >= 3) continue;

    //  //U64 filter_high = (rand << 20); // 0xFFFFFFFF;
    //  //if (bits::count(filter_high) <= 3) continue;

    //  for (unsigned i = 0; i < rands.size(); ++i) {
    //    ok = (rand != rands[i]);
    //    if (!ok) break;
    //  }
    //
    //  if (ok) {
    //    count = ((count + 1) % 6 == 0 ? 0 : count + 1);
    //    std::string eol((count != 0 ? "), " : "),\n"));
    //    std::cout << "ULL(" << rand << eol;
    //    rands.emplace_back(rand);
    //  }

    //}

    // pieces
    unsigned int idx = 0;
    for (SquareType_t sq = Square::A1; sq <= Square::H8; ++sq) {
        for (ColorType_t c = Color::WHITE; c <= Color::BLACK; ++c) {
            for (PieceType_t p = Piece::PAWN; p <= Piece::KING; ++p, ++idx) {
                piece_rands[sq][c][p] = zobrist_rands[idx];
            }
        }
    }

    // castle rights
    for (ColorType_t c = Color::WHITE; c <= Color::BLACK; ++c) {
        for (int bit = 0; bit < 16; ++bit, ++idx) {
            castle_rands[(int)c][bit] = zobrist_rands[idx];
        }
    }

    // ep
    for (ColType_t col = Col::A; col <= Col::H; ++col, ++idx) {
        ep_rands[col] = zobrist_rands[idx];
    }

    // stm
    stm_rands[Color::WHITE] = zobrist_rands[idx++];
    stm_rands[Color::BLACK] = zobrist_rands[idx++];

    // 512 here represents the maximum number of half moves expected in any
    // game of chess.
    for (int m = 0; m < 512; ++m, idx += 2)
    {
        move50_rands[m] = zobrist_rands[idx];
        hmv_rands[m] = zobrist_rands[idx + 1];
    }

    return true;
}

uint64 Zobrist::piece(SquareType_t s, ColorType_t c, PieceType_t p)
{
    return piece_rands[s][c][p];
}

uint64 Zobrist::castle(ColorType_t c, uint16 bit)
{
    return castle_rands[c][bit];
}

uint64 Zobrist::en_passant(uint8 column)
{
    return ep_rands[column];
}

uint64 Zobrist::side_to_move(ColorType_t c)
{
    return stm_rands[c];
}

uint64 Zobrist::move50(uint16 count)
{
    return move50_rands[count];
}

uint64 Zobrist::half_move_clock(uint16 count)
{
    return hmv_rands[count];
}
