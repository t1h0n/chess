#pragma once
#include <array>
#include <literals.hpp>
#include <memory>

#include "Pieces.hpp"

using ChessBoard = std::array<std::array<std::unique_ptr<Piece>, 8_sz>, 8_sz>;

class Board
{
public:
    /**
     * @brief add piece at position specified in piece
     * @return true if piece was added
     */
    bool add_piece(std::unique_ptr<Piece> piece);
    /**
     * @brief remove piece at position
     * @return true if piece was removed
     */
    bool remove_piece(const PiecePosition& position);

    bool square_is_empty(const PiecePosition& position) const;
    void clear_board();
    const ChessBoard& get_chess_board() const;

private:
    ChessBoard m_board;
};

inline const ChessBoard& Board::get_chess_board() const
{
    return m_board;
}