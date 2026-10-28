#pragma once

#ifndef LEMONADE_ENGINE_SRC_INPUT_MAP_H
#define LEMONADE_ENGINE_SRC_INPUT_MAP_H

#include <SDL_events.h>
#include <SDL_keycode.h>
#include <unordered_map>
#include <cctype>
#include <algorithm>

#include <string>
#include <vector>

#include "transforms.h"

struct InputAction
{
	SDL_KeyCode sym;
	SDL_Keymod mod = KMOD_NONE;
	SDL_Keymod nmod = KMOD_NONE; // mods that must be OFF


	constexpr bool operator==(const InputAction& other) const
	{
		return sym == other.sym && mod == other.mod && nmod == other.nmod;
	}

	std::string to_string() const
	{
		std::string str = SDL_GetKeyName(sym);

		std::transform(str.begin(), str.end(), str.begin(),
			[&](unsigned char c)
			{
				if (mod & KMOD_SHIFT)
				{
					return std::toupper(c);
				}
				else
				{
					return std::tolower(c);
				}
			}
		);

		return str;
	}
};

template<typename T>
class GenericInputMap
{
	using ActionName = T;

protected:
	std::unordered_map<ActionName, std::vector<InputAction>> m_input_actions;
	std::unordered_map<ActionName, Position> m_input_direction;

private:
	GenericInputMap() {}

	~GenericInputMap() {}

	static GenericInputMap<T>& get()
	{
		static GenericInputMap<T> singleton;
		return singleton;
	}

public:

	static std::string actions_to_string(ActionName name)
	{
		std::string final_str;
		auto actions = get_actions(name);

		for (const auto& action : actions)
		{
			final_str += action.to_string();

			if (actions.size() > 1 && action != actions.back())
			{
				final_str += "/";
			}
		}

		return final_str;
	}

	static void add_input_action(ActionName name, SDL_KeyCode sym, SDL_Keymod mod = KMOD_NONE, SDL_Keymod nmod = KMOD_NONE)
	{
		if (!has_action(name))
		{
			get().m_input_actions[name] = {};
		}

		get().m_input_actions[name].push_back(InputAction(sym, mod, nmod));
	}

	static void add_input_direction(ActionName name, Position direction)
	{
		get().m_input_direction[name] = direction;
	}
	
	static std::vector<InputAction> get_actions(ActionName name)
	{
		if (!has_action(name))
		{
			return {};
		}

		return get().m_input_actions[name];
	}

	static bool has_action(ActionName name)
	{
		return get().m_input_actions.find(name) != get().m_input_actions.end();
	}

	static Position get_input_direction(SDL_Event event)
	{
		if (event.type != SDL_KEYDOWN)
		{
			return Position(0, 0, 0);
		}

		Position input_dir;

		for (auto [action_name, move_dir] : get().m_input_direction)
		{
			if (is_action_pressed(action_name, event))
			{
				input_dir += move_dir;
			}
		}

		return input_dir;
	}


	static bool is_action(ActionName name, SDL_Event event)
	{
		if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP)
		{
			return false;
		}

		if (!has_action(name))
		{
			return false;
		}

		auto sym = event.key.keysym.sym;
		auto mod = event.key.keysym.mod;

		for (const auto& action : get_actions(name))
		{
			bool mod_on = action.mod != KMOD_NONE ? mod & action.mod : true;
			bool mod_off = action.nmod != KMOD_NONE ? !(mod & action.nmod) : true;
			bool sym_on = action.sym == sym;

			if (mod_on && mod_off && sym_on)
				return true;
		}

		return false;
	}
	
	static bool is_action_pressed(ActionName name, SDL_Event event)
	{
		if (event.type != SDL_KEYDOWN)
		{
			return false;
		}

		return is_action(name, event) && event.key.state & SDL_PRESSED;
	}

	static bool is_action_just_pressed(ActionName name, SDL_Event event)
	{
		return is_action_pressed(name, event) && event.key.type == SDL_KEYDOWN;
	}

	static bool is_action_released(ActionName name, SDL_Event event)
	{
		if (event.type != SDL_KEYUP)
		{
			return false;
		}

		return is_action(name, event) && event.key.state & SDL_RELEASED;
	}

	static bool is_action_just_released(ActionName name, SDL_Event event)
	{
		return is_action_released(name, event) && event.key.type == SDL_KEYUP;
	}
};


#endif // !LEMONADE_ENGINE_SRC_INPUT_MAP_H
