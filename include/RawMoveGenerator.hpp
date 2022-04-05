#pragma once

#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "Board.hpp"
#include "Pieces.hpp"

struct SpecialMovesData
{
    Position king_position;
    bool king_moved{false};
    std::optional<Position> en_passant_takable;
    std::optional<Position> queen_side_rook;
    std::optional<Position> king_side_rook;
};

class MoveGenerator
{
public:
    using MovesContainer = std::unordered_map<Position, std::unordered_set<Position>>;

public:
    MoveGenerator(Board& board, const SpecialMovesData& special_move_data, PieceColor side_to_move);
    void generate_moves();
    const MovesContainer& get_available_moves() const;
    bool is_king_side_castle_possible() const;
    bool is_queen_side_castle_possible() const;

private:
    void generate_normal_moves();
    void generate_special_moves();
    bool is_board_valid_after_move(Board& new_board) const;

private:
    Board& m_board;
    PieceColor m_side_to_move;
    MovesContainer m_available_moves;
    const SpecialMovesData& m_special_move_data;
    bool m_king_side_castle_possible{false};
    bool m_queen_side_castle_possible{false};
};

class RawMoveGenerator : public PieceVisitor
{
public:
    using MovesContainer = std::unordered_map<Position, std::unordered_set<Position>>;

public:
    RawMoveGenerator(const Board& board,
                     const SpecialMovesData& special_move_data,
                     PieceColor side_to_move);
    const MovesContainer& get_available_moves() const;
    void visit(King& piece) override;
    void visit(Queen& piece) override;
    void visit(Bishop& piece) override;
    void visit(Knight& piece) override;
    void visit(Rook& piece) override;
    void visit(Pawn& piece) override;

private:
    template <typename T>
    void add_move_from_squares_under_attack(T& piece);

private:
    const Board& m_board;
    PieceColor m_side_to_move;
    MovesContainer m_available_moves;
    const SpecialMovesData& m_special_move_data;
};

class SquaresUnderAttackGenerator : public PieceVisitor
{
public:
    SquaresUnderAttackGenerator(const Board& board, PieceColor side_to_move);
    const std::unordered_set<Position>& get_squares_under_attack() const;
    void visit(King& piece) override;
    void visit(Queen& piece) override;
    void visit(Bishop& piece) override;
    void visit(Knight& piece) override;
    void visit(Rook& piece) override;
    void visit(Pawn& piece) override;

private:
    bool should_generate_move(PieceColor color) const;
    bool try_add_square(const Position& piece_position);

    void generate_diagonal_moves(const Position& piece_position);
    void generate_vertical_moves(const Position& piece_position);

private:
    const Board& m_board;
    PieceColor m_side_to_move;
    std::unordered_set<Position> m_squares_under_attack;
};

inline MoveGenerator::MoveGenerator(Board& board,
                                    const SpecialMovesData& special_move_data,
                                    PieceColor side_to_move)
    : m_board(board)
    , m_special_move_data(special_move_data)
    , m_side_to_move(side_to_move)
{
}

inline bool MoveGenerator::is_king_side_castle_possible() const
{
    return m_king_side_castle_possible;
}

inline bool MoveGenerator::is_queen_side_castle_possible() const
{
    return m_queen_side_castle_possible;
}

inline const MoveGenerator::MovesContainer& MoveGenerator::get_available_moves() const
{
    return m_available_moves;
}

inline RawMoveGenerator::RawMoveGenerator(const Board& board,
                                          const SpecialMovesData& special_move_data,
                                          PieceColor side_to_move)
    : m_board(board)
    , m_special_move_data(special_move_data)
    , m_side_to_move(side_to_move)
{
}

inline SquaresUnderAttackGenerator::SquaresUnderAttackGenerator(const Board& board,
                                                                PieceColor side_to_move)
    : m_board(board)
    , m_side_to_move(side_to_move)
{
}

inline const RawMoveGenerator::MovesContainer& RawMoveGenerator::get_available_moves() const
{
    return m_available_moves;
}

inline const std::unordered_set<Position>& SquaresUnderAttackGenerator::get_squares_under_attack()
    const
{
    return m_squares_under_attack;
}

template <typename T>
inline void RawMoveGenerator::add_move_from_squares_under_attack(T& piece)
{
    SquaresUnderAttackGenerator squares_under_attack_generator{m_board, m_side_to_move};
    squares_under_attack_generator.visit(piece);
    const auto& available_squares = squares_under_attack_generator.get_squares_under_attack();
    if (!available_squares.empty())
    {
        m_available_moves[ piece.get_position() ] = available_squares;
    }
}