#pragma once

#ifndef LEMONADE_GAME_SRC_INPUT_RESULT_H
#define LEMONADE_GAME_SRC_INPUT_RESULT_H

#include "action.h"

class InputHandler;
class GameState;
class Action;

struct InputResult
{
	enum class Type
	{
		None,
		Push_GameState,
		Pop_GameState,
		Reset_GameState,
		Push_InputHandler,
		Pop_InputHandler,
		Reset_InputHandler,
		Action,
	};

	Type type{ Type::None };

	GameState* next_game_state{ nullptr };
	InputHandler* next_input_handler{ nullptr };
	QueuedAction next_action{ nullptr };

	InputResult* next_input_result{ nullptr };

	InputResult() :
		type(Type::None), next_game_state(nullptr), next_input_handler(nullptr), next_action(nullptr), next_input_result(nullptr)
	{
	}

	InputResult(Type type, GameState* game_state) :
		type(type), next_game_state(game_state)
	{
	}

	InputResult(Type type, InputHandler* input_handler) :
		type(type), next_input_handler(input_handler)
	{
	}

	InputResult(Type type) :
		type(type), next_game_state(nullptr), next_input_handler(nullptr)
	{
	}

	InputResult(GameState* game_state) :
		type(Type::Push_GameState), next_game_state(game_state), next_input_handler(nullptr)
	{
	}

	InputResult(InputHandler* input_handler) :
		type(Type::Push_InputHandler), next_input_handler(input_handler)
	{
	}

	InputResult(std::unique_ptr<Action> action) :
		type(Type::Action), next_action(std::move(action))
	{
	}

	InputResult(Type type, GameState* game_state, InputResult* next) :
		type(type), next_game_state(game_state), next_input_result(next)
	{
	}

	InputResult(Type type, InputHandler* input_handler, InputResult* next) :
		type(type), next_input_handler(input_handler), next_input_result(next)
	{
	}

	InputResult(Type type, InputResult* next) :
		type(type), next_input_result(next)
	{
	}

	InputResult(GameState* game_state, InputResult* next) :
		type(Type::Push_GameState), next_game_state(game_state), next_input_result(next)
	{
	}

	InputResult(InputHandler* input_handler, InputResult* next) :
		type(Type::Push_InputHandler), next_input_handler(input_handler), next_input_result(next)
	{
	}

	InputResult(std::unique_ptr<Action> action, InputResult* next) :
		type(Type::Action), next_action(std::move(action)), next_input_result(next)
	{
	}
};

#endif // !LEMONADE_GAME_SRC_INPUT_RESULT_H
