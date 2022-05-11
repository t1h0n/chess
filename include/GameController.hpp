#pragma once

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "ChessView.hpp"
#include "MoveGenerator.hpp"
#include "ResourceManager.hpp"

class Board;
class ChessView;

class GameController
{
public:
    GameController();

    void run();

private:
    void reset();
    void create_default_board();
    bool is_castling_move(const Position& from,
                          const Position& to,
                          const SpecialMovesData& current_side) const;
    void do_castling_move(const Position& rook_position,
                          const Position& new_rook_position,
                          const Position& new_king_position);

    void on_piece_captured(std::unique_ptr<Piece> captured_piece_type);
    void on_move_done();
    void on_check(PieceColor side_under_check);
    void on_checkmate(PieceColor side_under_check);
    void on_draw();

    SpecialMovesData& get_special_move_data(PieceColor side_to_move);

private:  // ui
    void handle_event(const sf::Event& event);
    void try_make_move(const Position& from, const Position& to);
    Position map_mouse_position_to_piece_position(std::int32_t x, std::int32_t y);

private:
    // ui event handlers
    void init_event_handlers();
    void on_window_closed(const sf::Event& event);
    void on_mouse_button_pressed(const sf::Event& event);
    void on_mouse_button_released(const sf::Event& event);
    void on_mouse_moved(const sf::Event& event);

private:
    sf::RenderWindow m_window;

    // view
    StyleData m_style_data;
    std::shared_ptr<ChessView> m_view;
    ResourceManager m_resource_manager;
    std::optional<Position> m_selected_piece;
    std::map<sf::Event::EventType, std::function<void(const sf::Event&)>> m_event_handlers;

    // game logic
    std::shared_ptr<Board> m_board;
    PieceColor m_side_to_move;
    bool m_reset_en_passant{true};
    SpecialMovesData m_special_moves_data_white;
    SpecialMovesData m_special_moves_data_black;
    AvailableMoves m_available_moves;
};