#pragma once

#ifndef LEMONADE_GAME_SRC_MAP_BUILDER_H
#define LEMONADE_GAME_SRC_MAP_BUILDER_H

#include "transforms.h"
#include "thing.h"
#include "raw_master.h"
#include "material_def.h"

#include <string>
#include <vector>
#include <map>

class TileMap;
struct TileType;
class World;

struct MappingType
{
	RawMaster::RawID tile;
	RawMaster::RawID furniture;
	RawMaster::RawID item;

	RawMaster::RawID material;
	MaterialAmount material_amount{ 1 };
	units::Temperature temperature{ 60.0_degF };
};

using TileMapping = std::map<char, MappingType>;

namespace MapBuilder
{
	struct ASCIIMap
	{
		TileMapping tile_mappings;
		std::vector<std::string> map_string;
	};

	struct LayeredASCIIMap
	{
		TileMapping tile_mappings;
		std::vector<std::vector<std::string>> map_string_layers;
	};

	extern void set_floor(TileMap* tile_map, RawMaster::Ref<TileType> floor_tile, Coord layer = 0);

	extern void make_building(
		TileMap* tile_map,
		Position building_position,
		Size building_size,
		RawMaster::Ref<TileType> floor_tile = nullptr,
		RawMaster::Ref<TileType> wall_tile = nullptr,
		RawMaster::Ref<TileType> air_tile = nullptr,
		RawMaster::Ref<TileType> roof_tile = nullptr
	);

	extern void make_from_ascii_map(
		World* world,
		ASCIIMap ascii_map,
		bool leave_out_spaces = true,
		Position position = Position(0, 0, 0)
	);

	extern void make_from_layered_ascii_map(
		World* world,
		LayeredASCIIMap layered_map,
		bool leave_out_spaces = true,
		Position position = Position(0, 0, 0)
	);
}

#endif // !LEMONADE_GAME_SRC_MAP_BUILDER_H