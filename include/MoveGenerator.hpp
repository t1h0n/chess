#pragma once

#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "Pieces.hpp"

class Board;

struct SpecialMovesData
{
    std::optional<Position> queen_side_rook;
    std::optional<Position> king_side_rook;
    std::optional<Position> en_passant_takable;

public:
    bool is_ok(const Board& board, PieceColor side_to_move, bool fisher_random = false) const;
};
using SquaresUnderAttack = std::unordered_set<Position>;
using NormalMoves = std::unordered_map<Position, std::unordered_set<Position>>;
struct AvailableMoves
{
    NormalMoves normal_moves;
    bool king_side_castle_possible;
    bool queen_side_castle_possible;

public:
    bool empty() const;
};

SquaresUnderAttack generate_squares_under_attack(Board& board, PieceColor side_to_move);

AvailableMoves generate_available_moves(Board& board,
                                        const SpecialMovesData& special_move_data,
                                        PieceColor side_to_move,
                                        bool fisher_random = false);