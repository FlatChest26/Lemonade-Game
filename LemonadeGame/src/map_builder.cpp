#include "map_builder.h"
#include "tile_map.h"
#include "raw_master.h"

namespace MapBuilder
{
	void set_floor(TileMap* tile_map, TileType* floor_tile, Coord layer)
	{
		if (!tile_map)
		{
			CERR("tile_map is nullptr!");
			return;
		}

		for (Coord y = 0; (Length)y < tile_map->get_height(); y++) for (Coord x = 0; (Length)x < tile_map->get_width(); x++)
		{
			tile_map->set_tile(x, y, layer, floor_tile);
		}
	}

	void make_building(TileMap* tile_map, Position building_position, Size building_size, TileType* floor_tile, TileType* wall_tile, TileType* air_tile, TileType* roof_tile)
	{
		if (!tile_map)
		{
			CERR("tile_map is nullptr!");
			return;
		}

		int building_left = building_position.x;
		int building_right = building_position.x + building_size.x;

		int building_top = building_position.y;
		int building_bottom = building_position.y + building_size.y;

		int building_back = building_position.z;
		int building_front = building_position.z + building_size.z;

		TileType* place_tile;

		for (int z = building_back; z < building_front; z++) for (int y = building_top; y < building_bottom; y++) for (int x = building_left; x < building_right; x++)
		{
			place_tile = air_tile;

			if (floor_tile)
			{
				if (z == building_back)
				{
					place_tile = floor_tile;
				}
			}

			if (wall_tile)
			{
				if (x == building_left || x == building_right - 1 ||
					y == building_top || y == building_bottom - 1)
				{
					place_tile = wall_tile;
				}
			}

			if (place_tile) tile_map->set_tile(x, y, z, place_tile);
		}

		if (roof_tile)
		{
			for (int y = building_top; y < building_bottom; y++) for (int x = building_left; x < building_right; x++)
			{
				tile_map->set_tile(x, y, building_front, roof_tile);
			}
		}
	}

	void make_from_ascii_map(TileMap* tile_map, ASCIIMap ascii_map, bool leave_out_spaces, Position position)
	{
		if (!tile_map)
		{
			CERR("tile_map is nullptr!");
			return;
		}

		Coord x = position.x;
		Coord y = position.y;
		Coord z = position.z;

		for (const auto& row : ascii_map.map_string)
		{
			x = position.x - 1;

			for (const auto& character : row)
			{
				x += 1;

				if (leave_out_spaces && character == ' ')
				{
					continue;
				}

				if (!ascii_map.tile_mappings.contains(character))
				{
					CERR("invalid tile character found " << character);
					tile_map->set_tile(x, y, z, RawMaster::get_tile_type("TILE_AIR"));
					continue;
				}

				tile_map->set_tile(x, y, z, RawMaster::get_tile_type(ascii_map.tile_mappings[character]));
			}

			y += 1;
		}
	}
}