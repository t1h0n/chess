#include <Board.hpp>
#include <iostream>
#include <literals.hpp>
#include <optional>
namespace
{
struct MoveWithSideEffects
{
    PiecePosition position;
    std::optional<internal::CastlingRights> side_effects;
    MoveWithSideEffects(std::uint8_t x,
                        std::uint8_t y,
                        std::optional<internal::CastlingRights> side_effects = std::nullopt)
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
                        const internal::CastlingRights& current_stats,
                        const internal::CastlingRights& other_stats)
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
        if (m_piece_position.x > 0)
        {
            if (m_piece_position.y > 0)
            {
                try_add_move(m_piece_position.x - 1, m_piece_position.y - 1, piece,
                             move_side_effects);
            }
            if (m_piece_position.y < 8)
            {
                try_add_move(m_piece_position.x - 1, m_piece_position.y + 1, piece,
                             move_side_effects);
            }
            try_add_move(m_piece_position.x - 1, m_piece_position.y, piece, move_side_effects);
        }
        if (m_piece_position.x < 8)
        {
            if (m_piece_position.y > 0)
            {
                try_add_move(m_piece_position.x + 1, m_piece_position.y - 1, piece,
                             move_side_effects);
            }
            if (m_piece_position.y < 8)
            {
                try_add_move(m_piece_position.x + 1, m_piece_position.y + 1, piece,
                             move_side_effects);
            }
            try_add_move(m_piece_position.x + 1, m_piece_position.y, piece, move_side_effects);
        }

        if (m_piece_position.y > 0)
        {
            try_add_move(m_piece_position.x, m_piece_position.y - 1, piece, move_side_effects);
        }
        if (m_piece_position.y < 8)
        {
            try_add_move(m_piece_position.x, m_piece_position.y + 1, piece, move_side_effects);
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
    }

private:
    bool try_add_move(std::uint8_t x,
                      std::uint8_t y,
                      Piece& current_piece,
                      std::optional<internal::CastlingRights> side_effects = std::nullopt)
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
                                   std::optional<internal::CastlingRights> side_effects
                                   = std::nullopt)
    {
        if (!m_board[ y ][ x ])
        {
            m_available_moves.emplace_back(x, y, side_effects);
        }
    }

    void try_add_pawn_capture_move(std::uint8_t x,
                                   std::uint8_t y,
                                   Piece& current_piece,
                                   std::optional<internal::CastlingRights> side_effects
                                   = std::nullopt)
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
    const internal::CastlingRights& m_current_stats;
    const internal::CastlingRights& m_other_stats;
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

void Board::reset_board()  // delegate this to boardgenerator
{
    for (auto& row : m_board)
    {
        for (auto& piece : row)
        {
            piece.reset();
        }
    }
    m_player_to_move = PieceColor::WHITE;
    // Pawns
    for (auto i = 0_sz; i < 8_sz; ++i)
    {
        m_board[ 1_sz ][ i ] = std::make_unique<Pawn>(
            PieceColor::WHITE, PiecePosition(static_cast<std::uint8_t>(i), 1));
        m_board[ 6_sz ][ i ] = std::make_unique<Pawn>(
            PieceColor::BLACK, PiecePosition(static_cast<std::uint8_t>(i), 6));
    }
    // Rooks
    m_board[ 0_sz ][ 0_sz ] = std::make_unique<Rook>(PieceColor::WHITE, PiecePosition(0, 0));
    m_board[ 0_sz ][ 7_sz ] = std::make_unique<Rook>(PieceColor::WHITE, PiecePosition(0, 7));
    m_board[ 7_sz ][ 0_sz ] = std::make_unique<Rook>(PieceColor::BLACK, PiecePosition(7, 0));
    m_board[ 7_sz ][ 7_sz ] = std::make_unique<Rook>(PieceColor::BLACK, PiecePosition(7, 7));
    // Knight
    m_board[ 0_sz ][ 1_sz ] = std::make_unique<Knight>(PieceColor::WHITE, PiecePosition(0, 1));
    m_board[ 0_sz ][ 6_sz ] = std::make_unique<Knight>(PieceColor::WHITE, PiecePosition(0, 6));
    m_board[ 7_sz ][ 1_sz ] = std::make_unique<Knight>(PieceColor::BLACK, PiecePosition(7, 1));
    m_board[ 7_sz ][ 6_sz ] = std::make_unique<Knight>(PieceColor::BLACK, PiecePosition(7, 6));
    // Bishop
    m_board[ 0_sz ][ 2_sz ] = std::make_unique<Bishop>(PieceColor::WHITE, PiecePosition(0, 2));
    m_board[ 0_sz ][ 5_sz ] = std::make_unique<Bishop>(PieceColor::WHITE, PiecePosition(0, 5));
    m_board[ 7_sz ][ 2_sz ] = std::make_unique<Bishop>(PieceColor::BLACK, PiecePosition(7, 2));
    m_board[ 7_sz ][ 5_sz ] = std::make_unique<Bishop>(PieceColor::BLACK, PiecePosition(7, 5));
    // Queen
    m_board[ 0_sz ][ 3_sz ] = std::make_unique<Queen>(PieceColor::WHITE, PiecePosition(0, 3));
    m_board[ 7_sz ][ 3_sz ] = std::make_unique<Queen>(PieceColor::BLACK, PiecePosition(7, 3));
    // King
    m_board[ 0_sz ][ 4_sz ] = std::make_unique<King>(PieceColor::WHITE, PiecePosition(0, 4));
    m_board[ 7_sz ][ 4_sz ] = std::make_unique<King>(PieceColor::BLACK, PiecePosition(7, 4));
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

GuiChessBoard Board::get_chess_board() const
{
    GuiChessBoard board;
    for (const auto& column : m_board)
    {
        for (auto& piece : column)
        {
            if (piece)
            {
                board.push_back(piece.get());
            }
        }
    }
    return board;
}

MoveResult Board::move_piece(const PiecePosition& from, const PiecePosition& to)
{
    if (is_move_valid(from, to))
    {
        return postprocess_move(from, to);
    }
    return MoveResult::FAILURE;
}
