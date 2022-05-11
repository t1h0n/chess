#include <gtest/gtest.h>

#include <Board.hpp>

TEST(Board, clone)
{
    Board original_board;
    EXPECT_TRUE(
        original_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{2, 3})));
    const auto cloned_board = original_board.clone();
    ASSERT_FALSE(cloned_board.is_square_empty({2, 3}));
    const auto& piece = cloned_board.get_piece_at_position({2, 3});
    EXPECT_EQ(piece.get_type(), PieceType::ROOK);
    EXPECT_EQ(piece.get_color(), PieceColor::WHITE);
    EXPECT_FALSE(original_board.is_ok());
}

TEST(Board, is_ok_place_2_kings_same_color)
{
    Board board;
    EXPECT_TRUE(board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{2, 3})));
    EXPECT_FALSE(board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{2, 3})));
    EXPECT_TRUE(board.add_piece(std::make_unique<King>(PieceColor::BLACK, Position{5, 3})));
    EXPECT_TRUE(board.is_ok());
}

TEST(Board, is_ok_place_2_kings_next_to_each_other)
{
    Board board;
    EXPECT_TRUE(board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{2, 3})));
    EXPECT_FALSE(board.add_piece(std::make_unique<King>(PieceColor::BLACK, Position{3, 4})));
    EXPECT_FALSE(board.is_ok());
}

TEST(Board, is_ok)
{
    Board board;
    EXPECT_TRUE(board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{2, 3})));
    EXPECT_TRUE(board.add_piece(std::make_unique<King>(PieceColor::BLACK, Position{4, 4})));
    EXPECT_TRUE(board.is_ok());
}

TEST(Board, get_piece_position_invalid)
{
    Board board;
    EXPECT_THROW(board.get_piece_at_position(Position{1, 1}), std::logic_error);
}
