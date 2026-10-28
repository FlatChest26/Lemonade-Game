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

void PlayerInputHandler::init()
{
	auto sidebar = get_world_state()->get_sidebar();

	if (!sidebar)
	{
		CERR("No sidebar on world state!");
		return;
	}

	if (!sidebar->has_menu("GAME_INFO_SIDEBAR"))
	{
		sidebar->add_menu("GAME_INFO_SIDEBAR", std::make_unique<SidebarMenu_GameInfo>());
	}

	if (!sidebar->has_menu("INVENTORY_SIDEBAR"))
	{
		m_inventory_menu = (SidebarMenu_Inventory*)sidebar->add_menu("INVENTORY_SIDEBAR", std::make_unique<SidebarMenu_Inventory>());
		m_inventory_menu->pin();
	}
	else
	{
		m_inventory_menu = (SidebarMenu_Inventory*)sidebar->get_menu("INVENTORY_SIDEBAR");
	}
}

void PlayerInputHandler::step(const TimeUnit& delta_time)
{
}

void PlayerInputHandler::render(const RenderParams& params) const
{
	output::draw_frame_rgb(
		params.world_render_frame.pos.x, params.world_render_frame.pos.y, 
		params.world_render_frame.pos.x + params.world_render_frame.size.x,
		params.world_render_frame.pos.y + params.world_render_frame.size.y,
		color::get("white"), color::get("black"), DEFAULT_RECT_FRAME, false, TCOD_BKGND_SET
	);
}

void PlayerLookHandler::init()
{
	PlayerInputHandler::init();

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
		world_state->set_camera_focus(m_look_focus);

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

void PlayerLookHandler::render(const RenderParams& params) const
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

	output::set_fg(draw_x + params.world_inner_frame_p1().x, draw_y + params.world_inner_frame_p1().x, draw_fg);
	output::set_bg(draw_x + params.world_inner_frame_p1().x, draw_y + params.world_inner_frame_p1().y, draw_bg);

	// Write info at look focus

	int write_x = params.world_inner_frame_p1().x;
	int write_y = params.world_inner_frame_p1().y;

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
	PlayerInputHandler::init();

	if (!get_player()->get_inventory())
	{
		CERR("Player has no inventory!");
	}

	if (get_player()->get_inventory()->empty())
	{
		m_inventory_menu->selected_slot = -1;
	}
	else if (m_inventory_menu->selected_slot == -1)
	{
		m_inventory_menu->selected_slot = 1;
	}

	m_inventory_menu->focus();
	m_inventory_menu->uncollapse();
}

void PlayerInventoryHandler::remove()
{
	m_inventory_menu->unfocus();
	m_inventory_menu->collapse();
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
			m_inventory_menu->selected_slot = -1;
		}
		else
		{
			m_inventory_menu->selected_slot += 1;
			m_inventory_menu->selected_slot = m_inventory_menu->selected_slot % inventory->get_item_count();
		}
	}
	else if (InputMap::is_action_pressed(Keybind::MenuDown, event))
	{
		if (inventory->empty())
		{
			m_inventory_menu->selected_slot = -1;
		}
		else
		{
			m_inventory_menu->selected_slot -= 1;
			m_inventory_menu->selected_slot = m_inventory_menu->selected_slot % inventory->get_item_count();
		}
	}
	else if (InputMap::is_action_pressed(Keybind::Drop, event))
	{
		auto selected_item = inventory->get_item(m_inventory_menu->selected_slot);
		
		if (selected_item)
		{
			return InputResult(std::make_unique<DropAction>(player->speed(), player, selected_item));;
		}
	}
	else if (InputMap::is_action_pressed(Keybind::Examine, event))
	{
		auto selected_item = inventory->get_item(m_inventory_menu->selected_slot);
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
	else if (InputMap::is_action_pressed(Keybind::PinSidebarMenu, event))
	{
		m_inventory_menu->set_pinned(!m_inventory_menu->is_pinned());
		return InputResult();
	}

	return InputResult();
}

void PlayerInventoryHandler::step(const TimeUnit& delta_time)
{
	PlayerInputHandler::step(delta_time);

	auto player = get_player();
	auto inventory = player->get_inventory();

	if (inventory->empty())
	{
		m_inventory_menu->selected_slot = -1;
	}
}