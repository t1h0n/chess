#include <RawMoveGenerator.hpp>
#include <array>
#include <literals.hpp>

bool SquaresUnderAttackGenerator::should_generate_move(PieceColor color) const
{
    return color == m_side_to_move;
}

bool SquaresUnderAttackGenerator::try_add_square(const Position& piece_position)
{
    if (!Board::is_piece_position_valid(piece_position))
    {
        return false;
    }
    if (!m_board.is_square_empty(piece_position))
    {
        const auto& piece = m_board.get_piece_at_position(piece_position);
        if (piece.get_color() != m_side_to_move)
        {
            m_squares_under_attack.insert(piece_position);
        }
        return false;
    }
    m_squares_under_attack.insert(piece_position);
    return true;
}

void SquaresUnderAttackGenerator::generate_diagonal_moves(const Position& piece_position)
{
    for (std::int32_t i = 1; i != 9; ++i)
    {
        if (!try_add_square(piece_position + Position{i, i}))
        {
            break;
        }
    }
    for (std::int32_t i = -1; i != -9; --i)
    {
        if (!try_add_square(piece_position + Position{i, i}))
        {
            break;
        }
    }

    for (std::int32_t i = 1; i != 9; ++i)
    {
        if (!try_add_square(piece_position + Position{-i, i}))
        {
            break;
        }
    }
    for (std::int32_t i = -1; i != -9; --i)
    {
        if (!try_add_square(piece_position + Position{-i, i}))
        {
            break;
        }
    }
}

void SquaresUnderAttackGenerator::generate_vertical_moves(const Position& piece_position)
{
    for (std::int32_t i = 1; i != 9; ++i)
    {
        if (!try_add_square(piece_position + Position{i, 0}))
        {
            break;
        }
    }
    for (std::int32_t i = -1; i != -9; --i)
    {
        if (!try_add_square(piece_position + Position{i, 0}))
        {
            break;
        }
    }

    for (std::int32_t i = 1; i != 9; ++i)
    {
        if (!try_add_square(piece_position + Position{0, i}))
        {
            break;
        }
    }
    for (std::int32_t i = -1; i != -9; --i)
    {
        if (!try_add_square(piece_position + Position{0, i}))
        {
            break;
        }
    }
}

void SquaresUnderAttackGenerator::visit(King& piece)
{
    if (!should_generate_move(piece.get_color()))
    {
        return;
    }
    for (std::int32_t i = -1; i != 2; ++i)
    {
        for (std::int32_t j = -1; j != 2; ++j)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            try_add_square(piece.get_position() + Position{i, j});
        }
    }
}
void SquaresUnderAttackGenerator::visit(Queen& piece)
{
    if (!should_generate_move(piece.get_color()))
    {
        return;
    }
    generate_diagonal_moves(piece.get_position());
    generate_vertical_moves(piece.get_position());
}
void SquaresUnderAttackGenerator::visit(Bishop& piece)
{
    if (!should_generate_move(piece.get_color()))
    {
        return;
    }
    generate_diagonal_moves(piece.get_position());
}
void SquaresUnderAttackGenerator::visit(Knight& piece)
{
    if (!should_generate_move(piece.get_color()))
    {
        return;
    }
    try_add_square(piece.get_position() + Position{2, 1});
    try_add_square(piece.get_position() + Position{-2, 1});
    try_add_square(piece.get_position() + Position{1, 2});
    try_add_square(piece.get_position() + Position{-1, 2});

    try_add_square(piece.get_position() + Position{-2, -1});
    try_add_square(piece.get_position() + Position{2, -1});
    try_add_square(piece.get_position() + Position{-1, -2});
    try_add_square(piece.get_position() + Position{1, -2});
}
void SquaresUnderAttackGenerator::visit(Rook& piece)
{
    if (!should_generate_move(piece.get_color()))
    {
        return;
    }
    generate_vertical_moves(piece.get_position());
}
void SquaresUnderAttackGenerator::visit(Pawn& piece)
{
    if (!should_generate_move(piece.get_color()))
    {
        return;
    }
    if (piece.get_color() == PieceColor::WHITE)
    {
        try_add_square(piece.get_position() + Position{1, 1});
        try_add_square(piece.get_position() + Position{-1, 1});
    }
    else
    {
        try_add_square(piece.get_position() + Position{1, -1});
        try_add_square(piece.get_position() + Position{-1, -1});
    }
}

void RawMoveGenerator::visit(King& piece)
{
    add_move_from_squares_under_attack(piece);
}
void RawMoveGenerator::visit(Queen& piece)
{
    add_move_from_squares_under_attack(piece);
}
void RawMoveGenerator::visit(Bishop& piece)
{
    add_move_from_squares_under_attack(piece);
}
void RawMoveGenerator::visit(Knight& piece)
{
    add_move_from_squares_under_attack(piece);
}
void RawMoveGenerator::visit(Rook& piece)
{
    add_move_from_squares_under_attack(piece);
}
void RawMoveGenerator::visit(Pawn& piece)
{
    if (piece.get_color() != m_side_to_move)
    {
        return;
    }
    const auto try_add_attacking_square = [ this, &piece ](const Position& piece_position) {
        if (!Board::is_piece_position_valid(piece_position))
        {
            return;
        }
        if (m_board.is_square_empty(piece_position))
        {
            // check for en_pasant
            if (!m_special_move_data.en_passant_takable)
            {
                return;
            }
            if (*m_special_move_data.en_passant_takable != piece_position)
            {
                return;
            }
            m_available_moves[ piece.get_position() ].insert(piece_position);
        }
        else
        {
            if (m_board.get_piece_at_position(piece_position).get_color() == m_side_to_move)
            {
                return;
            }
            m_available_moves[ piece.get_position() ].insert(piece_position);
        }
    };
    const auto try_add_normal_square = [ this, &piece ](const Position& piece_position) {
        if (!Board::is_piece_position_valid(piece_position))
        {
            return;
        }
        if (m_board.is_square_empty(piece_position))
        {
            m_available_moves[ piece.get_position() ].insert(piece_position);
        }
    };
    if (piece.get_color() == PieceColor::WHITE)
    {
        try_add_attacking_square(piece.get_position() + Position{1, 1});
        try_add_attacking_square(piece.get_position() + Position{-1, 1});
        try_add_normal_square(piece.get_position() + Position{0, 1});
    }
    else
    {
        try_add_attacking_square(piece.get_position() + Position{1, -1});
        try_add_attacking_square(piece.get_position() + Position{-1, -1});
        try_add_normal_square(piece.get_position() + Position{0, -1});
    }
}

void MoveGenerator::generate_normal_moves()
{
    RawMoveGenerator raw_move_generator{m_board, m_special_move_data, m_side_to_move};
    m_board.apply_piece_visitor(raw_move_generator);
    const auto& available_moves = raw_move_generator.get_available_moves();
    for (const auto& move_list : available_moves)
    {
        for (const auto& move_position : move_list.second)
        {
            auto new_board = m_board.clone();
            auto moving_piece = new_board.remove_piece(move_list.first);
            moving_piece->set_position(move_position);
            new_board.add_piece(std::move(moving_piece));
            if (is_board_valid_after_move(new_board))
            {
                m_available_moves[ move_list.first ].insert(move_position);
            }
        }
    }
}

void MoveGenerator::generate_special_moves()
{
    if (m_special_move_data.king_moved)
    {
        return;
    }
    SquaresUnderAttackGenerator squares_under_attack_generator{m_board,
                                                               get_oposite_color(m_side_to_move)};
    m_board.apply_piece_visitor(squares_under_attack_generator);
    const auto& squares_under_attack = squares_under_attack_generator.get_squares_under_attack();
    const auto& king_position = m_special_move_data.king_position;
    const auto is_square_ok
        = [ &squares_under_attack, this ](const Position& square, const Position& rook_position) {
              return (m_board.is_square_empty(square) || square == rook_position)
                     && !squares_under_attack.count(square);
          };
    // king side
    if (m_special_move_data.king_side_rook)
    {
        if (is_square_ok(king_position + Position{1, 0}, *m_special_move_data.king_side_rook)
            && is_square_ok(king_position + Position{2, 0}, *m_special_move_data.king_side_rook))
        {
            m_king_side_castle_possible = true;
        }
    }
    // queen side
    if (m_special_move_data.queen_side_rook)
    {
        if (is_square_ok(king_position + Position{-1, 0}, *m_special_move_data.king_side_rook)
            && is_square_ok(king_position + Position{-2, 0}, *m_special_move_data.king_side_rook)
            && (m_board.is_square_empty(king_position + Position{-3, 0})
                || king_position + Position{-3, 0} == *m_special_move_data.queen_side_rook))
        {
            m_queen_side_castle_possible = true;
        }
    }
}

bool MoveGenerator::is_board_valid_after_move(Board& board) const
{
    SquaresUnderAttackGenerator squares_under_attack_generator{board,
                                                               get_oposite_color(m_side_to_move)};
    board.apply_piece_visitor(squares_under_attack_generator);
    const auto& squares_under_attack = squares_under_attack_generator.get_squares_under_attack();
    return !squares_under_attack.count(m_special_move_data.king_position);
}

void MoveGenerator::generate_moves()
{
    generate_normal_moves();
    generate_special_moves();
}