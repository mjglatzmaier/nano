#include "position.h"

Position::Position(std::istringstream &fen) {
    SetupFromFen(fen);
}

Position::Position(const Position &p) {
    *this = p;
}

Position& Position::operator=(const Position& p) {
    // std::copy(std::begin(p.history), std::end(p.history), std::begin(history));
    // stats = p.stats;
    // hidx = p.hidx;
    rootMoves_      = p.rootMoves_;
    info_           = p.info_;
    pieces_         = p.pieces_;
    threadId_       = p.threadId_;
    nodesSearched_  = p.nodesSearched_;
    qNodesSearched_ = p.qNodesSearched_;
    return *(this);
}

PieceData& PieceData::operator=(const PieceData& pd) {
    std::copy(std::begin(pd.piecesByColor), std::end(pd.piecesByColor), std::begin(piecesByColor));
    std::copy(std::begin(pd.kingSquare),    std::end(pd.kingSquare),    std::begin(kingSquare));
    std::copy(std::begin(pd.colorOn),       std::end(pd.colorOn),       std::begin(colorOn));
    std::copy(std::begin(pd.pieceOn),       std::end(pd.pieceOn),       std::begin(pieceOn));
    std::copy(std::begin(pd.numberOf),      std::end(pd.numberOf),      std::begin(numberOf));
    std::copy(std::begin(pd.pieceBitmaps),  std::end(pd.pieceBitmaps),  std::begin(pieceBitmaps));
    std::copy(std::begin(pd.pieceIndex),    std::end(pd.pieceIndex),    std::begin(pieceIndex));
    std::copy(std::begin(pd.squareOf),      std::end(pd.squareOf),      std::begin(squareOf));
    return (*this);
}

void Position::SetupFromFen(std::istringstream& fen) {
    Clear();

    std::string token;
    fen >> token;
    auto s = Square::A8;
}