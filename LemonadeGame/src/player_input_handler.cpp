#include "player_input_handler.h"

#include "action.h"
#include "direction_action.h"
#include "location_actions.h"
#include "misc_actions.h"
#include "game.h"

#include "world_state.h"

#include "tile_map.h"

#include "render_params.h"

#include <SDL_keycode.h>
#include <map>

#include "transforms.h"

InputResult PlayerInputHandler::ev_key_down(SDL_Event event)
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

	// Check if movement keys are pressed
	Position move_dir = InputMap::get_input_direction(event);

	if (!move_dir.empty())
	{
		return InputResult(std::make_unique<BumpAction>(player, move_dir.unit()));
	}

	// Check action keys

	if (InputMap::is_action_pressed(Keybind::Quit, event))
	{
		game->stop_running();
		return InputResult();
	}
	else if (InputMap::is_action_pressed(Keybind::Look, event))
	{
		return InputResult(new PlayerLookHandler());
	}
	else if (InputMap::is_action_pressed(Keybind::Inventory, event))
	{
		return InputResult(new PlayerInventoryHandler());
	}
	else if (InputMap::is_action_pressed(Keybind::Open, event))
	{
		for (auto p : transforms::get_surrounding_locations(player->get_movable()->pos()))
		{
			if (world->get_tile(p)->can_open())
			{
				return InputResult(std::make_unique<OpenAction>(player->speed(), player, p));
			}
		}
	}
	else if (InputMap::is_action_pressed(Keybind::Close, event))
	{
		for (auto p : transforms::get_surrounding_locations(player->get_movable()->pos()))
		{
			if (world->get_tile(p)->can_close())
			{
				return InputResult(std::make_unique<CloseAction>(player->speed(), player, p));
			}
		}
	}
	else if (InputMap::is_action_pressed(Keybind::WaitShort, event))
	{
		return InputResult(std::make_unique<WaitAction>(TimeUnit::from_seconds(1.0)));
	}
	else if (InputMap::is_action_pressed(Keybind::WaitLong, event))
	{
		return InputResult(std::make_unique<WaitAction>(TimeUnit::from_seconds(10.0)));
	}
	
	else if (InputMap::is_action_pressed(Keybind::Grab, event))
	{
		auto entities_at = world->get_entities_at(player->get_movable()->pos());

		for (auto& item : entities_at)
		{
			if (item == player)
				continue;

			if (item->can_be_held())
			{
				return InputResult(std::make_unique<GrabAction>(player->speed(), player, item));
			}
		}
	}
	else if (InputMap::is_action_pressed(Keybind::Drop, event))
	{
		if (auto inventory = player->get_inventory())
		{
			if (inventory->get_item_count() == 1)
			{
				for (auto& item : inventory->get_items())
				{
					return InputResult(std::make_unique<DropAction>(player->speed(), player, item));
				}
			}
			else if (inventory->get_item_count() > 1)
			{
				return InputResult(new PlayerInventoryHandler());
			}
		}
		else
		{
			CERR("Player has no inventory!");
		}
	}
	else if (InputMap::is_action_pressed(Keybind::SetToTurnBasedSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::TURN_BASED);
		return InputResult();
	}
	else if (InputMap::is_action_pressed(Keybind::SetToRealTimeSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::REAL_TIME);
		return InputResult();
	}
	else if (InputMap::is_action_pressed(Keybind::SetToRealTime2xSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::REAL_TIME_2X);

		return InputResult();
	}
	else if (InputMap::is_action_pressed(Keybind::SetToRealTime4xSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::REAL_TIME_4X);
		return InputResult();
	}

#ifdef _DEBUG
	auto scancode = event.key.keysym.scancode;
	auto sym = event.key.keysym.sym;
	auto mod = event.key.keysym.mod;

	switch (sym)
	{
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


void PlayerInputHandler::render(RenderParams params) const
{
	output::draw_frame_rgb(
		0, 0, params.camera->get_width() + 2, params.camera->get_height() + 2,
		color::get("white"), color::get("black"), DEFAULT_RECT_FRAME, false, TCOD_BKGND_SET
	);

	if (auto world_state = get_world_state())
	{
		switch (world_state->get_tick_speed())
		{
		case TickSpeed::TURN_BASED:
			output::console_printf_ex(0, 0, DEFAULT_BKGND_FLAG, TCOD_alignment_t::TCOD_RIGHT, "Game Speed: Turn-based");
			break;
		case TickSpeed::REAL_TIME:
			output::console_printf_ex(0, 0, DEFAULT_BKGND_FLAG, TCOD_alignment_t::TCOD_RIGHT, "Game Speed: x1");
			break;
		case TickSpeed::REAL_TIME_2X:
			output::console_printf_ex(0, 0, DEFAULT_BKGND_FLAG, TCOD_alignment_t::TCOD_RIGHT, "Game Speed: x2");
			break;
		case TickSpeed::REAL_TIME_4X:
			output::console_printf_ex(0, 0, DEFAULT_BKGND_FLAG, TCOD_alignment_t::TCOD_RIGHT, "Game Speed: x4");
			break;
		case TickSpeed::REAL_TIME_8X:
			output::console_printf_ex(0, 0, DEFAULT_BKGND_FLAG, TCOD_alignment_t::TCOD_RIGHT, "Game Speed: x8");
			break;
		}
	}

	if (auto world = get_world())
	{
		output::console_printf_ex(0, 1, DEFAULT_BKGND_FLAG, TCOD_alignment_t::TCOD_RIGHT, world->get_time().time_point.str_12_hr());
		output::console_printf_ex(0, 2, DEFAULT_BKGND_FLAG, TCOD_alignment_t::TCOD_RIGHT, world->get_time().str_fancy(true));
	}
}

void PlayerLookHandler::init()
{
	auto player = get_player();

	if (!player)
	{
		CERR("No player found.");
		return;
	}

	if (!player->has_movable())
	{
		CERR("Player does not have a movable!");
		return;
	}

	m_look_focus = player->get_movable()->pos();
	get_world_state()->set_camera_focus(m_look_focus);
}

void PlayerLookHandler::remove()
{
	if (auto world_state = get_world_state())
	{
		world_state->remove_camera_focus();
	}
}

void PlayerLookHandler::step(TimeUnit delta_time)
{
	if (auto world_state = get_world_state())
	{
		world_state->set_camera_focus(m_look_focus);
	}
}

InputResult PlayerLookHandler::ev_key_down(SDL_Event event)
{
	auto player = get_player();
	auto world_state = get_world_state();

	// Check if movement keys are pressed
	Position move_dir = InputMap::get_input_direction(event);

	if (!move_dir.empty())
	{
		m_look_focus += move_dir.unit();

		m_look_focus.x = std::clamp(m_look_focus.x, 0, (Coord)get_world()->get_tile_map()->get_width());
		m_look_focus.y = std::clamp(m_look_focus.y, 0, (Coord)get_world()->get_tile_map()->get_height());
		m_look_focus.z = std::clamp(m_look_focus.z, 0, (Coord)get_world()->get_tile_map()->get_depth());

		return InputResult(InputResult::Type::Action);
	}

	if (InputMap::is_action_pressed(Keybind::Look, event) || InputMap::is_action_pressed(Keybind::Quit, event))
	{
		return InputResult(InputResult::Type::Pop_InputHandler);
	}
	else if (InputMap::is_action_pressed(Keybind::Examine, event))
	{
		auto tile = get_world()->get_tile(m_look_focus);
		get_world_state()->game_message(GameMessage(tile->description()));

		for (const auto& e : get_world()->get_entities_at(m_look_focus))
		{
			get_world_state()->game_message(GameMessage(e->description()));
		}

		return InputResult(InputResult::Type::Action);
	}
	else if (InputMap::is_action_pressed(Keybind::SetToTurnBasedSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::TURN_BASED);
		return InputResult();
	}
	else if (InputMap::is_action_pressed(Keybind::SetToRealTimeSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::REAL_TIME);
		return InputResult();
	}
	else if (InputMap::is_action_pressed(Keybind::SetToRealTime2xSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::REAL_TIME_2X);

		return InputResult();
	}
	else if (InputMap::is_action_pressed(Keybind::SetToRealTime4xSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::REAL_TIME_4X);
		return InputResult();
	}

	return InputResult();
}

void PlayerLookHandler::render(RenderParams params) const
{
	auto tile = get_world()->get_tile(m_look_focus);

	int draw_x{ m_look_focus.x }, draw_y{ m_look_focus.y }, draw_z{ 0 };

	if (params.camera)
	{
		if (!params.camera->in_view(m_look_focus.x, m_look_focus.y, m_look_focus.z))
			return;

		std::tie(draw_x, draw_y, draw_z) = params.camera->get_screen(m_look_focus.x, m_look_focus.y, m_look_focus.z).as_tuple();
	}

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

	output::set_fg(draw_x + params.world_inner_frame().x, draw_y + params.world_inner_frame().x, draw_fg);
	output::set_bg(draw_x + params.world_inner_frame().x, draw_y + params.world_inner_frame().y, draw_bg);

	// Write info at look focus

	int write_x = params.world_inner_frame().x;
	int write_y = params.world_inner_frame().y;

	if (tile->get_type())
	{
		if (!params.use_fov || tile->is_visible())
		{
			output::console_printf_rgb(write_x, write_y++, tile->fg(), tile->bg(), tile->get_display_name());
		}
		else if (tile->is_explored())
		{
			output::console_printf_rgb(write_x, write_y++, color::get("gray"), color::get("black"), tile->get_display_name() + " (remembered)");
		}
		else
		{
			output::console_printf_rgb(write_x, write_y++, color::get("gray"), color::get("black"), "unexplored");
		}
	}

	for (const auto& entity : get_world()->get_entities_at(m_look_focus))
	{
		output::console_printf_rgb(
			write_x, write_y++, entity->fg(), entity->bg(), entity->display_name() + (entity == get_player() ? " (You)" : "")
		);
	}
}

void PlayerInventoryHandler::init()
{
	if (!get_player()->get_inventory())
	{
		CERR("Player has no inventory!");
	}

	m_selected_slot = -1;
}

InputResult PlayerInventoryHandler::ev_key_down(SDL_Event event)
{
	auto player = get_player();
	auto inventory = player->get_inventory();
	auto world_state = get_world_state();

	if (InputMap::is_action_pressed(Keybind::Inventory, event) || InputMap::is_action_pressed(Keybind::Quit, event))
	{
		return InputResult(InputResult::Type::Pop_InputHandler);
	}
	else if (InputMap::is_action_pressed(Keybind::MenuUp, event))
	{
		if (inventory->empty())
		{
			m_selected_slot = -1;
		}
		else
		{
			m_selected_slot += 1;
			m_selected_slot = m_selected_slot % inventory->get_item_count();
		}
	}
	else if (InputMap::is_action_pressed(Keybind::MenuDown, event))
	{
		if (inventory->empty())
		{
			m_selected_slot = -1;
		}
		else
		{
			m_selected_slot -= 1;
			m_selected_slot = m_selected_slot % inventory->get_item_count();
		}
	}
	else if (InputMap::is_action_pressed(Keybind::Drop, event))
	{
		auto selected_item = inventory->get_item(m_selected_slot);
		
		if (selected_item)
		{
			return InputResult(std::make_unique<DropAction>(player->speed(), player, selected_item));;
		}
	}
	else if (InputMap::is_action_pressed(Keybind::Examine, event))
	{
		auto selected_item = inventory->get_item(m_selected_slot);
		if (selected_item)
		{
			get_world_state()->game_message(GameMessage(selected_item->description()));
			return InputResult(InputResult::Type::Action);
		}
	}
	else if (InputMap::is_action_pressed(Keybind::SetToTurnBasedSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::TURN_BASED);
		return InputResult();
	}
	else if (InputMap::is_action_pressed(Keybind::SetToRealTimeSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::REAL_TIME);
		return InputResult();
	}
	else if (InputMap::is_action_pressed(Keybind::SetToRealTime2xSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::REAL_TIME_2X);

		return InputResult();
	}
	else if (InputMap::is_action_pressed(Keybind::SetToRealTime4xSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::REAL_TIME_4X);
		return InputResult();
	}

	return InputResult();
}

using InventoryListTuple = std::tuple<std::string, color_t, color_t, bool>;

void PlayerInventoryHandler::render(RenderParams params) const
{
	auto player = get_player();
	auto inventory = player->get_inventory();

	int panel_width = (int)(std::string("Inventory").size() + 6);
	int panel_height = (int)std::max(size_t(4), inventory->get_item_count());

	std::vector<InventoryListTuple> inventory_list;

	if (inventory->get_item_count() < 1)
	{
		inventory_list.push_back(std::tuple("Empty", color::get("white"), color::get("black"), false));
	}
	else
	{
		int slot = 0;
		for (const auto& item : inventory->get_items())
		{
			inventory_list.push_back(std::tuple(snowy::grammar::a(item->display_noun()), item->fg(), item->bg(), m_selected_slot == slot));
			slot++;
		}
	}

	for (const auto& inv_list_item : inventory_list)
	{
		panel_width = (int)std::max(size_t(panel_width), std::get<0>(inv_list_item).size() + 2);
	}

	int panel_x = (params.camera->get_width() / 2) - (panel_width / 2) + params.world_inner_frame().x;
	int panel_y = (params.camera->get_height() / 2) - (panel_height / 2) + params.world_inner_frame().y;

	color_t fg = color::get("white");
	color_t bg = color::get("black");

	output::draw_frame_rgb(panel_x, panel_y, panel_width, panel_height, fg, bg, DEFAULT_RECT_FRAME, true, TCOD_BKGND_SET);
	output::console_printf_rgb_rect_ex(panel_x, panel_y, panel_width, 1, bg, fg, TCOD_BKGND_SET, TCOD_alignment_t::TCOD_CENTER, "Inventory");

	int draw_x = panel_x + 1;
	int draw_y = panel_y + 2;

	for (const auto& inv_list_item : inventory_list)
	{
		bool selected = std::get<3>(inv_list_item);
		color_t fg = selected ? std::get<2>(inv_list_item) : std::get<1>(inv_list_item);
		color_t bg = selected ? std::get<1>(inv_list_item) : std::get<2>(inv_list_item);

		draw_y += output::console_printf_rgb_rect_ex(
			draw_x, draw_y, panel_width - 2, panel_height - 2, fg, bg, TCOD_BKGND_SET, TCOD_CENTER, std::get<0>(inv_list_item)
		);
	}
}

void PlayerInventoryHandler::step(TimeUnit delta_time)
{
	auto player = get_player();
	auto inventory = player->get_inventory();

	if (inventory->empty())
	{
		m_selected_slot = -1;
	}
}
