#pragma once

#ifndef LEMONADE_GAME_SRC_TILE_TYPE_H
#define LEMONADE_GAME_SRC_TILE_TYPE_H

#include <cstdint>
#include <string>
#include <color.h>
#include <memory>
#include <map>

#include "enum_traits.h"
#include "snowy_macros.h"
#include "raw_object_checker.h"
#include "snowy_grammar.h"

#include "thing.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

inline const size_t DECORATION_MASK[17] = {
	0,
	6,
	6,
	6,
	5,
	4,
	2,
	8,
	5,
	3,
	1,
	7,
	5,
	9,
	10,
	11,
	0
};

using TileTexture = std::vector<Renderable>;

enum class TileFlag : uint64_t
{
	NONE = 0,

	IS_WALKABLE = 1 << 0,
	IS_BLOCKED = ~IS_WALKABLE,

	IS_TRANSPARENT = 1 << 1,
	IS_OPAQUE = ~IS_TRANSPARENT,

	HAS_FLOOR = 1 << 2,
	HAS_CEILING = 1 << 3,

	CAN_ASCEND = 1 << 4,
	CAN_DESCEND = 1 << 5,

	IS_AIR = 1 << 6,

	CAN_OPEN = 1 << 7,
	CAN_CLOSE = 1 << 8,

	OPENS_NEIGHBORS = 1 << 9,
	CLOSES_NEIGHBORS = 1 << 10,

	HAS_TRANSPARENT_FLOOR = 1 << 11,
	HAS_TRANSPARENT_CEILING = 1 << 12,
};

inline std::map<std::string, TileFlag> AllTileFlags
{
	{ "NONE",						TileFlag::NONE },
	{ "IS_WALKABLE",				TileFlag::IS_WALKABLE },
	{ "IS_BLOCKED",					TileFlag::IS_BLOCKED },
	{ "IS_TRANSPARENT",				TileFlag::IS_TRANSPARENT },
	{ "IS_OPAQUE",					TileFlag::IS_OPAQUE },
	{ "HAS_FLOOR",					TileFlag::HAS_FLOOR },
	{ "HAS_CEILING",				TileFlag::HAS_CEILING },
	{ "CAN_ASCEND",					TileFlag::CAN_ASCEND },
	{ "CAN_DESCEND",				TileFlag::CAN_DESCEND },
	{ "IS_AIR",						TileFlag::IS_AIR },
	{ "CAN_OPEN",					TileFlag::CAN_OPEN },
	{ "CAN_CLOSE",					TileFlag::CAN_CLOSE },
	{ "OPENS_NEIGHBORS",			TileFlag::OPENS_NEIGHBORS },
	{ "CLOSES_NEIGHBORS",			TileFlag::CLOSES_NEIGHBORS },
	{ "HAS_TRANSPARENT_FLOOR",		TileFlag::HAS_TRANSPARENT_FLOOR },
	{ "HAS_TRANSPARENT_CEILING",	TileFlag::HAS_TRANSPARENT_CEILING },
};

IS_FLAG_ENUM(TileFlag);

struct TileType
{
	ThingID ID;
	std::string description{ "" };

	Noun noun{ "air", "air" };

	TileTexture texture{ { ' ', color::get("cyan"), color::get("black") } };

	TileFlag flags{ 0 };

	bool random_texture{ false };
	bool wall_texture{ false };

	std::vector<ThingID> tags{};
	std::vector<ThingID> connects_with{};

	ThingID transforms_to{ "" };

	double move_cost{ 1.0f };

	static std::shared_ptr<RawObjectChecker> get_checker();
	static TileType from_json(const json& j);
};

namespace tile_types
{
	inline TileType VoidTile
	{
		.ID = "TILE_VOID",
		.noun = { "void", "void" },
		.texture = { { ' ', color::get("red"), color::get("black") } },
		.flags = TileFlag::IS_TRANSPARENT | TileFlag::HAS_FLOOR | TileFlag::HAS_CEILING,
		.tags = { "VOID" },
		.move_cost = 0.0f
	};
}

#endif // !LEMONADE_GAME_SRC_TILE_TYPE_H