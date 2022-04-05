#pragma once
#include <array>
#include <literals.hpp>
#include <memory>

#include "Pieces.hpp"

class Board
{
    using PieceContainer = std::array<std::array<std::unique_ptr<Piece>, 8_sz>, 8_sz>;

public:
    Board clone() const;
    /**
     * @brief add piece at position specified in piece
     * @return true if piece was added
     */
    bool add_piece(std::unique_ptr<Piece> piece);
    /**
     * @brief remove piece at position
     * @return true if piece was removed
     */
    std::unique_ptr<Piece> remove_piece(const Position& position);

    bool is_square_empty(const Position& position) const;
    /**
     * @warning throws out of range error if position is invalid or empty
     */
    const Piece& get_piece_at_position(const Position& position) const;
    void clear_board();

    void apply_piece_visitor(PieceVisitor& visitor);

    static bool is_piece_position_valid(const Position& piece_position);

private:
    PieceContainer m_board;
};
