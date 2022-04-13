#include <Board.hpp>
#include <stdexcept>

bool Board::is_piece_position_valid(const Position& piece_position)
{
    return piece_position.x <= 7 && piece_position.x >= 0 && piece_position.y <= 7
           && piece_position.y >= 0;
}

bool Board::add_piece(std::unique_ptr<Piece> piece)
{
    const auto& piece_position = piece->get_position();
    if (!is_square_empty(piece_position))
    {
        return false;
    }
    m_board[ piece_position.y ][ piece_position.x ] = std::move(piece);
    return true;
}

std::unique_ptr<Piece> Board::remove_piece(const Position& position)
{
    if (is_square_empty(position))
    {
        return nullptr;
    }
    return std::move(m_board[ position.y ][ position.x ]);
}

bool Board::is_square_empty(const Position& position) const
{
    return !is_piece_position_valid(position) || !m_board[ position.y ][ position.x ];
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

void Board::apply_piece_visitor(PieceVisitor& visitor)
{
    for (auto& row : m_board)
    {
        for (auto& piece_ptr : row)
        {
            if (piece_ptr)
            {
                piece_ptr->visit(visitor);
            }
        }
    }
}

const Piece& Board::get_piece_at_position(const Position& position) const
{
    if (is_square_empty(position))
    {
        throw std::logic_error("No piece at given position");
    }
    return *m_board[ position.y ][ position.x ];
}

Board Board::clone() const
{
    Board cloned_board;
    for (auto& row : m_board)
    {
        for (auto& piece_ptr : row)
        {
            if (piece_ptr)
            {
                cloned_board.m_board[ piece_ptr->get_position().y ][ piece_ptr->get_position().x ]
                    = Piece::get_piece_from_type(piece_ptr->get_type(), piece_ptr->get_color(),
                                                 piece_ptr->get_position());
            }
        }
    }
    return cloned_board;
}