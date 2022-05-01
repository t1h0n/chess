#include <gtest/gtest.h>

#include <Board.hpp>
#include <MoveGenerator.hpp>

TEST(SquaresUnderAttackGenerator, pawn)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<Pawn>(PieceColor::WHITE, Position{3, 3}));
    chess_board.add_piece(std::make_unique<Pawn>(PieceColor::BLACK, Position{3, 4}));
    chess_board.add_piece(std::make_unique<Pawn>(PieceColor::WHITE, Position{4, 4}));
    const auto squares_under_attack = generate_squares_under_attack(chess_board, PieceColor::WHITE);
    const std::unordered_set<Position> expected_squares_under_attack = {{3, 5}, {2, 4}, {5, 5}};
    EXPECT_EQ(squares_under_attack, expected_squares_under_attack);
}

TEST(SquaresUnderAttackGenerator, encircled_in_opposite_color_pieces_rook)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::BLACK, Position{3, 4}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{4, 4}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{2, 4}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{3, 5}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{3, 3}));
    const auto squares_under_attack = generate_squares_under_attack(chess_board, PieceColor::BLACK);
    const std::unordered_set<Position> expected_squares_under_attack
        = {{3, 5}, {2, 4}, {3, 3}, {4, 4}};

    EXPECT_EQ(squares_under_attack, expected_squares_under_attack);
}

TEST(MoveGenerator, encircled_in_opposite_color_pieces_rook)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::BLACK, Position{3, 4}));
    chess_board.add_piece(std::make_unique<King>(PieceColor::BLACK, Position{7, 7}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{4, 4}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{2, 4}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{3, 5}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{3, 3}));
    chess_board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{0, 0}));
    const auto available_moves = generate_available_moves(chess_board, {}, PieceColor::BLACK);
    const NormalMoves expected_available_normal_moves
        = {{Position{3, 4}, std::unordered_set<Position>{{4, 4}, {3, 3}, {2, 4}, {3, 5}}},
           {Position{7, 7}, std::unordered_set<Position>{{6, 6}, {6, 7}, {7, 6}}}};
    EXPECT_EQ(available_moves.normal_moves, expected_available_normal_moves);
}

TEST(MoveGenerator, en_passant)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<King>(PieceColor::BLACK, Position{0, 0}));
    chess_board.add_piece(std::make_unique<Pawn>(PieceColor::BLACK, Position{3, 4}));
    chess_board.add_piece(std::make_unique<Pawn>(PieceColor::WHITE, Position{4, 4}));
    chess_board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{7, 0}));
    const auto available_moves = generate_available_moves(
        chess_board, {std::nullopt, std::nullopt, Position{3, 5}}, PieceColor::WHITE);

    const NormalMoves expected_available_moves
        = {{Position{4, 4}, std::unordered_set<Position>{{3, 5}, {4, 5}}},
           {Position{7, 0}, std::unordered_set<Position>{{6, 0}, {6, 1}, {7, 1}}}};
    EXPECT_EQ(available_moves.normal_moves, expected_available_moves);
}

TEST(MoveGenerator, king_side_castling_possible)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{4, 0}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{7, 0}));
    chess_board.add_piece(std::make_unique<King>(PieceColor::BLACK, Position{6, 7}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::BLACK, Position{7, 1}));
    const auto available_moves = generate_available_moves(
        chess_board, {std::nullopt, Position{7, 0}, std::nullopt}, PieceColor::WHITE);

    EXPECT_TRUE(available_moves.king_side_castle_possible);
}

TEST(MoveGenerator, king_side_castling_impossible)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{4, 0}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{7, 0}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::BLACK, Position{4, 1}));
    chess_board.add_piece(std::make_unique<King>(PieceColor::BLACK, Position{5, 7}));
    const auto available_moves = generate_available_moves(
        chess_board, {std::nullopt, Position{7, 0}, std::nullopt}, PieceColor::WHITE);

    const NormalMoves expected_available_moves
        = {{Position{4, 0}, std::unordered_set<Position>{{4, 1}, {3, 0}, {5, 0}}}};
    EXPECT_FALSE(available_moves.king_side_castle_possible);
    EXPECT_EQ(available_moves.normal_moves, expected_available_moves);
}

TEST(MoveGenerator, queen_side_castling_possible)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{4, 0}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{0, 0}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::BLACK, Position{1, 1}));
    chess_board.add_piece(std::make_unique<King>(PieceColor::BLACK, Position{1, 7}));
    const auto available_moves = generate_available_moves(
        chess_board, {Position{0, 0}, std::nullopt, std::nullopt}, PieceColor::WHITE);

    EXPECT_TRUE(available_moves.queen_side_castle_possible);
}

TEST(MoveGenerator, king_under_check_piece_is_defended)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{4, 0}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::BLACK, Position{4, 1}));
    chess_board.add_piece(std::make_unique<King>(PieceColor::BLACK, Position{4, 2}));
    const auto available_moves = generate_available_moves(chess_board, {}, PieceColor::WHITE);

    const NormalMoves expected_available_moves
        = {{Position{4, 0}, std::unordered_set<Position>{{3, 0}, {5, 0}}}};
    EXPECT_EQ(available_moves.normal_moves, expected_available_moves);
}

TEST(MoveGenerator, pawn)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{4, 0}));
    chess_board.add_piece(std::make_unique<King>(PieceColor::BLACK, Position{4, 2}));
    chess_board.add_piece(std::make_unique<Pawn>(PieceColor::BLACK, Position{4, 6}));
    const auto available_moves = generate_available_moves(chess_board, {}, PieceColor::BLACK);

    const SquaresUnderAttack expected_pawn_moves = {{4, 4}, {4, 5}};
    EXPECT_EQ(available_moves.normal_moves.at(Position{4, 6}), expected_pawn_moves);
}

TEST(MoveGenerator, knight)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{4, 0}));
    chess_board.add_piece(std::make_unique<King>(PieceColor::BLACK, Position{4, 2}));
    chess_board.add_piece(std::make_unique<Knight>(PieceColor::WHITE, Position{1, 0}));
    chess_board.add_piece(std::make_unique<Pawn>(PieceColor::WHITE, Position{2, 2}));
    const auto available_moves = generate_available_moves(chess_board, {}, PieceColor::WHITE);

    const SquaresUnderAttack expected_knight_moves = {{0, 2}, {3, 1}};
    EXPECT_EQ(available_moves.normal_moves.at(Position{1, 0}), expected_knight_moves);
}

TEST(MoveGenerator, close_kings)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{4, 0}));
    chess_board.add_piece(std::make_unique<King>(PieceColor::BLACK, Position{4, 2}));
    const auto available_moves = generate_available_moves(chess_board, {}, PieceColor::WHITE);

    const SquaresUnderAttack expected_king_moves = {{3, 0}, {5, 0}};
    EXPECT_EQ(available_moves.normal_moves.at(Position{4, 0}), expected_king_moves);
}

TEST(MoveGenerator, chack_mate)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{4, 0}));
    chess_board.add_piece(std::make_unique<King>(PieceColor::BLACK, Position{4, 2}));
    chess_board.add_piece(std::make_unique<Queen>(PieceColor::BLACK, Position{4, 1}));
    const auto available_moves = generate_available_moves(chess_board, {}, PieceColor::BLACK);

    EXPECT_TRUE(available_moves.normal_moves.empty());
}