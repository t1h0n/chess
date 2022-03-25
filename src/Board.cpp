#include <Board.hpp>

#define RETURN_IF_TRUE(expr) \
    if (expr)                \
    return false

namespace
{
bool is_piece_position_valid(const PiecePosition& piece_position)
{
    return piece_position.x < 7 && piece_position.x > 0 && piece_position.y < 7
           && piece_position.y > 0;
}
}  // namespace

bool Board::add_piece(std::unique_ptr<Piece> piece)
{
    const auto& piece_position = piece->get_position();
    RETURN_IF_TRUE(!is_piece_position_valid(piece_position));
    RETURN_IF_TRUE(m_board[ piece_position.y ][ piece_position.x ]);

    m_board[ piece_position.y ][ piece_position.x ] = std::move(piece);
    return true;
}

bool Board::remove_piece(const PiecePosition& position)
{
    RETURN_IF_TRUE(!is_piece_position_valid(position));
    RETURN_IF_TRUE(!m_board[ position.y ][ position.x ]);

    m_board[ position.y ][ position.x ].reset();
    return true;
}

void Board::clear_board()
{
    for (auto& row : m_board)
    {
        for (auto& piece_ptr : row)
        {
            piece_ptr.reset();
        }
    }
}
