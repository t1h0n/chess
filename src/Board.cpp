#include <Board.hpp>
#include <iostream>
#include <literals.hpp>

namespace
{
struct MoveWithSideEffects
{
    PiecePosition position;
    std::optional<internal::SideStats> side_effects;
    MoveWithSideEffects(std::uint8_t x,
                        std::uint8_t y,
                        std::optional<internal::SideStats> side_effects = std::nullopt)
        : position(x, y)
        , side_effects(std::move(side_effects))
    {
    }
};

class PieceMoveDeterminer : public PieceVisitor  // needs current check board
{
public:
    PieceMoveDeterminer(std::vector<MoveWithSideEffects>& available_moves,
                        const PiecePosition& piece_position,
                        const ChessBoard& board,
                        const internal::SideStats& current_stats,
                        const internal::SideStats& other_stats)
        : m_available_moves(available_moves)
        , m_piece_position(piece_position)
        , m_board(board)
        , m_current_stats(current_stats)
        , m_other_stats(other_stats)
    {
    }
    virtual void visit(King& piece) override
    {
        auto move_side_effects = m_current_stats;
        move_side_effects.king_side_castle_possible = false;
        move_side_effects.queen_side_castle_possible = false;
        if (m_piece_position.x > 0_u8)
        {
            if (m_piece_position.y > 0_u8)
            {
                try_add_move(m_piece_position.x - 1_u8, m_piece_position.y - 1_u8, piece,
                             move_side_effects);
            }
            if (m_piece_position.y < 8_u8)
            {
                try_add_move(m_piece_position.x - 1_u8, m_piece_position.y + 1_u8, piece,
                             move_side_effects);
            }
            try_add_move(m_piece_position.x - 1_u8, m_piece_position.y, piece, move_side_effects);
        }
        if (m_piece_position.x < 8_u8)
        {
            if (m_piece_position.y > 0_u8)
            {
                try_add_move(m_piece_position.x + 1_u8, m_piece_position.y - 1_u8, piece,
                             move_side_effects);
            }
            if (m_piece_position.y < 8_u8)
            {
                try_add_move(m_piece_position.x + 1_u8, m_piece_position.y + 1_u8, piece,
                             move_side_effects);
            }
            try_add_move(m_piece_position.x + 1_u8, m_piece_position.y, piece, move_side_effects);
        }

        if (m_piece_position.y > 0_u8)
        {
            try_add_move(m_piece_position.x, m_piece_position.y - 1_u8, piece, move_side_effects);
        }
        if (m_piece_position.y < 8_u8)
        {
            try_add_move(m_piece_position.x, m_piece_position.y + 1_u8, piece, move_side_effects);
        }
        // castling needs
        if (!m_current_stats.in_under_check)
        {
            if (m_current_stats.king_side_castle_possible)
            {
                try_add_move(m_piece_position.x + 2_u8, m_piece_position.y, piece,
                             move_side_effects);
            }
            if (m_current_stats.king_side_castle_possible)
            {
                try_add_move(m_piece_position.x - 2_u8, m_piece_position.y, piece,
                             std::move(move_side_effects));
            }
        }
    }
    virtual void visit(Queen& piece) override
    {
    }
    virtual void visit(Bishop& piece) override
    {
    }
    virtual void visit(Knight& piece) override
    {
    }
    virtual void visit(Rook& piece) override
    {
    }
    virtual void visit(Pawn& piece) override
    {
        if (piece.get_color() == PieceColor::WHITE)
        {
            if (m_piece_position.y + 1_u8 == 7_u8)
            {
                auto move_side_effects = m_current_stats;
                move_side_effects.piece_to_be_promoted
                    = {m_piece_position.x, static_cast<std::uint8_t>(m_piece_position.y + 1_u8)};
                try_add_pawn_forward_move(m_piece_position.x, m_piece_position.y + 1_u8, piece,
                                          move_side_effects);
            }
            try_add_pawn_capture_move(m_piece_position.x + 1_u8, m_piece_position.y + 1_u8, piece);
            try_add_pawn_capture_move(m_piece_position.x - 1_u8, m_piece_position.y + 1_u8, piece);
        }
        else  // needs other stats to check if en_passant is possible
        {
            if (m_piece_position.y - 1_u8 == 0_u8)
            {
                auto move_side_effects = m_current_stats;
                move_side_effects.piece_to_be_promoted
                    = {m_piece_position.x, static_cast<std::uint8_t>(m_piece_position.y - 1_u8)};
                try_add_pawn_forward_move(m_piece_position.x, m_piece_position.y - 1_u8, piece,
                                          move_side_effects);
            }
            try_add_pawn_capture_move(m_piece_position.x + 1_u8, m_piece_position.y + 1_u8, piece);
            try_add_pawn_capture_move(m_piece_position.x - 1_u8, m_piece_position.y + 1_u8, piece);
        }
    }

private:
    bool try_add_move(std::uint8_t x,
                      std::uint8_t y,
                      Piece& current_piece,
                      std::optional<internal::SideStats> side_effects = std::nullopt)
    {
        if (!m_board[ y ][ x ] || m_board[ y ][ x ]->get_color() != current_piece.get_color())
        {
            m_available_moves.emplace_back(x, y, side_effects);
            return true;
        }
        return false;
    }
    void try_add_pawn_forward_move(std::uint8_t x,
                                   std::uint8_t y,
                                   Piece& current_piece,
                                   std::optional<internal::SideStats> side_effects = std::nullopt)
    {
        if (!m_board[ y ][ x ])
        {
            m_available_moves.emplace_back(x, y, side_effects);
        }
    }

    void try_add_pawn_capture_move(std::uint8_t x,
                                   std::uint8_t y,
                                   Piece& current_piece,
                                   std::optional<internal::SideStats> side_effects = std::nullopt)
    {
        if (m_board[ y ][ x ] && m_board[ y ][ x ]->get_color() != current_piece.get_color())
        {
            m_available_moves.emplace_back(x, y, side_effects);
        }
    }

private:
    std::vector<MoveWithSideEffects>& m_available_moves;
    const PiecePosition& m_piece_position;
    const ChessBoard& m_board;
    const internal::SideStats& m_current_stats;
    const internal::SideStats& m_other_stats;
};
}  // namespace

class BoardPrinter : public PieceVisitor
{
    virtual void visit(King& piece)
    {
        std::cout << "K" << to_c_str(piece.get_color());
    }
    virtual void visit(Queen& piece)
    {
        std::cout << "Q" << to_c_str(piece.get_color());
    }
    virtual void visit(Bishop& piece)
    {
        std::cout << "B" << to_c_str(piece.get_color());
    }
    virtual void visit(Knight& piece)
    {
        std::cout << "K" << to_c_str(piece.get_color());
    }
    virtual void visit(Rook& piece)
    {
        std::cout << "R" << to_c_str(piece.get_color());
    }
    virtual void visit(Pawn& piece)
    {
        std::cout << "P" << to_c_str(piece.get_color());
    }
};

Board::Board()
{
    reset_board();
}

void Board::reset_board()
{
    for (auto& row : m_board)
    {
        for (auto& piece : row)
        {
            piece.reset();
        }
    }
    m_white_stats.reset();
    m_black_stats.reset();
    m_player_to_move = PieceColor::WHITE;
    moves_without_take = 0;
    // Pawns
    for (auto i = 0_sz; i < 8_sz; ++i)
    {
        m_board[ 1_sz ][ i ] = std::make_unique<Pawn>(PieceColor::WHITE);
        m_board[ 6_sz ][ i ] = std::make_unique<Pawn>(PieceColor::BLACK);
    }
    // Rooks
    m_board[ 0_sz ][ 0_sz ] = std::make_unique<Rook>(PieceColor::WHITE);
    m_board[ 0_sz ][ 7_sz ] = std::make_unique<Rook>(PieceColor::WHITE);
    m_board[ 7_sz ][ 0_sz ] = std::make_unique<Rook>(PieceColor::BLACK);
    m_board[ 7_sz ][ 7_sz ] = std::make_unique<Rook>(PieceColor::BLACK);
    // Knight
    m_board[ 0_sz ][ 1_sz ] = std::make_unique<Knight>(PieceColor::WHITE);
    m_board[ 0_sz ][ 6_sz ] = std::make_unique<Knight>(PieceColor::WHITE);
    m_board[ 7_sz ][ 1_sz ] = std::make_unique<Knight>(PieceColor::BLACK);
    m_board[ 7_sz ][ 6_sz ] = std::make_unique<Knight>(PieceColor::BLACK);
    // Bishop
    m_board[ 0_sz ][ 2_sz ] = std::make_unique<Bishop>(PieceColor::WHITE);
    m_board[ 0_sz ][ 5_sz ] = std::make_unique<Bishop>(PieceColor::WHITE);
    m_board[ 7_sz ][ 2_sz ] = std::make_unique<Bishop>(PieceColor::BLACK);
    m_board[ 7_sz ][ 5_sz ] = std::make_unique<Bishop>(PieceColor::BLACK);
    // Queen
    m_board[ 0_sz ][ 3_sz ] = std::make_unique<Queen>(PieceColor::WHITE);
    m_board[ 7_sz ][ 3_sz ] = std::make_unique<Queen>(PieceColor::BLACK);
    // King
    m_board[ 0_sz ][ 4_sz ] = std::make_unique<King>(PieceColor::WHITE);
    m_board[ 7_sz ][ 4_sz ] = std::make_unique<King>(PieceColor::BLACK);
    m_white_stats.white_king = m_board[ 0_sz ][ 4_sz ].get();
    m_white_stats.black_king = m_board[ 7_sz ][ 4_sz ].get();
    auto piece_visitor = BoardPrinter();
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (m_board[ i ][ j ])
            {
                m_board[ i ][ j ]->visit(piece_visitor);
            }
        }
        std::cout << "\n";
    }
}

void internal::SideStats::reset()
{
    king_side_castle_possible = true;
    queen_side_castle_possible = true;
    in_under_check = false;
    en_passant_takeble = std::nullopt;
    piece_to_be_promoted = std::nullopt;
}