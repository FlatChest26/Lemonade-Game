#pragma once

#ifndef LEMONADE_GAME_SRC_INPUT_HANDLER_H
#define LEMONADE_GAME_SRC_INPUT_HANDLER_H

#include <SDL_events.h>
#include "time_units.h"
#include "render_params.h"

#include "action.h"

#define DEF_HANDLER_EVENT(x) virtual InputResult ev_##x( SDL_Event event )
#define OVERRIDE_HANDLER_EVENT(x) virtual InputResult ev_##x( SDL_Event event ) override

class InputHandler;
class GameState;
class WorldGameState;
class Game;
class Camera;
class World;
class Entity;

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
	std::unique_ptr<Action> next_action{ nullptr };

	InputResult() :
		type(Type::None), next_game_state(nullptr), next_input_handler(nullptr), next_action(nullptr)
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
};

class InputHandler
{
	friend class GameState;

protected:
	GameState* m_game_state{ nullptr };

	InputResult dispatch(SDL_Event event)
	{
		switch (event.type)
		{
		case SDL_QUIT:
			return ev_quit(event);

		case SDL_DISPLAYEVENT:
			return ev_display_event(event);
		case SDL_WINDOWEVENT:
			return ev_window_event(event);
		case SDL_SYSWMEVENT:
			return ev_syswm_event(event);

		case SDL_KEYDOWN:
			return ev_key_down(event);
		case SDL_KEYUP:
			return ev_key_up(event);

		case SDL_MOUSEMOTION:
			return ev_mouse_motion(event);
		case SDL_MOUSEBUTTONDOWN:
			return ev_mouse_button_down(event);
		case SDL_MOUSEBUTTONUP:
			return ev_mouse_button_up(event);
		case SDL_MOUSEWHEEL:
			return ev_mouse_wheel(event);
		}

		return InputResult{};
	}

public:

	InputHandler() = default;

public:

	// -- Getters & Setters -- //

	Game* get_game() const;

	GameState* get_game_state() const;
	WorldGameState* get_world_state() const;
	Entity* get_player() const;
	World* get_world() const;
	Camera* get_camera() const;

	virtual void set_game_state(GameState* game_state);

	virtual constexpr bool allow_input_pass_through() const { return false; }
	virtual bool is_top_level() const;

	// -- Utilities -- //

	virtual InputResult handle_event(SDL_Event event) { return dispatch(event); }

	virtual void init() {}
	virtual void update(double delta_time) {}
	virtual void step(TimeUnit delta_time) {}
	virtual void render(RenderParams params) const {}

protected:
	DEF_HANDLER_EVENT(quit) { return InputResult(); }

	DEF_HANDLER_EVENT(key_down) { return InputResult(); }
	DEF_HANDLER_EVENT(key_up) { return InputResult(); }

	DEF_HANDLER_EVENT(display_event) { return InputResult(); }
	DEF_HANDLER_EVENT(window_event) { return InputResult(); }
	DEF_HANDLER_EVENT(syswm_event) { return InputResult(); }

	DEF_HANDLER_EVENT(mouse_motion) { return InputResult(); }
	DEF_HANDLER_EVENT(mouse_button_down) { return InputResult(); }
	DEF_HANDLER_EVENT(mouse_button_up) { return InputResult(); }
	DEF_HANDLER_EVENT(mouse_wheel) { return InputResult(); }
};

#endif // !LEMONADE_GAME_SRC_INPUT_HANDLER_H