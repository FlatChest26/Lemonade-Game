
#pragma once

#ifndef LEMONADE_GAME_SRC_ACTION_OPTION_H
#define LEMONADE_GAME_SRC_ACTION_OPTION_H

#include <functional>
#include <string>

#include "keybinds.h"

class Action;
class Interaction;
class Entity;

struct ActionOption
{
	std::string name;
	Keybind input_action;
	std::function<Action*()> action;
};

struct InteractionOption
{
	std::string name;
	Keybind input_action;
	std::function<Interaction*(Entity*, Entity*)> action;
};

#endif // !LEMONADE_GAME_SRC_ACTION_OPTION_H