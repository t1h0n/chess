#pragma once
#include <cstdint>

class PieceVisitor;
class Piece;
class King;
class Queen;
class Bishop;
class Knight;
class Rook;
class Pawn;

enum class PieceType : std::uint8_t
{
    KING,
    QUEEN,
    BISHOP,
    KNIGHT,
    ROOK,
    PAWN
};
const char* to_c_str(PieceType piece_type);

enum class PromotablePieceType : std::uint8_t
{
    QUEEN = static_cast<std::uint8_t>(PieceType::QUEEN),
    BISHOP = static_cast<std::uint8_t>(PieceType::BISHOP),
    KNIGHT = static_cast<std::uint8_t>(PieceType::KNIGHT),
    ROOK = static_cast<std::uint8_t>(PieceType::ROOK)
};
const char* to_c_str(PromotablePieceType promotable_piece_type);

enum class PieceColor : std::uint8_t
{
    WHITE,
    BLACK
};

const char* to_c_str(PieceColor color);

struct PiecePosition
{
    std::uint8_t x;
    std::uint8_t y;
    PiecePosition() = default;
    PiecePosition(std::uint8_t x, std::uint8_t y)
        : x{x}
        , y{y}
    {
    }
};

class PieceVisitor
{
public:
    virtual void visit(King& piece) = 0;
    virtual void visit(Queen& piece) = 0;
    virtual void visit(Bishop& piece) = 0;
    virtual void visit(Knight& piece) = 0;
    virtual void visit(Rook& piece) = 0;
    virtual void visit(Pawn& piece) = 0;
    virtual ~PieceVisitor() = default;
};

class Piece
{
protected:
    PieceColor m_piece_color;
    PiecePosition m_position;

public:
    Piece(PieceColor piece_color, const PiecePosition& position);
    PieceColor get_color() const;
    const PiecePosition& get_position() const;
    void set_position(const PiecePosition& new_position);

    virtual PieceType get_piece_type() const = 0;
    virtual void visit(PieceVisitor& visitor) = 0;
    virtual ~Piece() = default;
};

class King : public Piece
{
public:
    King(PieceColor piece_color, const PiecePosition& position)
        : Piece(piece_color, position)
    {
    }
    virtual void visit(PieceVisitor& visitor) override
    {
        visitor.visit(*this);
    }
    virtual PieceType get_piece_type() const override
    {
        return PieceType::KING;
    }
};

class Queen : public Piece
{
public:
    Queen(PieceColor piece_color, const PiecePosition& position)
        : Piece(piece_color, position)
    {
    }
    virtual void visit(PieceVisitor& visitor) override
    {
        visitor.visit(*this);
    }
    virtual PieceType get_piece_type() const override
    {
        return PieceType::QUEEN;
    }
};

class Bishop : public Piece
{
public:
    Bishop(PieceColor piece_color, const PiecePosition& position)
        : Piece(piece_color, position)
    {
    }
    virtual void visit(PieceVisitor& visitor) override
    {
        visitor.visit(*this);
    }
    virtual PieceType get_piece_type() const override
    {
        return PieceType::BISHOP;
    }
};

class Knight : public Piece
{
public:
    Knight(PieceColor piece_color, const PiecePosition& position)
        : Piece(piece_color, position)
    {
    }
    virtual void visit(PieceVisitor& visitor) override
    {
        visitor.visit(*this);
    }
    virtual PieceType get_piece_type() const override
    {
        return PieceType::KNIGHT;
    }
};

class Rook : public Piece
{
public:
    Rook(PieceColor piece_color, const PiecePosition& position)
        : Piece(piece_color, position)
    {
    }
    virtual void visit(PieceVisitor& visitor) override
    {
        visitor.visit(*this);
    }
    virtual PieceType get_piece_type() const override
    {
        return PieceType::ROOK;
    }
};

class Pawn : public Piece
{
public:
    Pawn(PieceColor piece_color, const PiecePosition& position)
        : Piece(piece_color, position)
    {
    }
    virtual void visit(PieceVisitor& visitor) override
    {
        visitor.visit(*this);
    }
    virtual PieceType get_piece_type() const override
    {
        return PieceType::PAWN;
    }
};

inline Piece::Piece(PieceColor piece_color, const PiecePosition& position)
    : m_piece_color(piece_color)
    , m_position(position)
{
}

inline PieceColor Piece::get_color() const
{
    return m_piece_color;
}

inline const PiecePosition& Piece::get_position() const
{
    return m_position;
}

inline void Piece::set_position(const PiecePosition& new_position)
{
    m_position = new_position;
}