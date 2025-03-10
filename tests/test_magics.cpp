#include <gtest/gtest.h>
#include "magics.h"
#include "bitboards.h"

class TestMagicBitboards : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        //Zobrist::load();
        Bitboards::load();
        Magics::load();
    }
    static void TearDownTestSuite() { }
};

// Test bishop attack generation
TEST_F(TestMagicBitboards, BishopAttacks) {
    uint64_t no_blockers        = 0ULL;
    uint64_t expected_attacks   = 9241705379636978241ULL;
    uint64_t actual_attacks     = Magics::attacks<Piece::BISHOP>(no_blockers, Square::D4);
    EXPECT_EQ(actual_attacks, expected_attacks)
        << "Bishop moves on d4 with no blockers are incorrect.";
}

// Test rook attack generation
TEST_F(TestMagicBitboards, RookAttacks) {
    uint64_t no_blockers        = 0ULL;
    uint64_t expected_attacks   = 578721386714368008ULL;
    uint64_t actual_attacks     = Magics::attacks<Piece::ROOK>(no_blockers, Square::D4);
    EXPECT_EQ(actual_attacks, expected_attacks)
        << "Rook moves on d4 with no blockers are incorrect.";
}

TEST_F(TestMagicBitboards, BishopBlocked) {
    uint64_t blockers = Bitboards::square_masks[Square::C3] | Bitboards::square_masks[Square::G7];
    uint64_t expected_attacks = 18333342782201920ULL;
    uint64_t actual_attacks = Magics::attacks<Piece::BISHOP>(blockers, Square::D4);
    EXPECT_EQ(actual_attacks, expected_attacks)
        << "Bishop moves with blockers are incorrect.";
}

// Test blocker-dependent attacks for rook
TEST_F(TestMagicBitboards, RookBlocked) {
    uint64_t blockers = Bitboards::square_masks[Square::D3] | Bitboards::square_masks[Square::E6];
    uint64_t expected_attacks = 578721386714365952ULL;
    uint64_t actual_attacks = Magics::attacks<Piece::ROOK>(blockers, Square::D4);
    EXPECT_EQ(actual_attacks, expected_attacks)
        << "Rook moves with blockers are incorrect.";
}

// Benchmarking move generation
TEST_F(TestMagicBitboards, BishopMoveSpeed) {
    uint64_t total_time         = 0;
    int iterations              = 1e6;
    uint64_t no_blockers        = 0ULL;

    clock_t start = clock();
    for (int i = 0; i < iterations; i++) {
        Magics::attacks<Piece::BISHOP>(no_blockers, Square::D4);
    }
    clock_t end = clock();
    total_time = (end - start) * 1000 / CLOCKS_PER_SEC;
    std::cout << "[Benchmark] Bishop MoveGen: " << total_time << " ms for " << iterations << " iterations\n";
}

TEST_F(TestMagicBitboards, RookMoveSpeed) {
    uint64_t total_time         = 0;
    int iterations              = 1e6;
    uint64_t no_blockers        = 0ULL;

    clock_t start = clock();
    for (int i = 0; i < iterations; i++) {
        Magics::attacks<Piece::ROOK>(no_blockers, Square::D4);
    }
    clock_t end = clock();
    total_time = (end - start) * 1000 / CLOCKS_PER_SEC;
    std::cout << "[Benchmark] Rook MoveGen: " << total_time << " ms for " << iterations << " iterations\n";
}