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

PieceColor get_oposite_color(PieceColor color)
{
    return color == PieceColor::WHITE ? PieceColor::BLACK : PieceColor::WHITE;
}

std::unique_ptr<Piece> Piece::get_piece_from_type(PieceType piece_type,
                                                  PieceColor piece_color,
                                                  const Position& piece_position)
{
    switch (piece_type)
    {
    case PieceType::KING:
        return std::make_unique<King>(piece_color, piece_position);
    case PieceType::QUEEN:
        return std::make_unique<Queen>(piece_color, piece_position);
    case PieceType::BISHOP:
        return std::make_unique<Bishop>(piece_color, piece_position);
    case PieceType::KNIGHT:
        return std::make_unique<Knight>(piece_color, piece_position);
    case PieceType::ROOK:
        return std::make_unique<Rook>(piece_color, piece_position);
    case PieceType::PAWN:
        return std::make_unique<Pawn>(piece_color, piece_position);
    default:
        break;
    }
    return nullptr;
}