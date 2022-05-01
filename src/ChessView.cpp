#include <ChessView.hpp>
#include <Pieces.hpp>
#include <ResourceManager.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <map>

namespace
{
sf::Glsl::Vec3 convert_color_to_vec3(const sf::Color& color)
{
    return {static_cast<float>(color.r) / 255.0f, static_cast<float>(color.g) / 255.0f,
            static_cast<float>(color.b) / 255.0f};
}

class GuiBoardGenerator : public PieceVisitor
{
public:
    GuiBoardGenerator(const StyleData::TextureLocations& white_pieces,
                      const StyleData::TextureLocations& black_pieces,
                      ResourceManager& resource_manager,
                      bool inverse_board,
                      std::int32_t side_size)
        : m_white_pieces{white_pieces}
        , m_black_pieces{black_pieces}
        , m_resource_manager{resource_manager}
        , m_inverse_board{inverse_board}
        , m_side_size{side_size}
    {
    }
    void visit(King& piece)
    {
        insert_piece(piece.get_position(), get_appropriate_style_data(piece.get_color()).king_path);
    }
    void visit(Queen& piece)
    {
        insert_piece(piece.get_position(),
                     get_appropriate_style_data(piece.get_color()).queen_path);
    }
    void visit(Bishop& piece)
    {
        insert_piece(piece.get_position(),
                     get_appropriate_style_data(piece.get_color()).bishop_path);
    }
    void visit(Knight& piece)
    {
        insert_piece(piece.get_position(),
                     get_appropriate_style_data(piece.get_color()).knight_path);
    }
    void visit(Rook& piece)
    {
        insert_piece(piece.get_position(), get_appropriate_style_data(piece.get_color()).rook_path);
    }
    void visit(Pawn& piece)
    {
        insert_piece(piece.get_position(), get_appropriate_style_data(piece.get_color()).pawn_path);
    }
    std::unordered_map<Position, sf::Sprite>&& get_gui_board()
    {
        return std::move(m_pieces_sprites);
    }

private:
    const StyleData::TextureLocations& get_appropriate_style_data(PieceColor color)
    {
        return color == PieceColor::WHITE ? m_white_pieces : m_black_pieces;
    }

    void insert_piece(const Position& position, const std::string& path)
    {
        const auto& texture = m_resource_manager.get_texture(path);
        sf::Sprite new_sprite{texture};
        new_sprite.setScale(
            {static_cast<float>(m_side_size) / static_cast<float>(texture.getSize().x),
             static_cast<float>(m_side_size) / static_cast<float>(texture.getSize().y)});

        new_sprite.setPosition(
            {static_cast<float>(m_side_size * position.x),
             static_cast<float>(m_side_size * (m_inverse_board ? position.y : 7 - position.y))});
        m_pieces_sprites.emplace(position, new_sprite);
    }

private:
    const StyleData::TextureLocations& m_white_pieces;
    const StyleData::TextureLocations& m_black_pieces;
    ResourceManager& m_resource_manager;
    bool m_inverse_board;
    std::int32_t m_side_size;

    std::unordered_map<Position, sf::Sprite> m_pieces_sprites;
};
std::unordered_map<Position, sf::Sprite> generate_gui_board(
    const StyleData::TextureLocations& white_pieces,
    const StyleData::TextureLocations& black_pieces,
    ResourceManager& resource_manager,
    const std::shared_ptr<Board>& board,
    bool inverse_board,
    std::int32_t side_size)
{
    GuiBoardGenerator gui_board_generator{white_pieces, black_pieces, resource_manager,
                                          inverse_board, side_size};
    board->apply_piece_visitor(gui_board_generator);
    return gui_board_generator.get_gui_board();
}
}  // namespace

ChessView::ChessView(ResourceManager& resource_manager,
                     const std::shared_ptr<Board>& board,
                     const StyleData& style_data)
    : m_resource_manager{resource_manager}
    , m_board{board}
{
    set_style_data(style_data);
}

void ChessView::select_piece(const Position& piece_position)
{
    m_selected_piece = piece_position;
}

void ChessView::unselect_piece()
{
    m_pieces_sprites[m_selected_piece].setPosition(
        m_selected_piece.x * m_style_data.side_size,
        m_style_data.inverse_board ? m_selected_piece.y * m_style_data.side_size
                                   : (7 - m_selected_piece.y) * m_style_data.side_size);
}

void ChessView::set_selected_piece_position(std::int32_t x, std::int32_t y)
{
    m_pieces_sprites[m_selected_piece].setPosition(
        {static_cast<float>(x - m_style_data.side_size / 2),
         static_cast<float>(y - m_style_data.side_size / 2)});
}

void ChessView::set_style_data(const StyleData& style_data)
{
    if (m_style_data.board_shader_path != style_data.board_shader_path)
    {
        m_board_shader.loadFromFile(style_data.board_shader_path, sf::Shader::Type::Fragment);
        m_board_shader.setUniform("side_size", style_data.side_size);
        m_board_shader.setUniform("black_color",
                                  convert_color_to_vec3(style_data.board_black_color));
        m_board_shader.setUniform("white_color",
                                  convert_color_to_vec3(style_data.board_white_color));
        m_board_shader.setUniform("inverse_board", style_data.inverse_board);
        m_board_sprite.setSize({static_cast<float>(style_data.side_size * 8),
                                static_cast<float>(style_data.side_size * 8)});
    }
    else
    {
        if (m_style_data.side_size != style_data.side_size)
        {
            m_board_shader.setUniform("side_size", style_data.side_size);
            m_board_sprite.setSize({static_cast<float>(style_data.side_size * 8),
                                    static_cast<float>(style_data.side_size * 8)});
        }
        if (m_style_data.board_black_color != style_data.board_black_color)
        {
            m_board_shader.setUniform("black_color",
                                      convert_color_to_vec3(style_data.board_black_color));
        }
        if (m_style_data.board_white_color != style_data.board_white_color)
        {
            m_board_shader.setUniform("white_color",
                                      convert_color_to_vec3(style_data.board_white_color));
        }
        if (m_style_data.inverse_board != style_data.inverse_board)
        {
            m_board_shader.setUniform("inverse_board", style_data.inverse_board);
        }
    }
    process_texture_locations(style_data.white_pieces);
    process_texture_locations(style_data.black_pieces);

    m_style_data = style_data;
}

void ChessView::process_texture_locations(const StyleData::TextureLocations& new_locations)
{
    m_resource_manager.load_texture_from_file(new_locations.king_path);
    m_resource_manager.load_texture_from_file(new_locations.queen_path);
    m_resource_manager.load_texture_from_file(new_locations.rook_path);
    m_resource_manager.load_texture_from_file(new_locations.bishop_path);
    m_resource_manager.load_texture_from_file(new_locations.knight_path);
    m_resource_manager.load_texture_from_file(new_locations.pawn_path);
}

void ChessView::render(sf::RenderWindow& window)
{
    window.draw(m_board_sprite, &m_board_shader);
    for (const auto& mapped_sprite : m_pieces_sprites)
    {
        if (mapped_sprite.first != m_selected_piece)
        {
            window.draw(mapped_sprite.second);
        }
    }
    window.draw(m_pieces_sprites[m_selected_piece]);
}

void ChessView::update()
{
    m_pieces_sprites = generate_gui_board(m_style_data.white_pieces, m_style_data.black_pieces,
                                          m_resource_manager, m_board, m_style_data.inverse_board,
                                          m_style_data.side_size);
}
