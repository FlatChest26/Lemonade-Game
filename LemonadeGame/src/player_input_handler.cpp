#include "player_input_handler.h"

#include "action.h"
#include "direction_action.h"
#include "game.h"

#include "world_state.h"

#include "tile_map.h"

#include "render_params.h"

#include <SDL_keycode.h>
#include <map>

#include "transforms.h"

const std::map<SDL_KeyCode, Position> MOVE_KEYS
{
	{ SDLK_UP,		{0, -1, 0} },
	{ SDLK_DOWN,	{0, 1, 0} },
	{ SDLK_LEFT,	{-1, 0, 0} },
	{ SDLK_RIGHT,	{1, 0, 0} },
	{ SDLK_KP_1,	{-1, 1, 0} },
	{ SDLK_KP_2,	{0, 1, 0} },
	{ SDLK_KP_3,	{1, 1, 0} },
	{ SDLK_KP_4,	{-1, 0, 0} },
	{ SDLK_KP_6,	{1, 0, 0} },
	{ SDLK_KP_7,	{-1, -1, 0} },
	{ SDLK_KP_8,	{0, -1, 0} },
	{ SDLK_KP_9,	{1, -1, 0} }
};

InputResult PlayerInputHandler::ev_key_down(SDL_Event event)
{
	auto player = get_player();

	if (!player)
	{
		CERR("No player found.");
		return InputResult();
	}

	auto scancode = event.key.keysym.scancode;
	auto sym = event.key.keysym.sym;
	auto mod = event.key.keysym.mod;

	if (mod & KMOD_SHIFT)
	{
		switch (sym)
		{
		case SDLK_PERIOD:
		{
			//player->queue_action(std::make_unique<BumpAction>(player, (Coord)0, (Coord)0, (Coord)-1));

			return InputResult(std::make_unique<BumpAction>(player, (Coord)0, (Coord)0, (Coord)-1));
		}
		case SDLK_COMMA:
		{
			//player->queue_action(std::make_unique<BumpAction>(player, (Coord)0, (Coord)0, (Coord)1));

			return InputResult(std::make_unique<BumpAction>(player, (Coord)0, (Coord)0, (Coord)1));
		}
		}
	}

	switch (sym)
	{
	case SDLK_ESCAPE:
	{
		if (auto game = get_game())
		{
			game->stop_running();
		}

		break;
	}
	case SDLK_F1:
	{
		if (auto world_state = get_world_state())
		{
			world_state->set_tick_speed(TickSpeed::TURN_BASED);
		}

		break;
	}
	case SDLK_F2:
	{
		if (auto world_state = get_world_state())
		{
			world_state->set_tick_speed(TickSpeed::REAL_TIME);
		}

		break;
	}
	case SDLK_F3:
	{
		if (auto world_state = get_world_state())
		{
			world_state->set_tick_speed(TickSpeed::REAL_TIME_2X);
		}

		break;
	}
	case SDLK_F4:
	{
		if (auto world_state = get_world_state())
		{
			world_state->set_tick_speed(TickSpeed::REAL_TIME_4X);
		}

		break;
	}
	case SDLK_F5:
	{
		COUT(player->pos());

		break;
	}
	case SDLK_PERIOD: case SDLK_KP_5:
	{
		//player->queue_action(std::make_unique<WaitAction>(TimeUnit::from_seconds(1.0), player));
		return InputResult(std::make_unique<WaitAction>(TimeUnit::from_seconds(1.0), player));
	}
	case SDLK_COMMA:
	{
		//player->queue_action(std::make_unique<WaitAction>(TimeUnit::from_seconds(10.0), player));
		return InputResult(std::make_unique<WaitAction>(TimeUnit::from_seconds(10.0), player));
	}
	case SDLK_x:
	{
		return InputResult(new PlayerLookHandler());
	}
	}

	for (auto [move_key, move_dir] : MOVE_KEYS)
	{
		if (sym == move_key)
		{
			Coord dz = 0;

			if (mod & KMOD_SHIFT)
			{
				dz = 1;
			}

			if (mod & KMOD_CTRL)
			{
				dz = -1;
			}

			//player->queue_action(std::make_unique<BumpAction>(player, move_dir.x, move_dir.y, move_dir.z + dz));
			return InputResult(std::make_unique<BumpAction>(player, move_dir.x, move_dir.y, move_dir.z + dz));
		}
	}

	return InputResult();
}

void PlayerLookHandler::init()
{
	auto player = get_player();

	if (!player)
	{
		CERR("No player found.");
		return;
	}

	m_look_focus = player->pos();
}

void PlayerLookHandler::step(TimeUnit delta_time)
{
	auto game_settings = get_world_state()->get_game_settings();

	if (game_settings.fixed_camera)
	{
		get_world_state()->update_camera(Position(m_look_focus));
	}
	else
	{
		get_world_state()->update_camera(m_look_focus);
	}
}

InputResult PlayerLookHandler::ev_key_down(SDL_Event event)
{
	auto player = get_player();

	if (!player)
	{
		CERR("No player found.");
		return InputResult();
	}

	auto scancode = event.key.keysym.scancode;
	auto sym = event.key.keysym.sym;
	auto mod = event.key.keysym.mod;

	switch (sym)
	{
	case SDLK_ESCAPE:case SDLK_x:
	{
		return InputResult(InputResult::Type::Pop_InputHandler);
	}
	case SDLK_r:
	{
		auto tile = get_world()->get_tile(m_look_focus);
		COUT(tile->get_display_name());
		return InputResult(InputResult::Type::Action);
	}
	}

	if (mod & KMOD_SHIFT)
	{
		switch (sym)
		{
		case SDLK_PERIOD:
		{
			m_look_focus += Position(0, 0, -1);
			return InputResult(InputResult::Type::Action);
		}
		case SDLK_COMMA:
		{
			m_look_focus += Position(0, 0, 1);
			return InputResult(InputResult::Type::Action);
		}
		}
	}

	for (auto [move_key, move_dir] : MOVE_KEYS)
	{
		if (sym == move_key)
		{
			Coord dz = 0;

			if (mod & KMOD_SHIFT)
			{
				dz = 1;
			}

			if (mod & KMOD_CTRL)
			{
				dz = -1;
			}

			m_look_focus += Position(move_dir.x, move_dir.y, dz);

			return InputResult(InputResult::Type::Action);
		}
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

	output::set_fg(draw_x, draw_y, tile->bg());
	output::set_bg(draw_x, draw_y, tile->fg());

	// Write info at look focus

	int write_y = 0;

	if (tile->get_type())
	{
		output::console_printf_rgb(0, write_y++, tile->fg(), tile->bg(), tile->get_display_name());

		if (tile->is_walkable()) output::console_printf_rgb(0, write_y++, tile->fg(), tile->bg(), ">Is Walkable");
		if (tile->is_blocked()) output::console_printf_rgb(0, write_y++, tile->fg(), tile->bg(), ">Is Blocked");

		if (tile->is_transparent()) output::console_printf_rgb(0, write_y++, tile->fg(), tile->bg(), ">Is Transparent");
		if (tile->is_opaque())  output::console_printf_rgb(0, write_y++, tile->fg(), tile->bg(), ">Is Opaque");

		if (tile->has_floor())  output::console_printf_rgb(0, write_y++, tile->fg(), tile->bg(), ">Has Floor");
		if (tile->has_ceiling())  output::console_printf_rgb(0, write_y++, tile->fg(), tile->bg(), ">Has Ceiling");

		if (tile->is_air())  output::console_printf_rgb(0, write_y++, tile->fg(), tile->bg(), ">Is Air");

		if (tile->can_ascend())  output::console_printf_rgb(0, write_y++, tile->fg(), tile->bg(), ">Can Ascend");
		if (tile->can_descend())  output::console_printf_rgb(0, write_y++, tile->fg(), tile->bg(), ">Can Descend");

		if (tile->can_open())  output::console_printf_rgb(0, write_y++, tile->fg(), tile->bg(), ">Can Open");
		if (tile->can_close())  output::console_printf_rgb(0, write_y++, tile->fg(), tile->bg(), ">Can Close");
	}

	for (const auto& entity : get_world()->get_entities_at(m_look_focus))
	{
		output::console_printf_rgb(
			0, write_y++, entity->fg(), entity->bg(), entity->get_display_name() + (entity == get_player() ? " (You)" : "")
		);
	}
}