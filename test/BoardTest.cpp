#include <gtest/gtest.h>

#include <Board.hpp>

TEST(Board, clone)
{
    Board original_board;
    original_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{2, 3}));
    const auto cloned_board = original_board.clone();
    ASSERT_FALSE(cloned_board.is_square_empty({2, 3}));
    const auto& piece = cloned_board.get_piece_at_position({2, 3});
    EXPECT_EQ(piece.get_type(), PieceType::ROOK);
    EXPECT_EQ(piece.get_color(), PieceColor::WHITE);
}
