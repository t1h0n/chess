#pragma once
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>

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
PieceColor get_opposite_color(PieceColor color);

struct Position
{
    std::int32_t x;
    std::int32_t y;
    constexpr Position(std::int32_t x = 0, std::int32_t y = 0);
    constexpr std::int32_t length2(const Position& other) const;

    Position& operator+=(const Position& rhs);
    Position& operator-=(const Position& rhs);
};

inline std::ostream& operator<<(std::ostream& os, const Position& position)
{
    os << "{" << position.x << "," << position.y << "}";
    return os;
}

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
    Position m_position;

public:
    Piece(PieceColor piece_color, const Position& position);
    PieceColor get_color() const;
    const Position& get_position() const;
    void set_position(const Position& new_position);
    void move(const Position& amount);

    virtual PieceType get_type() const = 0;
    virtual void accept(PieceVisitor& visitor) = 0;
    virtual ~Piece() = default;

public:
    static std::unique_ptr<Piece> get_piece_from_type(PieceType piece_type,
                                                      PieceColor piece_color,
                                                      const Position& piece_position);
};

class King : public Piece
{
public:
    King(PieceColor piece_color, const Position& position)
        : Piece(piece_color, position)
    {
    }
    void accept(PieceVisitor& visitor) override
    {
        visitor.visit(*this);
    }
    PieceType get_type() const override
    {
        return PieceType::KING;
    }
};

class Queen : public Piece
{
public:
    Queen(PieceColor piece_color, const Position& position)
        : Piece(piece_color, position)
    {
    }
    void accept(PieceVisitor& visitor) override
    {
        visitor.visit(*this);
    }
    PieceType get_type() const override
    {
        return PieceType::QUEEN;
    }
};

class Bishop : public Piece
{
public:
    Bishop(PieceColor piece_color, const Position& position)
        : Piece(piece_color, position)
    {
    }
    void accept(PieceVisitor& visitor) override
    {
        visitor.visit(*this);
    }
    PieceType get_type() const override
    {
        return PieceType::BISHOP;
    }
};

class Knight : public Piece
{
public:
    Knight(PieceColor piece_color, const Position& position)
        : Piece(piece_color, position)
    {
    }
    void accept(PieceVisitor& visitor) override
    {
        visitor.visit(*this);
    }
    PieceType get_type() const override
    {
        return PieceType::KNIGHT;
    }
};

class Rook : public Piece
{
public:
    Rook(PieceColor piece_color, const Position& position)
        : Piece(piece_color, position)
    {
    }
    void accept(PieceVisitor& visitor) override
    {
        visitor.visit(*this);
    }
    PieceType get_type() const override
    {
        return PieceType::ROOK;
    }
};

class Pawn : public Piece
{
public:
    Pawn(PieceColor piece_color, const Position& position)
        : Piece(piece_color, position)
    {
    }
    void accept(PieceVisitor& visitor) override
    {
        visitor.visit(*this);
    }
    PieceType get_type() const override
    {
        return PieceType::PAWN;
    }
};

inline Piece::Piece(PieceColor piece_color, const Position& position)
    : m_piece_color(piece_color)
    , m_position(position)
{
}

inline PieceColor Piece::get_color() const
{
    return m_piece_color;
}

inline const Position& Piece::get_position() const
{
    return m_position;
}

inline void Piece::set_position(const Position& new_position)
{
    m_position = new_position;
}

inline void Piece::move(const Position& amount)
{
    m_position += amount;
}

inline constexpr Position::Position(std::int32_t x, std::int32_t y)
    : x{x}
    , y{y}
{
}

inline constexpr std::int32_t Position::length2(const Position& other) const
{
    const auto difference_x = x - other.x;
    const auto difference_y = y - other.y;
    return difference_x * difference_x + difference_y * difference_y;
}

inline Position operator+(const Position& lhs, const Position& rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

inline Position operator-(const Position& lhs, const Position& rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

inline Position& Position::operator+=(const Position& rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

inline Position& Position::operator-=(const Position& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

inline bool operator==(const Position& lhs, const Position& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const Position& lhs, const Position& rhs)
{
    return !(lhs.x == rhs.x && lhs.y == rhs.y);
}

namespace std
{
template <>
struct hash<Position>
{
    size_t operator()(const Position& posisition) const
    {
        return std::hash<std::int32_t>()(posisition.x << 8)
               ^ std::hash<std::int32_t>()(posisition.y);
    }
};
}  // namespace std
