#include <Pieces.hpp>

const char* to_c_str(PieceType piece_type)
{
    switch (piece_type)
    {
    case PieceType::KING:
        return "KING";
    case PieceType::QUEEN:
        return "QUEEN";
    case PieceType::BISHOP:
        return "BISHOP";
    case PieceType::KNIGHT:
        return "KNIGHT";
    case PieceType::ROOK:
        return "ROOK";
    case PieceType::PAWN:
        return "PAWN";
    default:
        return "error";
    };
}

const char* to_c_str(PromotablePieceType promotable_piece_type)
{
    switch (promotable_piece_type)
    {
    case PromotablePieceType::QUEEN:
        return "QUEEN";
    case PromotablePieceType::BISHOP:
        return "BISHOP";
    case PromotablePieceType::KNIGHT:
        return "KNIGHT";
    case PromotablePieceType::ROOK:
        return "ROOK";
    default:
        return "error";
    };
}

const char* to_c_str(PieceColor color)
{
    switch (color)
    {
    case PieceColor::WHITE:
        return "w";
    case PieceColor::BLACK:
        return "b";
    default:
        return "error";
    };
}