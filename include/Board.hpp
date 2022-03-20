#pragma once
#include <array>
#include <bitset>
#include <literals.hpp>
#include <memory>
#include <vector>

#include "Pieces.hpp"

class Board;
using ChessBoard = std::array<std::array<std::unique_ptr<Piece>, 8_sz>, 8_sz>;
using GuiChessBoard = std::vector<Piece const*>;

enum class MoveResult : std::uint32_t
{
    FAILURE = 0,
    SUCCESS = 1,
    PIECE_CAN_BE_PROMOTED = (1 << 2) + SUCCESS,
    MATCH_FINISHED = (1 << 3) + SUCCESS
};

enum class GameResult : std::uint32_t
{
    GAME_IN_PROGRESS,
    DRAW,
    WHITE_WON,
    BLACK_WON
};

inline bool is_move_successful(MoveResult result)
{
    return static_cast<std::uint32_t>(result) & 0x1U;
}

namespace internal
{
class MoveRepititionChecker
{
    struct PieceMove
    {
        PiecePosition from;
        PiecePosition to;
    };

public:
    bool is_threefold_repitition() const;
    void add_move(const PiecePosition& from, const PiecePosition& to);
    void undo_move();
    void reset();

private:
    std::vector<PieceMove> list_of_moves;
};

struct CastlingRights
{
    bool king_side_castle_possible = true;
    bool queen_side_castle_possible = true;
    Rook* king_side_rook = nullptr;
    Rook* queen_side_rook = nullptr;
};

struct GameStats
{
    CastlingRights white_rights;
    CastlingRights black_rights;
    Pawn* en_passant_takeble = nullptr;
    Pawn* piece_to_be_promoted = nullptr;
    std::uint32_t moves_without_take = 0U;
};

class MoveKeeper
{
    struct StatsToBeRecovered
    {
        GameStats game_stats;
        PiecePosition from;
        PiecePosition to;
        std::unique_ptr<Piece> captured_piece;
    };

public:
    MoveKeeper(ChessBoard& board, GameStats& stats)
        : m_board(board)
        , m_stats(stats)
    {
    }
    // first move is checked if it is possible to be done that it is added to move keeper and only
    // then executed
    void add_move(const PiecePosition& from, const PiecePosition& to);
    void undo_move();
    void reset();

private:
    ChessBoard& m_board;
    GameStats& m_stats;
};
}  // namespace internal

// creates and fills internal board structures
// can reset board and can get material difference
class BoardCreator;

class Board
{
public:
    Board(/*const BoardCreator& board_creator*/);

    // returns possible moves
    std::vector<PiecePosition> get_possible_moves(const PiecePosition& pos);
    std::vector<PiecePosition> get_king_attackers(PieceColor kings_color);
    // move piece
    MoveResult move_piece(const PiecePosition& start_pos, const PiecePosition& end_pos);
    void undo_move();
    GameResult get_game_result() const;

    void promote_piece(PromotablePieceType promote_into_piece);
    void reset_board();
    GuiChessBoard get_chess_board() const;

private:
    bool is_move_valid(const PiecePosition& from, const PiecePosition& to);
    // sets flags if needed adds move to move keeper and to moverepititioncheker, triggers
    // regeneration of possible moves
    MoveResult postprocess_move(const PiecePosition& from, const PiecePosition& to);
    // generates list of possible moves and fills possible moves table
    void generate_possible_moves();

private:
    // BoardCreator m_board_creator;
    ChessBoard m_board;
    internal::GameStats game_stats;

    PieceColor m_player_to_move = PieceColor::WHITE;

    std::array<std::bitset<8_sz>, 8_sz> squares_under_attack;  // created from possible moves
    std::unordered_map<PiecePosition, std::vector<PiecePosition>> m_possible_moves;
    internal::MoveKeeper m_move_keeper;
    internal::MoveRepititionChecker m_move_repitition_checker;
};