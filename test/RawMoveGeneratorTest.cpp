#include <gtest/gtest.h>

#include <Board.hpp>
#include <RawMoveGenerator.hpp>

TEST(SquaresUnderAttackGenerator, pawn)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<Pawn>(PieceColor::WHITE, Position{3, 3}));
    chess_board.add_piece(std::make_unique<Pawn>(PieceColor::BLACK, Position{3, 4}));
    chess_board.add_piece(std::make_unique<Pawn>(PieceColor::WHITE, Position{4, 4}));
    SquaresUnderAttackGenerator squares_under_attack_generator{chess_board, PieceColor::WHITE};
    chess_board.apply_piece_visitor(squares_under_attack_generator);
    const std::unordered_set<Position> expected_available_squares = {{3, 5}, {2, 4}, {5, 5}};
    EXPECT_EQ(squares_under_attack_generator.get_squares_under_attack(),
              expected_available_squares);
}

TEST(SquaresUnderAttackGenerator, encircled_in_opposite_color_pieces_rook)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::BLACK, Position{3, 4}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{4, 4}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{2, 4}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{3, 5}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{3, 3}));
    SquaresUnderAttackGenerator squares_under_attack_generator{chess_board, PieceColor::BLACK};
    chess_board.apply_piece_visitor(squares_under_attack_generator);
    const std::unordered_set<Position> expected_available_squares
        = {{3, 5}, {2, 4}, {3, 3}, {4, 4}};

    EXPECT_EQ(squares_under_attack_generator.get_squares_under_attack(),
              expected_available_squares);
}

TEST(MoveGenerator, encircled_in_opposite_color_pieces_rook)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::BLACK, Position{3, 4}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{4, 4}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{2, 4}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{3, 5}));
    chess_board.add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{3, 3}));
    chess_board.add_piece(std::make_unique<King>(PieceColor::BLACK, Position{7, 7}));
    MoveGenerator move_generator{
        chess_board, {{7, 7}, true, std::nullopt, std::nullopt, std::nullopt}, PieceColor::BLACK};
    move_generator.generate_moves();
    const MoveGenerator::MovesContainer expected_available_moves
        = {{Position{3, 4}, std::unordered_set<Position>{{4, 4}, {3, 3}, {2, 4}, {3, 5}}},
           {Position{7, 7}, std::unordered_set<Position>{{6, 6}, {6, 7}, {7, 6}}}};
    EXPECT_EQ(move_generator.get_available_moves(), expected_available_moves);
}

TEST(MoveGenerator, en_passant)
{
    Board chess_board;
    chess_board.add_piece(std::make_unique<Pawn>(PieceColor::BLACK, Position{3, 4}));
    chess_board.add_piece(std::make_unique<Pawn>(PieceColor::WHITE, Position{4, 4}));
    chess_board.add_piece(std::make_unique<King>(PieceColor::WHITE, Position{7, 7}));
    MoveGenerator move_generator{
        chess_board,
        {{7, 7}, true, std::make_optional<Position>(3, 5), std::nullopt, std::nullopt},
        PieceColor::WHITE};
    move_generator.generate_moves();
    const MoveGenerator::MovesContainer expected_available_moves
        = {{Position{4, 4}, std::unordered_set<Position>{{3, 5}, {4, 5}}},
           {Position{7, 7}, std::unordered_set<Position>{{6, 6}, {6, 7}, {7, 6}}}};
    EXPECT_EQ(move_generator.get_available_moves(), expected_available_moves);
}