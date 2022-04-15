#include <Board.hpp>
#include <MoveGenerator.hpp>
#include <array>
#include <literals.hpp>
#include <vector>

namespace
{
bool is_piece_info_valid(const Board& board,
                         const Position& piece_position,
                         PieceType expected_type,
                         PieceColor side_to_move)
{
    try
    {
        const auto& piece = board.get_piece_at_position(piece_position);
        return piece.get_type() == expected_type && piece.get_color() == side_to_move;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}

bool is_king_info_valid(const Board& board,
                        const Position& piece_position,
                        PieceColor side_to_move,
                        bool king_moved,
                        bool fisher_random)
{
    if (!is_piece_info_valid(board, piece_position, PieceType::KING, side_to_move))
    {
        return false;
    }
    // if king moved than we dont care about castling
    if (king_moved)
    {
        return true;
    }
    if (!fisher_random)
    {
        if (side_to_move == PieceColor::WHITE)
        {
            return board.get_piece_at_position(piece_position).get_position() == Position{4, 0};
        }
        return board.get_piece_at_position(piece_position).get_position() == Position{4, 7};
    }
    // if fisher random than we can only be sure about y position
    if (side_to_move == PieceColor::WHITE)
    {
        return board.get_piece_at_position(piece_position).get_position().y == 0;
    }
    return board.get_piece_at_position(piece_position).get_position().y == 7;
}

/**
 * @warning this function presumes that king info is valid
 */
bool is_rook_info_valid(const Board& board,
                        const std::optional<Position>& piece_position,
                        PieceColor side_to_move,
                        const Position& king_position,
                        bool fisher_random,
                        bool queen_side_rook = false)
{
    if (!piece_position)
    {
        return true;
    }
    if (!is_piece_info_valid(board, *piece_position, PieceType::ROOK, side_to_move))
    {
        return false;
    }
    const auto& rook_position = *piece_position;
    // if normal chess we know the exact position of the rooks
    if (!fisher_random)
    {
        if (queen_side_rook)
        {
            return side_to_move == PieceColor::WHITE ? rook_position == Position{0, 0}
                                                     : rook_position == Position{0, 7};
        }
        return side_to_move == PieceColor::WHITE ? rook_position == Position{7, 0}
                                                 : rook_position == Position{7, 7};
    }

    const bool y_position_valid
        = side_to_move == PieceColor::WHITE ? rook_position.y == 0 : rook_position.y == 7;

    const bool relative_to_king_position_valid
        = queen_side_rook ? rook_position.x < king_position.x : rook_position.x > king_position.x;
    return relative_to_king_position_valid && y_position_valid;
}

bool is_en_passant_info_valid(const Board& board,
                              const std::optional<Position>& piece_position,
                              PieceColor side_to_move)
{
    if (!piece_position)
    {
        return true;
    }
    const auto& pawn_position = *piece_position;
    if (side_to_move == PieceColor::WHITE)
    {
        return pawn_position.y == 5
               && is_piece_info_valid(board, pawn_position + Position{0, -1}, PieceType::PAWN,
                                      PieceColor::BLACK);
    }
    return pawn_position.y == 2
           && is_piece_info_valid(board, pawn_position + Position{0, 1}, PieceType::PAWN,
                                  PieceColor::WHITE);
}
}  // namespace

bool SpecialMovesData::is_ok(const Board& board, PieceColor side_to_move, bool fisher_random) const
{
    return is_king_info_valid(board, king_position, side_to_move, king_moved, fisher_random)
           && is_en_passant_info_valid(board, en_passant_takable, side_to_move)
           && is_rook_info_valid(board, king_side_rook, side_to_move, king_position, fisher_random)
           && is_rook_info_valid(board, queen_side_rook, side_to_move, king_position, fisher_random,
                                 true);
}
namespace
{
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
    bool is_board_valid_after_move(Board& board, const Position& king_position) const;

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
    const MovesContainer& get_available_raw_moves() const;
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

MoveGenerator::MoveGenerator(Board& board,
                             const SpecialMovesData& special_move_data,
                             PieceColor side_to_move)
    : m_board(board)
    , m_special_move_data(special_move_data)
    , m_side_to_move(side_to_move)
{
}

bool MoveGenerator::is_king_side_castle_possible() const
{
    return m_king_side_castle_possible;
}

bool MoveGenerator::is_queen_side_castle_possible() const
{
    return m_queen_side_castle_possible;
}

const MoveGenerator::MovesContainer& MoveGenerator::get_available_moves() const
{
    return m_available_moves;
}

RawMoveGenerator::RawMoveGenerator(const Board& board,
                                   const SpecialMovesData& special_move_data,
                                   PieceColor side_to_move)
    : m_board(board)
    , m_special_move_data(special_move_data)
    , m_side_to_move(side_to_move)
{
}

SquaresUnderAttackGenerator::SquaresUnderAttackGenerator(const Board& board,
                                                         PieceColor side_to_move)
    : m_board(board)
    , m_side_to_move(side_to_move)
{
}

const RawMoveGenerator::MovesContainer& RawMoveGenerator::get_available_raw_moves() const
{
    return m_available_moves;
}

const std::unordered_set<Position>& SquaresUnderAttackGenerator::get_squares_under_attack() const
{
    return m_squares_under_attack;
}

template <typename T>
void RawMoveGenerator::add_move_from_squares_under_attack(T& piece)
{
    SquaresUnderAttackGenerator squares_under_attack_generator{m_board, m_side_to_move};
    squares_under_attack_generator.visit(piece);
    const auto& available_squares = squares_under_attack_generator.get_squares_under_attack();
    if (!available_squares.empty())
    {
        m_available_moves[piece.get_position()] = available_squares;
    }
}

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
    const auto try_add_attacking_square = [this, &piece](const Position& piece_position) {
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
            m_available_moves[piece.get_position()].insert(piece_position);
        }
        else
        {
            if (m_board.get_piece_at_position(piece_position).get_color() == m_side_to_move)
            {
                return;
            }
            m_available_moves[piece.get_position()].insert(piece_position);
        }
    };
    const auto try_add_normal_square = [this, &piece](const Position& piece_position) {
        if (!Board::is_piece_position_valid(piece_position))
        {
            return;
        }
        if (m_board.is_square_empty(piece_position))
        {
            m_available_moves[piece.get_position()].insert(piece_position);
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
    const auto& available_moves = raw_move_generator.get_available_raw_moves();
    for (const auto& move_list : available_moves)
    {
        for (const auto& move_position : move_list.second)
        {
            auto new_board = m_board.clone();
            auto moving_piece = new_board.remove_piece(move_list.first);
            const auto& king_position = moving_piece->get_type() == PieceType::KING
                                            ? move_position
                                            : m_special_move_data.king_position;
            const auto taken_piece = new_board.remove_piece(move_position);
            if (taken_piece && taken_piece->get_type() == PieceType::KING)
            {
                throw std::logic_error("Invalid board! Can't capture king!");
            }
            moving_piece->set_position(move_position);
            new_board.add_piece(std::move(moving_piece));
            if (is_board_valid_after_move(new_board, king_position))
            {
                m_available_moves[move_list.first].insert(move_position);
            }
        }
    }
}

void MoveGenerator::generate_special_moves()
{  // TODO: add support of fisher random
    // if king moved no castling is possible
    if (m_special_move_data.king_moved)
    {
        return;
    }
    SquaresUnderAttackGenerator squares_under_attack_generator{m_board,
                                                               get_opposite_color(m_side_to_move)};
    m_board.apply_piece_visitor(squares_under_attack_generator);
    const auto& squares_under_attack = squares_under_attack_generator.get_squares_under_attack();

    // if king is under attack no castling is possible
    if (squares_under_attack.count(m_special_move_data.king_position))
    {
        return;
    }
    const auto can_move_through_square = [this, &squares_under_attack](const Position& square) {
        return m_board.is_square_empty(square) && !squares_under_attack.count(square);
    };
    const auto kings_y_coord = m_special_move_data.king_position.y;
    // king side
    if (m_special_move_data.king_side_rook)
    {
        m_king_side_castle_possible = can_move_through_square(Position{5, kings_y_coord})
                                      && can_move_through_square({6, kings_y_coord});
    }
    // queen side
    if (m_special_move_data.queen_side_rook)
    {
        m_queen_side_castle_possible = m_board.is_square_empty({1, kings_y_coord})
                                       && can_move_through_square({2, kings_y_coord})
                                       && can_move_through_square({3, kings_y_coord});
    }
}

bool MoveGenerator::is_board_valid_after_move(Board& board, const Position& king_position) const
{
    SquaresUnderAttackGenerator squares_under_attack_generator{board,
                                                               get_opposite_color(m_side_to_move)};
    board.apply_piece_visitor(squares_under_attack_generator);
    const auto& squares_under_attack = squares_under_attack_generator.get_squares_under_attack();
    return !squares_under_attack.count(king_position);
}

void MoveGenerator::generate_moves()
{
    if (!m_special_move_data.is_ok(m_board, m_side_to_move))
    {
        std::cerr << __PRETTY_FUNCTION__ << " INVALID SPECIAL_MOVE_DATA\n";
        return;
    }
    generate_normal_moves();
    generate_special_moves();
}
}  // namespace

SquaresUnderAttack generate_squares_under_attack(Board& board, PieceColor side_to_move)
{
    SquaresUnderAttackGenerator squares_under_attack_generator{board, side_to_move};
    board.apply_piece_visitor(squares_under_attack_generator);
    return squares_under_attack_generator.get_squares_under_attack();
}

NormalMoves generate_raw_available_moves(Board& board,
                                         const SpecialMovesData& special_move_data,
                                         PieceColor side_to_move)
{
    RawMoveGenerator raw_moves_generator{board, special_move_data, side_to_move};
    board.apply_piece_visitor(raw_moves_generator);
    return raw_moves_generator.get_available_raw_moves();
}

AvailableMoves generate_available_moves(Board& board,
                                        const SpecialMovesData& special_move_data,
                                        PieceColor side_to_move)
{
    MoveGenerator move_generator{board, special_move_data, side_to_move};
    move_generator.generate_moves();
    return {move_generator.get_available_moves(), move_generator.is_king_side_castle_possible(),
            move_generator.is_queen_side_castle_possible()};
}