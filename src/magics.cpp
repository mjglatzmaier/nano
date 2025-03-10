
#include "magics.h"
#include "magicsrands.h"
#include "types.h"

namespace Magics {
namespace Detail {

    struct Table
    {
        uint64 mask;
        uint64 magic;
        uint16 offset;
        uint8 shift;
        inline unsigned entry(const uint64 &occ_) { return unsigned(magic * (mask & occ_) >> shift); }
    };
        std::vector<std::vector<uint8>> ridx(64, std::vector<uint8>(4096));
        std::vector<std::vector<uint8>> bidx(64, std::vector<uint8>(512));
        std::vector<uint64> battks;
        std::vector<uint64> rattks;
        Table rtable[64];
        Table btable[64];
    } // end namespace detail
}

uint64 Magics::next_magic(const unsigned int &bits, Util::Rand<unsigned int> &r)
{
    uint64 res = 0ULL;
    for (unsigned int i = 0; i < bits; ++i)
        res |= (1ULL << (r.next() & 63));
    return res;
}

template <PieceType_t p>
uint64 Magics::gen_attacks(const uint64 &occ, const SquareType_t &s)
{
    std::vector<std::vector<int>> steps =
        {
            {},             // pawn
            {},             // knight
            {-7, 7, -9, 9}, // bishop
            {-1, 1, -8, 8}  // rook
        };
    uint64 bm = 0ULL;
    for (auto &step : steps[p])
    {
        uint64 tmp = 0ULL;
        int sqs = 1;

        while ((tmp & occ) == 0ULL && sqs < 8)
        {
            int to = s + (sqs++) * step;
            if (Util::on_board(to) &&
                Util::col_dist(s, to) <= 7 &&
                Util::row_dist(s, to) <= 7 &&
                ((p == Piece::BISHOP && Util::on_diagonal(s, to)) ||
                 (p == Piece::ROOK && (Util::same_row(s, to) || Util::same_col(s, to)))))
            {
                tmp |= (1ULL << to);
            }
        }
        bm |= tmp;
    }
    return bm;
}

bool Magics::load() {
    std::vector<uint64> occupancy, atks, used;
    occupancy.reserve(4096);
    atks.reserve(4096);
    used.reserve(4096);
    occupancy.resize(4096);
    atks.resize(4096);
    used.resize(4096);
    Util::Rand<unsigned int> r;

    // 4096 is computed from counting the number of possible blockers for a rook/bishop at a given square.
    // E.g. the rook@A1 has 12 squares which can be blocked (A8,H1 have been removed)
    // in mathematica : sum[12!/(n!*(12-n)!),{n,1,12}] = 4095 .. similar computation for bishop@E4.
    Detail::battks.reserve(1428);
    Detail::battks.resize(1428);
    Detail::rattks.reserve(4900);
    Detail::rattks.resize(4900);

    // structs to provide shorthand indexing
    struct _attks
    {
        std::vector<uint64> &operator[](const int &idx) { return (idx == 2 ? Detail::battks : Detail::rattks); }
    };
    struct _idx
    {
        std::vector<std::vector<uint8>> &operator[](const int &idx) { return (idx == 2 ? Detail::bidx : Detail::ridx); }
    };
    _attks attack_arr;
    _idx indices;

    // these offsets tally the number of unique attack arrays
    // that exist for bishop/rook at the given square. E.g. all values
    // for the rook can be computed by
    // value = empty_row1_sqs * empty_row2_sqs * empty_col1_sqs * empty_col2_sqs
    // rook@A1 has 49 unique attack patterns (but 4096 occupancies at A1)
    std::vector<std::vector<int>> offsets =
        {
            {}, // pawn
            {}, // knight
            { 
                7, 6, 10, 12, 12, 10, 6, 7,
                6, 6, 10, 12, 12, 10, 6, 6,
                10, 10, 40, 48, 48, 40, 10, 10,
                12, 12, 48, 108, 108, 48, 12, 12,
                12, 12, 48, 108, 108, 48, 12, 12,
                10, 10, 40, 48, 48, 40, 10, 10,
                6, 6, 10, 12, 12, 10, 6, 6,
                7, 6, 10, 12, 12, 10, 6, 7},
            {
                49, 42, 70, 84, 84, 70, 42, 49,
                42, 36, 60, 72, 72, 60, 36, 42,
                70, 60, 100, 120, 120, 100, 60, 70,
                84, 72, 120, 144, 144, 120, 72, 84,
                84, 72, 120, 144, 144, 120, 72, 84,
                70, 60, 100, 120, 120, 100, 60, 70,
                42, 36, 60, 72, 72, 60, 36, 42,
                49, 42, 70, 84, 84, 70, 42, 49
            }
        };

    for (PieceType_t p = Piece::BISHOP; p <= Piece::ROOK; ++p)
    {
        constexpr size_t bits = 6;
        for (SquareType_t s = Square::A1; s <= Square::H8; ++s)
        {

            uint64 mask = (p == Piece::ROOK ? Bitboards::rook_masks[s] : Bitboards::bishop_masks[s]);
            uint8 shift = 64 - Bits::count(mask);
            uint64 b = 0ULL;
            int occ_size = 0;
            uint64 magic = 0ULL;
            uint64 filter = 0ULL;

            // enumerate all occupancy combinations of the bishop/rook mask
            do
            {
                occupancy[occ_size] = b;
                atks[occ_size++] = (p == Piece::BISHOP ? gen_attacks<Piece::BISHOP>(b, s) : gen_attacks<Piece::ROOK>(b, s));
                b = (b - mask) & mask;
            } while (b);

            int count = 0;
            // do {

            //  do {
            //    magic = next_magic(bits, r);
            //    filter = (magic * mask) & 0xFFFF000000000000;
            //  } while (bits::count(filter) < 7);

            //  used.assign(occ_size, 0);

            //  for (count = 0; count < occ_size; ++count) {
            //    unsigned int idx = unsigned(magic * (mask & occupancy[count]) >> shift);

            //    if ((used[idx] == 0ULL &&
            //      (used[idx] != occupancy[count] ||
            //        occupancy[count] == 0ULL))) used[idx] = occupancy[count];
            //    else break;
            //  }
            //} while (count != occ_size);

            // remove redundant occupancies
            uint16 offset = 0;
            magic = (p == Piece::BISHOP ? bishop_magics[s] : rook_magics[s]);

            for (int i = 0; i < s; ++i)
                offset += offsets[p][i];

            uint64 stored[64][144] = {{}};

            // filter those occupancies which are redundant, the index : r_occ[s][idx] + offset
            for (int i = 0; i < occ_size; ++i)
            {
                unsigned int idx = unsigned(magic * (mask & occupancy[i]) >> shift);
                uint64 atk = atks[i];

                int k = 0;
                while (k < 144)
                {
                    uint64 prev = stored[s][k];
                    if (!prev && prev != atk)
                    {
                        stored[s][k] = atk;
                        break;
                    }
                    else if (prev == atk)
                        break;
                    ++k;
                }
                indices[p][s][idx] = k;

                int o = indices[p][s][idx] + offset; // total offset
                attack_arr[p][o] = atks[i];
                Detail::Table *tab = (p == Piece::BISHOP ? Detail::btable : Detail::rtable);
                tab[s].magic = magic;
                tab[s].mask = mask;
                tab[s].shift = shift;
                tab[s].offset = offset;
            }
            // std::cout << "s " << s <<  "occ size: " << occ_size << std::endl;
            // std::string eol = (s % 6 != 0 ? "), " : "),\n");
            // std::cout << "uint64(" << magic << eol;
            // if (s == 63) std::cout << "==============================" << std::endl;
        }
    }
    return true;
}

namespace Magics
{
    template <>
    uint64 attacks<Piece::ROOK>(const uint64 &occ, const SquareType_t &s)
    {
        using namespace Detail;
        return rattks[ridx[s][rtable[s].entry(occ)] + rtable[s].offset];
    }

    template <>
    uint64 attacks<Piece::BISHOP>(const uint64 &occ, const SquareType_t &s)
    {
        using namespace Detail;
        return battks[bidx[s][btable[s].entry(occ)] + btable[s].offset];
    }
}