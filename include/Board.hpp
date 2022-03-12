#pragma once
#include <array>
#include <literals.hpp>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "Pieces.hpp"

using ChessBoard = std::array<std::array<std::unique_ptr<Piece>, 8_sz>, 8_sz>;

struct PieceWithPosition
{
    PiecePosition position;
    PieceType type;
    PieceColor color;
};

namespace internal
{
class MoveRepititionChecker
{
public:
    bool is_threefold_repitition() const;
    void add_move(const PiecePosition& position, PieceType type, PieceColor color);

private:
    std::vector<PieceWithPosition> list_of_moves;
};
struct SideStats
{
    bool king_side_castle_possible = true;
    bool queen_side_castle_possible = true;
    bool in_under_check = false;
    Piece* white_king = nullptr;
    Piece* black_king = nullptr;
    std::optional<PiecePosition> en_passant_takeble = std::nullopt;
    std::optional<PiecePosition> piece_to_be_promoted = std::nullopt;
    void reset();
};
}  // namespace internal

class Board
{
public:
    Board();
    Board(const std::vector<PieceWithPosition>& board);

    void move_piece(const PiecePosition& start_pos, const PiecePosition& end_pos);
    void promote_piece(PromotablePieceType promote_into_piece);
    void reset_board();

private:
    ChessBoard m_board;
    internal::SideStats m_white_stats;
    internal::SideStats m_black_stats;

    PieceColor m_player_to_move = PieceColor::WHITE;

    std::int32_t moves_without_take = 0;
    internal::MoveRepititionChecker m_move_repitition_checker;
};