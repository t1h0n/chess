#pragma once

#include <Board.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <memory>
#include <unordered_map>

class ResourceManager;

namespace sf
{
class RenderWindow;
}

struct StyleData
{
    struct TextureLocations
    {
        std::string king_path;
        std::string queen_path;
        std::string rook_path;
        std::string bishop_path;
        std::string knight_path;
        std::string pawn_path;
    };
    TextureLocations white_pieces;
    TextureLocations black_pieces;
    std::string board_shader_path;
    std::int32_t side_size = 0;
    sf::Color board_black_color;
    sf::Color board_white_color;
    bool inverse_board = false;
};

class ChessView
{
public:
    ChessView(ResourceManager& resource_manager,
              const std::shared_ptr<Board>& board,
              const StyleData& style_data);

    void set_style_data(const StyleData& style_data);

    void select_piece(const Position& piece_position);
    void unselect_piece();
    void set_selected_piece_position(std::int32_t x, std::int32_t y);

    void render(sf::RenderWindow& target);
    void update();

private:
    void process_texture_locations(const StyleData::TextureLocations& new_locations);

private:
    ResourceManager& m_resource_manager;
    std::shared_ptr<Board> m_board;
    StyleData m_style_data;

    std::unordered_map<Position, sf::Sprite> m_pieces_sprites;
    Position m_selected_piece;

    sf::RectangleShape m_board_sprite;
    sf::Shader m_board_shader;
};