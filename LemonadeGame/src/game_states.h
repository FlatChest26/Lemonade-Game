#pragma once

#ifndef LEMONADE_GAME_SRC_GAME_STATES_H
#define LEMONADE_GAME_SRC_GAME_STATES_H

#include "action.h"
#include "input_handler.h"
#include "lemonade_game.h"
#include "render_params.h"
#include "renderer.h"
#include "snowy_macros.h"

#include <memory>
#include <utility>

class WorldGameState;
struct GameSettings;

class GameState : public std::enable_shared_from_this<GameState>
{
	friend class Game;

protected:
	Game* m_game;

	std::unique_ptr<Renderer> m_renderer;
	std::vector<std::unique_ptr<InputHandler>> m_input_handlers;

public:

	// -- Getters -- //

	Game* get_game() const { return m_game; }
	GameSettings& get_game_settings() const;

	Renderer* get_renderer() const { return m_renderer.get(); }

	InputHandler* get_current_input_handler() const
	{
		if (m_input_handlers.empty())
		{
			return nullptr;
		}

		return m_input_handlers.back().get();
	}

	// -- Checks -- //

	virtual constexpr bool is_world_game_state() const { return false; }
	virtual constexpr WorldGameState* as_world_game_state() const { return nullptr; }

	// -- Game Loop -- //

	virtual void update(double delta_time);
	virtual void render() const;

	// -- Input Handling -- //

	virtual InputResult handle_event(SDL_Event event);

	virtual void handle_action(std::unique_ptr<Action> action)
	{
		CERR("This game state does not handle actions.");
	}

	virtual void handle_input_result(InputResult result);

protected:

	void pop_input_handler();
	void push_input_handler(InputHandler* input_handler);
	void reset_input_handlers(InputHandler* input_handler);
};

#endif // !LEMONADE_GAME_SRC_GAME_STATES_H