#pragma once

#ifndef LEMONADE_GAME_SRC_CONSUMABLE_H
#define LEMONADE_GAME_SRC_CONSUMABLE_H

#include "living.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct ConsumableEffect
{
	StatID modified_stat{ needs::HUNGER };
	PosNegValue amount{ -5 };
	std::string taste{ "like something unknown" };

	static std::shared_ptr<RawObjectChecker> get_checker();
	static ConsumableEffect from_json(const json& j);
};


#endif // !LEMONADE_GAME_SRC_CONSUMABLE_H
