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


enum class FurnitureFlag : uint32_t
{
	NONE = 0,

	IS_BLOCKING = 1 << 0,
	IS_OPAQUE = 1 << 1,
};

IS_FLAG_ENUM(FurnitureFlag);

inline std::map<std::string, FurnitureFlag> AllFurnitureFlags
{
	{ "is_blocking",	FurnitureFlag::IS_BLOCKING },
	{ "is_opaque",		FurnitureFlag::IS_OPAQUE },
};

struct FurnitureTile
{
	Position position;
	Renderable renderable;

	static RawObjectChecker* get_checker();
	static FurnitureTile from_json(const json& j);
};

struct FurnitureDefinition
{
	std::string ID;

	Size size;
	std::vector<FurnitureTile> tiles;

	RawMaster::Ref<ItemDefinition> item_definition;

	double sleep_quality{ 1.0f };

	FurnitureFlag flags;

	Renderable renderable_at(Coord x, Coord y, Coord z) const;

	static RawObjectChecker* get_checker();
	static FurnitureDefinition from_json(const json& j);

public:
	bool is_blocking() const { return flags & FurnitureFlag::IS_BLOCKING; }
	bool is_opaque() const { return flags & FurnitureFlag::IS_OPAQUE; }
};

#endif // !LEMONADE_GAME_SRC_FURNITURE_DEF_H
