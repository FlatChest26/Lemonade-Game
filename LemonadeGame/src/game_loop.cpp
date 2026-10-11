#include "game_loop.h"
#include "lemonade_game.h"

#include <memory>
#include <string>
#include <vector>

#include "game.h"
#include "world_state.h"

#include <libtcod/timer.hpp>

static constexpr const char* GAME_TITLE = "Lemonade Game";
static std::unique_ptr<Game> game;

namespace game_loop
{
	void new_game()
	{
		// Make new game
		game = std::make_unique<Game>();

		game->get_settings().fixed_camera = false;
		game->get_settings().show_explored_tiles = true;

		// Start new game on world state
		WorldGameState* world_state = new WorldGameState();

		// Make new world state and add it to the game
		game->handle_input_result(InputResult{ world_state });

		// Make new game
		if (world_state)
		{
			world_state->new_game();
		}
	}

	static void render()
	{
		if (!game->should_update_screen)
		{
			return;
		}

		output::clear_console();

		game->render();
		game->should_update_screen = false;

		output::present();
	}

	static void update(double delta_time)
	{
		game->update(delta_time);
	}

	static void handle_events()
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			output::convert_event(event);

			switch (event.type)
			{
			case SDL_QUIT:
			{
				game->stop_running();
				break;
			}
			case SDL_WINDOWEVENT:
			{
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_CLOSE:
				{
					game->stop_running();
					break;
				}

				case SDL_WINDOWEVENT_RESIZED:
				case SDL_WINDOWEVENT_SHOWN:
				case SDL_WINDOWEVENT_EXPOSED:
				case SDL_WINDOWEVENT_SIZE_CHANGED:
				case SDL_WINDOWEVENT_MOVED:
				case SDL_WINDOWEVENT_MAXIMIZED:
				case SDL_WINDOWEVENT_RESTORED:
				{
					output::update_window();
					game->request_screen_update();
				}
				}
				break;
			}
			}

			game->handle_event(event);
		}
	}

	bool run()
	{
		auto timer = tcod::Timer();

		game->is_running = true;

		while (game->is_running)
		{
			// Make sure to sync the timer to the target FPS
			auto delta_time = timer.sync(60);

			// Game loop

			handle_events();
			update(delta_time);
			render();

			// Update window title if needed
			if (game->should_update_window_title)
			{
				if (cfg::settings::SHOW_FPS)
				{
					output::set_window_title((std::string)GAME_TITLE + " | FPS: " + std::to_string(timer.get_mean_fps()));
				}
				else
				{
					output::set_window_title(GAME_TITLE);
					game->should_update_window_title = false;
				}
			}

			//if (cfg::settings::SHOW_FPS)
			//{
			//	output::console_printf(0, 0, "FPS: " + std::to_string(timer.get_mean_fps()));
			//}

			game->frame++;
		}

		return false;
	}
}