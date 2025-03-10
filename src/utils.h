#pragma once

#ifndef UTILS_H_
#define UTILS_H_

#include <cstdlib>
#include <random>
#include <climits>
#include <memory>
#include <chrono>
#include <ctime>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

#include "types.h"

namespace Util {

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

inline std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

//=== Board Utility Functions =========================================

constexpr int row(int square) { return square >> 3; }
constexpr int col(int square) { return square & 7; }
constexpr int row_dist(int s1, int s2) { return abs(row(s1) - row(s2)); }
constexpr int col_dist(int s1, int s2) { return abs(col(s1) - col(s2)); }
constexpr bool on_board(int square) { return square >= 0 && square <= 63; }
constexpr bool same_row(int s1, int s2) { return row(s1) == row(s2); }
constexpr bool same_col(int s1, int s2) { return col(s1) == col(s2); }
constexpr bool on_diagonal(int s1, int s2) { return col_dist(s1, s2) == row_dist(s1, s2); }

constexpr bool aligned(int s1, int s2) {
    return on_diagonal(s1, s2) || same_row(s1, s2) || same_col(s1, s2);
}

constexpr bool aligned(int s1, int s2, int s3) {
    return (same_col(s1, s2) && same_col(s1, s3)) ||
           (same_row(s1, s2) && same_row(s1, s3)) ||
           (on_diagonal(s1, s3) && on_diagonal(s1, s2) && on_diagonal(s2, s3));
}

//=== Bitboard Functions =================================================

constexpr uint64 squares_infront(uint64 col_bb, ColorType_t color, int square) {
    return (color == Color::WHITE) ? col_bb << (8 * (row(square) + 1))
                                   : col_bb >> (8 * (8 - row(square)));
}

constexpr uint64 squares_behind(uint64 col_bb, ColorType_t color, int square) {
    return ~squares_infront(col_bb, color, square) & col_bb;
}

//=== Random Number Generator =============================================

template <typename T>
class Rand {
 public:
    Rand() : gen_(std::random_device{}()), dis_(0, UINT_MAX) {}

    explicit Rand(T min, T max) : gen_(std::random_device{}()), dis_(min, max) {}

    T next() { return dis_(gen_); }

 private:
    std::mt19937 gen_;
    std::uniform_int_distribution<T> dis_;
};

//=== Timer Class =========================================================

class Clock {
 public:
    Clock() = default;

    void start() { start_ = std::chrono::system_clock::now(); }
    void stop() { end_ = std::chrono::system_clock::now(); }

    double elapsed_ms() {
        stop();
        double ms_elapsed = ms();
        start();
        return ms_elapsed;
    }

    double ms() const {
        std::chrono::duration<double> duration = end_ - start_;
        return duration.count() * 1000.0;
    }

 private:
    std::chrono::time_point<std::chrono::system_clock> start_;
    std::chrono::time_point<std::chrono::system_clock> end_;
};

}  // namespace util

#endif  // UTILS_H_
