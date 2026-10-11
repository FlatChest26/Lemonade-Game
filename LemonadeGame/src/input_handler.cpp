#include "input_handler.h"

#include "game_states.h"
#include "game.h"
#include "world_state.h"

Game* InputHandler::get_game() const
{
	if (auto game_state = get_game_state())
	{
		return game_state->get_game();
	}

	return {};
}

GameState* InputHandler::get_game_state() const
{
	return m_game_state;
}

WorldGameState* InputHandler::get_world_state() const
{
	if (auto game_state = get_game_state())
	{
		if (game_state->is_world_game_state())
		{
			return game_state->as_world_game_state();
		}
	}

	return {};
}

Entity* InputHandler::get_player() const
{
	if (auto world_state = get_world_state())
	{
		return world_state->get_player();
	}

	return nullptr;
}

World* InputHandler::get_world() const
{
	if (auto world_state = get_world_state())
	{
		return world_state->get_world();
	}

	return nullptr;
}

Camera* InputHandler::get_camera() const
{
	if (auto world_state = get_world_state())
	{
		return world_state->get_camera();
	}

	return nullptr;
}

void InputHandler::set_game_state(GameState* game_state)
{
	m_game_state = game_state;
}

bool InputHandler::is_top_level() const
{
	if (m_game_state)
	{
		return m_game_state->get_current_input_handler() == this;
	}

	return false;
}