#include "player_input_handler.h"

#include "action.h"
#include "direction_action.h"
#include "location_actions.h"
#include "interactions.h"
#include "special_actions.h"

#include "game.h"

#include "world_state.h"

#include "tile_map.h"

#include "render_params.h"

#include <SDL_keycode.h>
#include <map>

#include "transforms.h"

template<typename T>
T* InputHandler_Player::register_sidebar(const SidebarMenuID& ID)
{
	auto sidebar = get_world_state()->get_sidebar();

	if (!sidebar->has_menu(ID))
	{
		return (T*)sidebar->add_menu(ID, std::make_unique<T>());
	}
	else
	{
		return (T*)sidebar->get_menu(ID);
	}

	return nullptr;
}

void InputHandler_Player::init()
{
	auto sidebar = get_world_state()->get_sidebar();

	if (!sidebar)
	{
		CERR("No sidebar on world state!");
		return;
	}

	auto player = get_player();

	m_game_info_menu = register_sidebar<SidebarMenu_GameInfo>(sidebar_menus::GAME_INFO);

	if (player)
	{
		m_person_info_menu = register_sidebar<SidebarMenu_PersonInfo>(sidebar_menus::PERSON_INFO);
		m_person_info_menu->entity = player;
	}


	if (player)
	{
		m_needs_menu = register_sidebar<SidebarMenu_Needs>(sidebar_menus::NEEDS);
		m_needs_menu->entity = player;
	}

	if (player)
	{
		if (!sidebar->has_menu(sidebar_menus::INVENTORY))
		{
			m_inventory_menu = register_sidebar<SidebarMenu_Inventory>(sidebar_menus::INVENTORY);
			m_inventory_menu->pin();
		}

		m_inventory_menu = register_sidebar<SidebarMenu_Inventory>(sidebar_menus::INVENTORY);
		m_inventory_menu->entity = player;

		m_inventory_menu->collapse();
	}

	if (!sidebar->has_menu(sidebar_menus::LOOK))
	{
		m_look_menu = register_sidebar<SidebarMenu_Look>(sidebar_menus::LOOK);
		m_look_menu->hide();
	}

	m_look_menu = register_sidebar<SidebarMenu_Look>(sidebar_menus::LOOK);

	if (player)
	{
		m_look_menu->look_position = player->get_movable()->pos();
		m_look_menu->follow_player = true;
	}

	m_examine_menu = register_sidebar<SidebarMenu_Examine>(sidebar_menus::EXAMINE);
	m_examine_menu->set_visible(!m_examine_menu->examined_entities.empty() || m_examine_menu->examined_tile != nullptr);

	m_interaction_menu = register_sidebar<SidebarMenu_Interaction>(sidebar_menus::INTERACTION);
}

InputResult InputHandler_Player::ev_key_down(SDL_Event event)
{
	auto player = get_player();

	if (!player)
	{
		CERR("No player found.");
		return InputResult();
	}

	auto world = get_world();

	if (!world)
	{
		CERR("No world found.");
		return InputResult();
	}

	auto world_state = get_world_state();

	if (!world_state)
	{
		CERR("No world state found.");
		return InputResult();
	}

	auto game = get_game();

	if (!game)
	{
		CERR("No game found.");
		return InputResult();
	}

	bool is_alive = false;
	bool is_sleeping = false;

	if (player->is_person())
	{
		is_sleeping = player->get_person()->is_asleep();
		is_alive = player->get_person()->is_alive();
	}

	bool can_act = !is_sleeping && is_alive;

	// Check if movement keys are pressed
	Position move_dir = InputMap::get_input_direction(event);

	if (can_act && !move_dir.empty())
	{
		return InputResult(std::make_unique<BumpAction>(player, move_dir.unit()));
	}

	// Check action keys

	if (InputMap::is_action_pressed(Keybind::Quit, event))
	{
		game->stop_running();
		return InputResult();
	}
	
	if (can_act && InputMap::is_action_pressed(Keybind::Look, event))
	{
		return InputResult(new InputHandler_Look());
	}
	
	if (can_act && InputMap::is_action_pressed(Keybind::Inventory, event))
	{
		return InputResult(new InputHandler_Inventory());
	}
	
	if (can_act && InputMap::is_action_pressed(Keybind::Open, event))
	{
		std::set<Position> open_positions;
		std::unordered_map<Position, Tile*> added_tiles;

		for (auto p : transforms::get_surrounding_locations(player->get_movable()->pos()))
		{
			if (!world->get_tile(p)->can_open())
				continue;

			auto tile = world->get_tile(p);
			added_tiles[p] = tile;

			bool is_connection = false;

			for (auto& [other_pos, other_tile] : added_tiles)
			{
				if (other_tile == tile)
					continue;

				if (transforms::is_adjacent(p, other_pos) && (other_tile->can_connect_with(tile) || tile->can_connect_with(other_tile)))
				{
					is_connection = true;
					break;
				}
			}

			if (!is_connection)
				open_positions.insert(p);
		}

		if (open_positions.size() == 1)
		{
			return InputResult(std::make_unique<OpenAction>(player->interact_speed(), player, *open_positions.begin()));
		}
		else if (open_positions.size() > 1)
		{
			auto callback = [](InputHandler_Player* input_handler, const Position& pos)
				{
					return InputResult(std::make_unique<OpenAction>(input_handler->get_player()->interact_speed(), input_handler->get_player(), pos));
				};

			return InputResult(new InputHandler_PositionSelector({
				.center = player->get_movable()->pos(),
				.callback = callback,
				.valid_positions = open_positions,
				.title = "Open where?",
				.input_keybind = Keybind::Open
			}));
		}

		return InputResult();
	}
	
	if (can_act && InputMap::is_action_pressed(Keybind::Close, event))
	{
		std::set<Position> close_positions;
		std::unordered_map<Position, Tile*> added_tiles;

		for (auto p : transforms::get_surrounding_locations(player->get_movable()->pos()))
		{
			if (!world->get_tile(p)->can_close())
				continue;

			auto tile = world->get_tile(p);
			added_tiles[p] = tile;

			bool is_connection = false;

			for (auto& [other_pos, other_tile] : added_tiles)
			{
				if (other_tile == tile)
					continue;

				if (transforms::is_adjacent(p, other_pos) && (other_tile->can_connect_with(tile) || tile->can_connect_with(other_tile)))
				{
					is_connection = true;
					break;
				}
			}

			if (!is_connection)
				close_positions.insert(p);
		}

		if (close_positions.size() == 1)
		{
			return InputResult(std::make_unique<CloseAction>(player->interact_speed(), player, *close_positions.begin()));
		}
		else if (close_positions.size() > 1)
		{
			auto callback = [](InputHandler_Player* input_handler, const Position& pos)
				{
					return InputResult(std::make_unique<CloseAction>(input_handler->get_player()->interact_speed(), input_handler->get_player(), pos));
				};

			return InputResult(new InputHandler_PositionSelector({
				.center = player->get_movable()->pos(), 
				.callback = callback, 
				.valid_positions = close_positions,
				.title = "Close where?",
				.input_keybind = Keybind::Close
			}));
		}

		return InputResult();
	}
	
	if (InputMap::is_action_pressed(Keybind::Sleep, event))
	{
		if (player->get_person()->is_awake())
		{
			if (!player->get_person()->can_sleep())
			{
				world_state->game_message(GameMessage("You're not tired enough to sleep."));
				return InputResult();
			}

			return InputResult(std::make_unique<StartSleepAction>(player->interact_speed(), player));
		}
		else
		{
			if (!player->get_person()->can_wake_up())
			{
				world_state->game_message(GameMessage("You're too tired to wake up."));
				return InputResult();
			}

			return InputResult(std::make_unique<EndSleepAction>(player->interact_speed(), player));
		}
	}
	
	if (InputMap::is_action_pressed(Keybind::WaitShort, event))
	{
		return InputResult(std::make_unique<WaitAction>(TimeUnit::from_seconds(1.0)));
	}
	
	if (InputMap::is_action_pressed(Keybind::WaitLong, event))
	{
		return InputResult(std::make_unique<WaitAction>(TimeUnit::from_seconds(10.0)));
	}
	
	if (can_act && InputMap::is_action_pressed(Keybind::Grab, event))
	{
		auto inventory = player->get_inventory();

		if (!inventory)
		{
			CERR("player has no inventory!");
			return InputResult();
		}

		if (inventory->is_full())
		{
			world_state->game_message(GameMessage("Your inventory is full.", color::get("light_red")));
			return InputResult();
		}

		auto items_at = world->get_entities_at(
			player->get_movable()->pos(), 
			EntityFilter{ 
				.ignore_entities = { player }, 
				.required_flags = EntityFlag::CAN_BE_HELD 
			}
		);

		if (items_at.empty())
		{
			return InputResult();
		}

		for (auto& item : items_at)
		{
			if (inventory->can_carry(item))
			{
				return InputResult(std::make_unique<GrabAction>(player->interact_speed(), player, item));
			}
		}

		world_state->game_message(GameMessage("You don't have the free space to carry that.", color::get("light_red")));

		return InputResult();
	}

	if (InputMap::is_action_pressed(Keybind::Quit, event))
	{
		game->stop_running();
		return InputResult();
	}

	if (InputMap::is_action_pressed(Keybind::SetToTurnBasedSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::TURN_BASED);
		return InputResult();
	}

	if (InputMap::is_action_pressed(Keybind::SetToRealTimeSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::REAL_TIME);
		return InputResult();
	}

	if (InputMap::is_action_pressed(Keybind::SetToRealTime2xSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::REAL_TIME_2X);

		return InputResult();
	}

	if (InputMap::is_action_pressed(Keybind::SetToRealTime4xSpeed, event))
	{
		if (player->is_person() && player->get_person()->is_asleep())
		{
			world_state->set_tick_speed(TickSpeed::REAL_TIME_8X);
		}
		else
		{
			world_state->set_tick_speed(TickSpeed::REAL_TIME_4X);
		}
		return InputResult();
	}

#ifdef _DEBUG
	auto scancode = event.key.keysym.scancode;
	auto sym = event.key.keysym.sym;
	auto mod = event.key.keysym.mod;

	switch (sym)
	{
	case SDLK_F5:
	{
		if (player->has_movable())
		{
			COUT(player->get_movable()->pos());
		}
		break;
	}
	case SDLK_F6:
	{
		if (player->has_movable())
		{
			for (auto& entity : world->get_entities())
			{
				if (entity->has_brain())
				{
					auto new_goal = std::make_unique<Behavior_MoveTo>(player->get_movable()->pos());
					entity->get_brain()->add_behavior(std::move(new_goal));
				}
			}
		}
		break;
	}
	}
#endif // _DEBUG
	
	return InputResult();
}

void InputHandler_Player::step(const TimeUnit& delta_time)
{
	if (m_look_menu && get_player() && m_look_menu->follow_player)
	{
		m_look_menu->look_position = get_player()->get_movable()->pos();
	}
}

void InputHandler_Player::render(const RenderParams& params) const
{
	output::draw_frame_rgb(
		params.world_render_frame.pos.x, params.world_render_frame.pos.y,
		params.world_render_frame.pos.x + params.world_render_frame.size.x,
		params.world_render_frame.pos.y + params.world_render_frame.size.y,
		color::get("white"), color::get("black"), DEFAULT_RECT_FRAME, false, TCOD_BKGND_SET
	);
}

void InputHandler_Look::init()
{
	InputHandler_Player::init();

	m_look_menu->show();
	m_look_menu->focus();
	m_look_menu->follow_player = false;
	get_world_state()->set_camera_focus(m_look_menu->look_position);

	if (m_examine_menu->is_visible())
	{
		m_examine_menu->focus();
	}
}

void InputHandler_Look::remove()
{
	if (auto world_state = get_world_state())
	{
		world_state->remove_camera_focus();
	}

	m_examine_menu->examined_entities.clear();
	m_examine_menu->examined_tile = nullptr;

	m_look_menu->follow_player = true;
	m_look_menu->unfocus();
	m_look_menu->hide();
}

InputResult InputHandler_Look::ev_key_down(SDL_Event event)
{
	auto player = get_player();
	auto world_state = get_world_state();
	auto world = get_world();

	bool is_alive = false;
	bool is_sleeping = false;

	if (player->is_person())
	{
		is_sleeping = player->get_person()->is_asleep();
		is_alive = player->get_person()->is_alive();
	}

	// Check if movement keys are pressed
	Position move_dir = InputMap::get_input_direction(event);

	if (!move_dir.empty())
	{
		m_look_menu->look_position += move_dir.unit();
		world_state->set_camera_focus(m_look_menu->look_position);
		return InputResult();
	}

	if (InputMap::is_action_pressed(Keybind::Look, event) || InputMap::is_action_pressed(Keybind::Quit, event))
	{
		return InputResult(InputResult::Type::Pop_InputHandler);
	}
	
	if (InputMap::is_action_pressed(Keybind::Inventory, event))
	{
		return InputResult(InputResult::Type::Pop_InputHandler, new InputResult(new InputHandler_Inventory()));
	}
	
	if (InputMap::is_action_pressed(Keybind::Examine, event))
	{
		m_examine_menu->examined_entities.clear();
		m_examine_menu->examined_tile = nullptr;
		m_examine_menu->hide();

		for (const auto& e : world->get_entities_at(m_look_menu->look_position))
		{
			m_examine_menu->examined_entities.push_back(e);
			m_examine_menu->show();
			m_examine_menu->focus();
		}

		if (m_examine_menu->examined_entities.empty())
		{
			auto tile = world->get_tile(m_look_menu->look_position);

			if (tile)
			{
				m_examine_menu->examined_tile = tile;
				m_examine_menu->show();
				m_examine_menu->focus();
			}
		}

		return InputResult();
	}

	return InputHandler_Player::ev_key_down(event);
}

void InputHandler_Look::render(const RenderParams& params) const
{
	int draw_x{ m_look_menu->look_position.x }, draw_y{ m_look_menu->look_position.y }, draw_z{ m_look_menu->look_position.z };

	if (params.camera)
	{
		if (!params.camera->in_view(m_look_menu->look_position))
			return;

		std::tie(draw_x, draw_y, draw_z) = params.camera->get_screen(m_look_menu->look_position).as_tuple();
	}

	if (draw_z != 0)
		return;

	auto tile = get_world()->get_tile(m_look_menu->look_position);

	auto draw_fg = tile->bg();
	auto draw_bg = tile->fg();

	if (params.use_fov)
	{
		if (tile->is_visible())
		{
			draw_fg = tile->bg();
			draw_bg = tile->fg();
		}
		else if (tile->is_explored())
		{
			draw_fg = color::get("black");
			draw_bg = color::get("col_explored");
		}
		else
		{
			draw_fg = color::get("white");
			draw_bg = color::get("white");
		}
	}

	output::set_fg(draw_x + params.world_inner_frame_p1().x, draw_y + params.world_inner_frame_p1().x, draw_fg);
	output::set_bg(draw_x + params.world_inner_frame_p1().x, draw_y + params.world_inner_frame_p1().y, draw_bg);
}

void InputHandler_Inventory::init()
{
	InputHandler_Player::init();

	auto inventory = get_player()->get_inventory();

	if (!inventory)
	{
		CERR("Player has no inventory!");
	}

	m_inventory_menu->selected_slot = std::clamp(m_inventory_menu->selected_slot, 0, (int)inventory->get_max_slots());

	m_inventory_menu->focus();
	m_inventory_menu->uncollapse();
}

void InputHandler_Inventory::remove()
{
	m_examine_menu->examined_entities.clear();
	m_examine_menu->examined_tile = nullptr;

	m_inventory_menu->unfocus();
	m_inventory_menu->collapse();
}

InputResult InputHandler_Inventory::ev_key_down(SDL_Event event)
{
	auto player = get_player();
	auto inventory = player->get_inventory();
	auto world_state = get_world_state();

	bool is_alive = false;
	bool is_sleeping = false;

	if (player->is_person())
	{
		is_sleeping = player->get_person()->is_asleep();
		is_alive = player->get_person()->is_alive();
	}

	if (InputMap::is_action_pressed(Keybind::Inventory, event) || InputMap::is_action_pressed(Keybind::Quit, event))
	{
		return InputResult(InputResult::Type::Pop_InputHandler);
	}
	
	if (InputMap::is_action_pressed(Keybind::Look, event))
	{
		return InputResult(InputResult::Type::Pop_InputHandler, new InputResult(new InputHandler_Look()));
	}
	
	if (InputMap::is_action_pressed(Keybind::MenuUp, event))
	{
		m_inventory_menu->selected_slot += 1;
		m_inventory_menu->selected_slot = m_inventory_menu->selected_slot % inventory->get_max_slots();
		return InputResult();
	}
	
	if (InputMap::is_action_pressed(Keybind::MenuDown, event))
	{
		m_inventory_menu->selected_slot -= 1;
		m_inventory_menu->selected_slot = m_inventory_menu->selected_slot % inventory->get_max_slots();

		return InputResult();
	}
	
	if (!is_sleeping && is_alive && InputMap::is_action_pressed(Keybind::Drop, event))
	{
		if (auto selected_item = inventory->get_item(m_inventory_menu->selected_slot))
		{
			return InputResult(std::make_unique<DropAction>(player->interact_speed(), player, selected_item), new InputResult(InputResult::Type::Pop_InputHandler));
		}

		return InputResult();
	}
	
	if (InputMap::is_action_pressed(Keybind::Examine, event))
	{
		if (auto selected_item = inventory->get_item(m_inventory_menu->selected_slot))
		{
			m_examine_menu->examined_entities.clear();
			m_examine_menu->examined_entities.push_back(selected_item);
			m_examine_menu->show();
			m_examine_menu->focus();
		}

		return InputResult();
	}
	
	if (InputMap::is_action_pressed(Keybind::PinSidebarMenu, event))
	{
		m_inventory_menu->set_pinned(!m_inventory_menu->is_pinned());
		return InputResult();
	}

	return InputHandler_Player::ev_key_down(event);
}

void InputHandler_Inventory::step(const TimeUnit& delta_time)
{
	InputHandler_Player::step(delta_time);

	auto player = get_player();
	auto inventory = player->get_inventory();
}

void InputHandler_Interaction::init()
{
	InputHandler_Player::init();

}

void InputHandler_Interaction::remove()
{

}

InputResult InputHandler_Interaction::ev_key_down(SDL_Event event)
{
	return InputHandler_Player::ev_key_down(event);
}

InputResult InputHandler_PositionSelector::ev_key_down(SDL_Event event)
{
	Position move_dir = InputMap::get_input_direction(event);

	if (!move_dir.empty())
	{
		if (!m_valid_positions.empty() && m_valid_positions.contains(move_dir + m_center))
		{
			return InputResult(InputResult::Type::Pop_InputHandler);
		}

		auto result = m_callback(this, move_dir + m_center);
		result.next_input_result = new InputResult(InputResult::Type::Pop_InputHandler);
		return result;
	}

	if (InputMap::is_action_pressed(m_input_keybind, event) || InputMap::is_action_pressed(Keybind::Quit, event))
	{
		return InputResult(InputResult::Type::Pop_InputHandler);
	}

	return InputHandler_Player::ev_key_down(event);
}

void InputHandler_PositionSelector::render(const RenderParams& params) const
{
	for (auto& pos : m_valid_positions)
	{
		int draw_x{ pos.x }, draw_y{ pos.y }, draw_z{ pos.z };

		if (params.camera)
		{
			if (!params.camera->in_view(pos))
				return;

			std::tie(draw_x, draw_y, draw_z) = params.camera->get_screen(pos).as_tuple();
		}

		if (draw_z != 0)
			continue;

		auto tile = get_world()->get_tile(pos);

		auto draw_fg = tile->bg();
		auto draw_bg = tile->fg();

		if (params.use_fov)
		{
			if (tile->is_visible())
			{
				draw_fg = tile->bg();
				draw_bg = tile->fg();
			}
			else if (tile->is_explored())
			{
				draw_fg = color::get("black");
				draw_bg = color::get("col_explored");
			}
			else
			{
				draw_fg = color::get("white");
				draw_bg = color::get("white");
			}
		}

		output::set_fg(draw_x + params.world_inner_frame_p1().x, draw_y + params.world_inner_frame_p1().x, draw_fg);
		output::set_bg(draw_x + params.world_inner_frame_p1().x, draw_y + params.world_inner_frame_p1().y, draw_bg);
	}

	output::console_printf_rgb_rect_ex(
		params.world_inner_frame_p1().x, params.world_inner_frame_p1().y, params.world_inner_frame_size().x, params.world_inner_frame_size().y,
		color::get("white"), color::get("black"),
		TCOD_BKGND_SET, TCOD_CENTER, m_title
	);
}