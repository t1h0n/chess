#include <Board.hpp>
#include <stdexcept>

static constexpr std::int32_t BOARD_SIZE = 7;

bool Board::is_piece_position_valid(const Position& piece_position)
{
    return piece_position.x <= BOARD_SIZE && piece_position.x >= 0 && piece_position.y <= BOARD_SIZE
           && piece_position.y >= 0;
}

bool Board::add_piece(std::unique_ptr<Piece> piece)
{
    const auto& piece_position = piece->get_position();
    if (!is_square_empty(piece_position))
    {
        return false;
    }
    if (piece->get_type() == PieceType::KING)
    {
        if (!is_king_placement_valid(*piece))
        {
            return false;
        }
    }
    m_board[piece_position.y][piece_position.x] = std::move(piece);
    return true;
}

bool Board::is_king_placement_valid(const Piece& king)
{
    static const auto is_more_than_square_away
        = [](const Position& king, const Position& other_king) {
              const auto difference = king - other_king;
              return difference.x * difference.x + difference.y * difference.y > 2;
          };
    auto&& [this_king, other_king] = king.get_color() == PieceColor::WHITE
                                         ? std::tie(m_white_king, m_black_king)
                                         : std::tie(m_black_king, m_white_king);

    if (this_king)
    {
        return false;
    }
    if (other_king && king.get_position().length2(*other_king) <= 2)
    {
        return false;
    }
    this_king = king.get_position();
    return true;
}

std::unique_ptr<Piece> Board::remove_piece(const Position& position)
{
    if (is_square_empty(position))
    {
        return nullptr;
    }
    if (m_board[position.y][position.x]->get_type() == PieceType::KING)
    {
        m_board[position.y][position.x]->get_color() == PieceColor::WHITE ? m_white_king.reset()
                                                                          : m_black_king.reset();
    }
    return std::move(m_board[position.y][position.x]);
}

bool Board::is_square_empty(const Position& position) const
{
    return !is_piece_position_valid(position) || !m_board[position.y][position.x];
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
                piece_ptr->accept(visitor);
            }
        }
    }
}

const Piece& Board::get_piece_at_position(const Position& position) const
{
    if (is_square_empty(position))
    {
        throw std::logic_error(std::string(__PRETTY_FUNCTION__) + " No piece at given position");
    }
    return *m_board[position.y][position.x];
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
                cloned_board.m_board[piece_ptr->get_position().y][piece_ptr->get_position().x]
                    = Piece::get_piece_from_type(piece_ptr->get_type(), piece_ptr->get_color(),
                                                 piece_ptr->get_position());
            }
        }
    }
    return cloned_board;
}

const std::optional<Position>& Board::get_king_position(PieceColor side_to_move) const
{
    return side_to_move == PieceColor::WHITE ? m_white_king : m_black_king;
}