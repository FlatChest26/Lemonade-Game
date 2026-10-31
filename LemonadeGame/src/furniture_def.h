#pragma once

#ifndef LEMONADE_GAME_SRC_FURNITURE_DEF_H
#define LEMONADE_GAME_SRC_FURNITURE_DEF_H

#include "renderable.h"
#include "snowy_grammar.h"
#include "currency.h"
#include "item_def.h"

#include "transforms.h"

#include "raw_master.h"

#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct FurnitureTile
{
	Position position;
	Renderable renderable;

	static std::shared_ptr<RawObjectChecker> get_checker();
	static FurnitureTile from_json(const json& j);
};

struct FurnitureDefinition
{
	std::string ID;

	Size size;
	std::vector<FurnitureTile> tiles;

	RawMaster::Ref<ItemDefinition> item_definition;

	bool is_blocking = true;

	Renderable get_renderable_at(Coord x, Coord y, Coord z) const;

	static std::shared_ptr<RawObjectChecker> get_checker();
	static FurnitureDefinition from_json(const json& j);
};

#endif // !LEMONADE_GAME_SRC_FURNITURE_DEF_H
