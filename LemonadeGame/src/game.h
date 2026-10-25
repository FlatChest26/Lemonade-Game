#pragma once

#ifndef LEMONADE_GAME_SRC_GAME_H
#define LEMONADE_GAME_SRC_GAME_H

#include "game_states.h"

struct GameSettings
{
	bool show_explored_tiles = true;
	bool fixed_camera = true; 
	bool instant_actions = true;
	bool use_second_person_pronouns = true;
};

/*
This class manages the upper level of application logic, including the game loop and game state management.
It is responsible for handling events, updating the game state, and rendering the current game state to the screen.

The Game class uses a stack of GameState objects to manage different states of the game, allowing for easy transitions between states such as
menus, gameplay, and pause screens.
*/

class Game : public std::enable_shared_from_this<Game>
{
private:
	std::vector<std::unique_ptr<GameState>> m_game_states;

public:
	bool is_running = false;
	bool should_update_window_title = true;
	bool should_update_screen = true;

	uint32_t frame = 0;

	GameSettings settings;

public:
	Game() {}
	virtual ~Game() {}

public:
	// -- Getters -- //

	GameState* get_current_game_state() const
	{
		if (m_game_states.empty())
		{
			return nullptr;
		}

		return m_game_states.back().get();
	}

	GameSettings& get_settings() { return settings; }

public:
	// -- Game State -- //

	void handle_input_result(InputResult result)
	{
		if (auto game_state = get_current_game_state())
		{
			game_state->handle_input_result(std::move(result));
		}

		switch (result.type)
		{
		case InputResult::Type::None:
		{
			return;
		}
		case InputResult::Type::Pop_GameState:
		{
			pop_game_state();

			return;
		}
		case InputResult::Type::Push_GameState:
		{
			push_game_state(result.next_game_state);

			return;
		}
		case InputResult::Type::Reset_GameState:
		{
			reset_game_states(result.next_game_state);

			return;
		}
		}
	}

private:

	void pop_game_state()
	{
		if (m_game_states.empty())
		{
			return;
		}

		m_game_states.pop_back();
	}

	void push_game_state(GameState* game_state)
	{
		if (!game_state)
			return;

		auto new_state = std::unique_ptr<GameState>(game_state);
		m_game_states.push_back(std::move(new_state));
		m_game_states.back()->m_game = this;
	}

	void reset_game_states(GameState* game_state)
	{
		m_game_states.clear();

		if (game_state)
		{
			push_game_state(game_state);
		}
	}

public:
	// -- Utility -- //

	void request_screen_update()
	{
		should_update_screen = true;
	}

	void stop_running()
	{
		is_running = false;
	}

	int frame_count() const { return frame; }

public:
	// -- Game Loop -- //

	void handle_event(SDL_Event event)
	{
		if (auto current_state = get_current_game_state())
		{
			handle_input_result(current_state->handle_event(event));
		}
	}

	void update(double delta_time) const
	{
		if (auto current_state = get_current_game_state())
		{
			current_state->update(delta_time);
		}
	}
	void render() const
	{
		if (auto current_state = get_current_game_state())
		{
			current_state->render();
		}
	}
};

#endif // !LEMONADE_GAME_SRC_GAME_H