#pragma once

#ifndef LEMONADE_GAME_SRC_TILE_TYPE_H
#define LEMONADE_GAME_SRC_TILE_TYPE_H

#include <cstdint>
#include <string>
#include <color.h>
#include <memory>
#include <map>

#include "enum_traits.h"
#include <snowy_database.h>
#include <snowy_macros.h>
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

class TileType : public Thing
{
public:
	enum class Flag : uint64_t
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

		IS_HALF_TILE = 1 << 11,
	};

protected:
	ThingID m_ID;

	Noun m_noun;
	Pronouns m_pronouns;

	std::string m_description{ "" };

	TileTexture m_texture{};
	TileType::Flag m_flags{ TileType::Flag::NONE };

	bool m_random_texture{ false };
	bool m_wall_texture{ false };

	std::vector<ThingID> m_tags{};
	std::vector<ThingID> m_connects_with{};

	ThingID m_transforms_to{ "" };

public:
	struct initializer {
		ThingID ID;
		std::string description{ "" };

		Noun noun{ "air", "air" };
		Pronouns pronouns{ pronouns::IT_ITS };

		TileTexture texture{ { ' ', color::get("cyan"), color::get("black") } };

		TileType::Flag flags{ 0 };

		bool random_texture{ false };
		bool wall_texture{ false };

		std::vector<ThingID> tags{};
		std::vector<ThingID> connects_with{};

		ThingID transforms_to{ "" };
	};

public:

	TileType(const initializer& init = {});
	static TileType from_json(const json& j);

	constexpr bool is_tile_type() const override { return true; }
	constexpr TileType* as_tile_type() override { return this; }

	// -- Getters & Setters -- //

	virtual ThingID ID() const override { return m_ID; }

	virtual constexpr Noun get_noun() const override { return m_noun; }
	virtual constexpr Pronouns get_pronouns() const override { return m_pronouns; }
	virtual std::string description() const override { return m_description; }

	virtual TileTexture& get_texture() { return m_texture; }
	virtual const TileTexture& get_texture() const { return m_texture; }

	virtual TileType::Flag tile_flags() const { return m_flags; }

	virtual bool has_random_texture() const { return m_random_texture; }
	virtual bool has_wall_texture() const { return m_wall_texture; }

	virtual std::vector<ThingID>& get_tags() { return m_tags; }
	virtual const std::vector<ThingID>& get_tags() const { return m_tags; }

	virtual std::vector<ThingID>& get_connect_with() { return m_connects_with; }
	virtual const std::vector<ThingID>& get_connect_with() const { return m_connects_with; }

	virtual ThingID get_transform_ID() const { return m_transforms_to; }
};

inline std::map<std::string, TileType::Flag> AllTileFlags
{
	{ "NONE",				TileType::Flag::NONE				},
	{ "IS_WALKABLE",		TileType::Flag::IS_WALKABLE			},
	{ "IS_BLOCKED",			TileType::Flag::IS_BLOCKED			},
	{ "IS_TRANSPARENT",		TileType::Flag::IS_TRANSPARENT		},
	{ "IS_OPAQUE",			TileType::Flag::IS_OPAQUE			},
	{ "HAS_FLOOR",			TileType::Flag::HAS_FLOOR			},
	{ "HAS_CEILING",		TileType::Flag::HAS_CEILING			},
	{ "CAN_ASCEND",			TileType::Flag::CAN_ASCEND			},
	{ "CAN_DESCEND",		TileType::Flag::CAN_DESCEND			},
	{ "IS_AIR",				TileType::Flag::IS_AIR				},
	{ "CAN_OPEN",			TileType::Flag::CAN_OPEN			},
	{ "CAN_CLOSE",			TileType::Flag::CAN_CLOSE			},
	{ "OPENS_NEIGHBORS",	TileType::Flag::OPENS_NEIGHBORS		},
	{ "CLOSES_NEIGHBORS",	TileType::Flag::CLOSES_NEIGHBORS	},
	{ "IS_HALF_TILE",		TileType::Flag::IS_HALF_TILE		},
};

IS_FLAG_ENUM(TileType::Flag);

namespace tile_types
{
	using TileTypeDB = snowy::SingletonDatabase<TileType>;

	inline TileType NO_TILE{ {
		.ID = "TILE_VOID",
		.noun = { "void", "void" },
		.texture = { { ' ', color::get("red"), color::get("black") } },
		.flags = TileType::Flag::IS_TRANSPARENT | TileType::Flag::HAS_FLOOR | TileType::Flag::HAS_CEILING,
		.tags = { "VOID" }
	} };

	inline snowy::Database<TileType*> tiles_db
	{
		{ "TILE_VOID", &NO_TILE },
	};

	static inline void initialize_db()
	{
		TileTypeDB::init();
	}
}

#endif // !LEMONADE_GAME_SRC_TILE_TYPE_H