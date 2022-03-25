#include <gtest/gtest.h>

#include <Board.hpp>

TEST(Board, empty_board)
{
    Board board;
    ChessBoard chess_board;
    EXPECT_EQ(board.get_chess_board(), chess_board);
}
