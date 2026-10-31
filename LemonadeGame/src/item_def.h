#pragma once

#ifndef LEMONADE_GAME_SRC_ITEM_DEF_H
#define LEMONADE_GAME_SRC_ITEM_DEF_H

#include "renderable.h"
#include "snowy_grammar.h"
#include "currency.h"

#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct ItemDefinition
{
	std::string ID;
	Noun noun;
	Renderable renderable;

	std::string description; 

	Currency price{ Currency::none() };
	size_t carry_slots{ 1 };

	static std::shared_ptr<RawObjectChecker> get_checker();
	static ItemDefinition from_json(const json& j);
};

#endif // !LEMONADE_GAME_SRC_ITEM_DEF_H
