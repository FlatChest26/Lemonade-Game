#pragma once

#ifndef LEMONADE_GAME_SRC_MAP_BUILDER_H
#define LEMONADE_GAME_SRC_MAP_BUILDER_H

#include "transforms.h"
#include "thing.h"

#include <string>
#include <vector>
#include <map>

class TileMap;
class TileType;

using TileMapping = std::map<char, ThingID>;

namespace MapBuilder
{
	inline const TileMapping default_tile_mapping
	{
		{ ' ', "TILE_AIR"					},
		{ ',', "TILE_GRASS"					},
		{ '.', "TILE_PLAIN_FLOOR"			},
		{ '#', "TILE_PLAIN_WALL"			},
		{ '+', "TILE_DOOR"					},
		{ 'o', "TILE_WINDOW"				},
		{ '<', "TILE_UPWARD_STAIRCASE"		},
		{ '>', "TILE_DOWNWARD_STAIRCASE"	},
		{ 'X', "TILE_UP_AND_DOWN_STAIRCASE" },
		{ '=', "TILE_GARAGE_DOOR"			},
		{ '^', "TILE_FENCE"					},
		{ '/', "TILE_GLASS_DOOR"			},
		{ 'V', "TILE_SLANTED_ROOF"			},
	};

	struct ASCIIMap
	{
		std::vector<std::string> map_string;
		TileMapping tile_mappings{ MapBuilder::default_tile_mapping };
	};

	extern void set_floor(TileMap* tile_map, TileType* floor_tile, Coord layer = 0);

	extern void make_building(
		TileMap* tile_map,
		Position building_position,
		Size building_size,
		TileType* floor_tile = nullptr,
		TileType* wall_tile = nullptr,
		TileType* air_tile = nullptr,
		TileType* roof_tile = nullptr
	);

	extern void make_from_ascii_map(
		TileMap* tile_map,
		ASCIIMap ascii_map,
		bool leave_out_spaces = true,
		Position position = Position(0, 0, 0)
	);
}

#endif // !LEMONADE_GAME_SRC_MAP_BUILDER_H