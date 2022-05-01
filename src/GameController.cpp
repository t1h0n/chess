#include <Board.hpp>
#include <ChessView.hpp>
#include <GameController.hpp>
#include <SFML/Graphics.hpp>
#include <functional>

namespace
{
constexpr std::uint32_t DEFAULT_BOARD_SIDE_SIZE = 800u;

constexpr SpecialMovesData DEFAULT_SPECIAL_MOVE_DATA_WHITE{Position{0, 0}, Position{7, 0},
                                                           std::nullopt};

constexpr SpecialMovesData DEFAULT_SPECIAL_MOVE_DATA_BLACK{Position{0, 7}, Position{7, 7},
                                                           std::nullopt};

const std::string DEFAULT_PIECES_LOCATION = "resources/textures/pieces/01_classic/";
const StyleData DEFAULT_STYLE_DATA{{
                                       DEFAULT_PIECES_LOCATION + "w-king.png",
                                       DEFAULT_PIECES_LOCATION + "w-queen.png",
                                       DEFAULT_PIECES_LOCATION + "w-rook.png",
                                       DEFAULT_PIECES_LOCATION + "w-bishop.png",
                                       DEFAULT_PIECES_LOCATION + "w-knight.png",
                                       DEFAULT_PIECES_LOCATION + "w-pawn.png",
                                   },
                                   {
                                       DEFAULT_PIECES_LOCATION + "b-king.png",
                                       DEFAULT_PIECES_LOCATION + "b-queen.png",
                                       DEFAULT_PIECES_LOCATION + "b-rook.png",
                                       DEFAULT_PIECES_LOCATION + "b-bishop.png",
                                       DEFAULT_PIECES_LOCATION + "b-knight.png",
                                       DEFAULT_PIECES_LOCATION + "b-pawn.png",
                                   },
                                   "resources/shaders/board/board.frag",
                                   static_cast<std::int32_t>(DEFAULT_BOARD_SIDE_SIZE) / 8,
                                   {250, 245, 228},
                                   {255, 99, 99},
                                   false};
const sf::ContextSettings DEAFULT_CONTEXT_SETTINGS{24, 8, 8, 3, 0};
}  // namespace

GameController::GameController()
    : m_style_data{DEFAULT_STYLE_DATA}
    , m_window{sf::VideoMode(DEFAULT_BOARD_SIDE_SIZE, DEFAULT_BOARD_SIDE_SIZE), "Chess",
               sf::Style::Titlebar | sf::Style::Close, DEAFULT_CONTEXT_SETTINGS}
{
    m_board = std::make_shared<Board>();
    m_view = std::make_shared<ChessView>(m_resource_manager, m_board, m_style_data);
    reset();
    // window
    init_event_handlers();
    m_window.setVerticalSyncEnabled(true);
}

void GameController::run()
{
    while (m_window.isOpen())
    {
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            handle_event(event);
        }
        m_window.clear();
        m_view->render(m_window);
        m_window.display();
    }
}

void GameController::init_event_handlers()
{
    m_event_handlers[sf::Event::EventType::Closed]
        = std::bind(&GameController::on_window_closed, this, std::placeholders::_1);
    m_event_handlers[sf::Event::EventType::MouseButtonPressed]
        = std::bind(&GameController::on_mouse_button_pressed, this, std::placeholders::_1);
    m_event_handlers[sf::Event::EventType::MouseButtonReleased]
        = std::bind(&GameController::on_mouse_button_released, this, std::placeholders::_1);
    m_event_handlers[sf::Event::EventType::MouseMoved]
        = std::bind(&GameController::on_mouse_moved, this, std::placeholders::_1);
}

void GameController::handle_event(const sf::Event& event)
{
    if (const auto event_handler_it = m_event_handlers.find(event.type);
        event_handler_it != m_event_handlers.end())
    {
        event_handler_it->second(event);
    }
}

void GameController::on_window_closed(const sf::Event& event)
{
    m_window.close();
}

void GameController::on_mouse_button_pressed(const sf::Event& event)
{
    if (event.mouseButton.button == sf::Mouse::Button::Left)  // piece got selected
    {
        m_selected_piece
            = map_mouse_position_to_piece_position(event.mouseButton.x, event.mouseButton.y);
        if (m_board->is_square_empty(*m_selected_piece)
            || m_board->get_piece_at_position(*m_selected_piece).get_color() != m_side_to_move)
        {
            m_selected_piece.reset();
            return;
        }
        m_view->select_piece(*m_selected_piece);
    }
    else
    {
        if (m_selected_piece)
        {
            m_view->unselect_piece();
            m_selected_piece.reset();
        }
    }
}

void GameController::on_mouse_button_released(const sf::Event& event)
{
    if (event.mouseButton.button == sf::Mouse::Button::Left)
    {
        m_view->unselect_piece();
        if (m_selected_piece)
        {
            try_make_move(*m_selected_piece, map_mouse_position_to_piece_position(
                                                 event.mouseButton.x, event.mouseButton.y));

            m_selected_piece.reset();
        }
    }
}

void GameController::on_mouse_moved(const sf::Event& event)
{
    if (m_selected_piece)
    {
        m_view->set_selected_piece_position(event.mouseMove.x, event.mouseMove.y);
    }
}

Position GameController::map_mouse_position_to_piece_position(std::int32_t x, std::int32_t y)
{
    return {x / m_style_data.side_size, m_style_data.inverse_board
                                            ? y / m_style_data.side_size
                                            : 7 - y / m_style_data.side_size};
}

bool GameController::is_castling_move(const Position& from,
                                      const Position& to,
                                      const SpecialMovesData& current_side) const
{
    if (from != *m_board->get_king_position(m_side_to_move))
    {
        return false;
    }
    if (!current_side.king_side_rook && !current_side.king_side_rook)
    {
        return false;
    }
    if (from.y != to.y)
    {
        return false;
    }
    if (std::abs(from.x - to.x) < 2)
    {
        return false;
    }
    return true;
}

void GameController::do_castling_move(const Position& rook_position,
                                      const Position& new_rook_position,
                                      const Position& new_king_position)
{
    auto rook = m_board->remove_piece(rook_position);
    auto king = m_board->remove_piece(*m_board->get_king_position(m_side_to_move));
    king->set_position(new_king_position);
    rook->set_position(new_rook_position);
    m_board->add_piece(std::move(rook));
    m_board->add_piece(std::move(king));
}

void GameController::try_make_move(const Position& from, const Position& to)
{
    // TODO: back up everything to make ip possible to undo moves
    m_reset_en_passant = true;
    auto& current_side = get_special_move_data(m_side_to_move);
    if (is_castling_move(from, to, current_side))
    {
        const auto& king_position = m_board->get_king_position(m_side_to_move);
        if (from.x > to.x && current_side.queen_side_rook)
        {
            const Position new_king_position{2, king_position->y};
            const Position new_rook_position{3, current_side.queen_side_rook->y};
            do_castling_move(*current_side.queen_side_rook, new_rook_position, new_king_position);
        }
        else if (from.x < to.x && current_side.king_side_rook)
        {
            const Position new_king_position{6, king_position->y};
            const Position new_rook_position{5, current_side.king_side_rook->y};
            do_castling_move(*current_side.king_side_rook, new_rook_position, new_king_position);
        }
        else
        {
            return;
        }
        current_side.queen_side_rook.reset();
        current_side.king_side_rook.reset();
        on_move_done();
        return;
    }
    const auto moves_from_square = m_available_moves.normal_moves.find(from);
    if (moves_from_square == m_available_moves.normal_moves.end())
    {
        return;
    }
    if (moves_from_square->second.find(to) == moves_from_square->second.end())
    {
        return;
    }
    const auto& piece_to_move = m_board->get_piece_at_position(from);
    if (piece_to_move.get_type() == PieceType::KING)
    {
        current_side.queen_side_rook.reset();
        current_side.king_side_rook.reset();
    }
    else if (piece_to_move.get_type() == PieceType::ROOK)
    {
        if (current_side.queen_side_rook
            && *current_side.queen_side_rook == piece_to_move.get_position())
        {
            current_side.queen_side_rook.reset();
        }
        else if (current_side.king_side_rook
                 && *current_side.king_side_rook == piece_to_move.get_position())
        {
            current_side.king_side_rook.reset();
        }
    }
    else if (piece_to_move.get_type() == PieceType::PAWN)
    {
        if (std::abs(from.y - to.y) == 2)
        {
            m_reset_en_passant = false;
            get_special_move_data(get_opposite_color(m_side_to_move)).en_passant_takable
                = {to.x, m_side_to_move == PieceColor::WHITE ? 2 : 5};
        }
        else if (current_side.en_passant_takable && *current_side.en_passant_takable == to)
        {
            m_board->remove_piece(
                {to.x, m_side_to_move == PieceColor::WHITE ? to.y - 1 : to.y + 1});
        }
    }
    if (!m_board->is_square_empty(to))
    {
        on_piece_captured(m_board->remove_piece(to));
    }
    auto moving_piece = m_board->remove_piece(from);
    moving_piece->set_position(to);
    m_board->add_piece(std::move(moving_piece));
    on_move_done();
}

void GameController::on_move_done()
{
    const auto squares_under_attack = generate_squares_under_attack(*m_board, m_side_to_move);
    m_side_to_move = get_opposite_color(m_side_to_move);
    if (m_reset_en_passant)
    {
        get_special_move_data(m_side_to_move).en_passant_takable.reset();
    }
    const bool is_under_check
        = squares_under_attack.count(*m_board->get_king_position(m_side_to_move));
    m_available_moves
        = generate_available_moves(*m_board, get_special_move_data(m_side_to_move), m_side_to_move);
    if (m_available_moves.empty())
    {
        if (is_under_check)
        {
            on_checkmate(m_side_to_move);
        }
        else
        {
            on_draw();
        }
    }
    else if (is_under_check)
    {
        on_check(m_side_to_move);
    }
    m_view->update();
}

void GameController::reset()
{
    create_default_board();
    m_side_to_move = PieceColor::WHITE;
    m_available_moves
        = generate_available_moves(*m_board, m_special_moves_data_white, PieceColor::WHITE);
    m_reset_en_passant = true;
    m_view->update();
}

void GameController::create_default_board()
{
    m_board->clear_board();
    // kings
    m_board->add_piece(std::make_unique<King>(PieceColor::WHITE, Position{4, 0}));
    m_board->add_piece(std::make_unique<King>(PieceColor::BLACK, Position{4, 7}));
    // queens
    m_board->add_piece(std::make_unique<Queen>(PieceColor::WHITE, Position{3, 0}));
    m_board->add_piece(std::make_unique<Queen>(PieceColor::BLACK, Position{3, 7}));
    // rooks
    m_board->add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{0, 0}));
    m_board->add_piece(std::make_unique<Rook>(PieceColor::WHITE, Position{7, 0}));
    m_board->add_piece(std::make_unique<Rook>(PieceColor::BLACK, Position{0, 7}));
    m_board->add_piece(std::make_unique<Rook>(PieceColor::BLACK, Position{7, 7}));
    // bishops
    m_board->add_piece(std::make_unique<Bishop>(PieceColor::WHITE, Position{2, 0}));
    m_board->add_piece(std::make_unique<Bishop>(PieceColor::WHITE, Position{5, 0}));
    m_board->add_piece(std::make_unique<Bishop>(PieceColor::BLACK, Position{2, 7}));
    m_board->add_piece(std::make_unique<Bishop>(PieceColor::BLACK, Position{5, 7}));
    // knights
    m_board->add_piece(std::make_unique<Knight>(PieceColor::WHITE, Position{1, 0}));
    m_board->add_piece(std::make_unique<Knight>(PieceColor::WHITE, Position{6, 0}));
    m_board->add_piece(std::make_unique<Knight>(PieceColor::BLACK, Position{1, 7}));
    m_board->add_piece(std::make_unique<Knight>(PieceColor::BLACK, Position{6, 7}));
    // pawns
    for (std::int32_t i = 0; i < 8; ++i)
    {
        m_board->add_piece(std::make_unique<Pawn>(PieceColor::WHITE, Position{i, 1}));
        m_board->add_piece(std::make_unique<Pawn>(PieceColor::BLACK, Position{i, 6}));
    }
    m_special_moves_data_white = DEFAULT_SPECIAL_MOVE_DATA_WHITE;
    m_special_moves_data_black = DEFAULT_SPECIAL_MOVE_DATA_BLACK;
}

SpecialMovesData& GameController::get_special_move_data(PieceColor side_to_move)
{
    return side_to_move == PieceColor::WHITE ? m_special_moves_data_white
                                             : m_special_moves_data_black;
}

void GameController::on_check(PieceColor side_under_check)
{
    std::cout << "check to " << to_c_str(side_under_check) << "\n";
}

void GameController::on_checkmate(PieceColor side_under_check)
{
    std::cout << "checkmate to " << to_c_str(side_under_check) << "\n";
}

void GameController::on_draw()
{
    std::cout << "draw\n";
}

void GameController::on_piece_captured(std::unique_ptr<Piece> captured_piece)
{
    if (captured_piece->get_type() == PieceType::ROOK)
    {
        auto& current_data = get_special_move_data(captured_piece->get_color());
        const auto update_special_move_data = [&captured_piece](auto& rook_data) {
            if (rook_data && *rook_data == captured_piece->get_position())
            {
                rook_data.reset();
            }
        };
        update_special_move_data(current_data.queen_side_rook);
        update_special_move_data(current_data.king_side_rook);
    }
    std::cout << "captured piece " << to_c_str(captured_piece->get_type()) << " "
              << to_c_str(captured_piece->get_color()) << "\n";
}
