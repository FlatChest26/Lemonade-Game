#include "fov.h"
#include "tile_map.h"
#include "snowy_macros.h"
#include "line_of_sight.h"
#include "world.h"

namespace sym_shadowcast2d
{
	static const int quadrant_table[4][4] = {
		{ 1,  0,  0,  1 },
		{ 0,  1,  1,  0 },
		{ 0, -1, -1,  0 },
		{ -1,  0,  0, -1 },
	};

	struct Row
	{
		const int pov_x;
		const int pov_y;
		const int pov_z;

		int depth;
		float slope_low;
		const float slope_high;

		int quadrant;
	};

	static bool is_symmetric(const Row* row, int column)
	{
		return column >= row->depth * row->slope_low && column <= row->depth * row->slope_high;
	}

	static float slope(int row_depth, int column)
	{
		return (2.0f * column - 1.0f) / (2.0f * row_depth);
	}

	static int round_half_up(float n)
	{
		return (int)roundf(n * (1 + FLT_EPSILON));
	}

	static int round_half_down(float n)
	{
		return (int)roundf(n * (1 - FLT_EPSILON));
	}

	static void scan(TileMap* map, Row* row, int radius, int radius_squared)
	{
		if (radius > 0 && row->depth > radius)
			return;

		const int xx = quadrant_table[row->quadrant][0];
		const int xy = quadrant_table[row->quadrant][1];
		const int yx = quadrant_table[row->quadrant][2];
		const int yy = quadrant_table[row->quadrant][3];

		if (!map->in_bounds(row->pov_x + row->depth * xx, row->pov_y + row->depth * yx, row->pov_z))
			return;

		const int column_min = round_half_up(row->depth * row->slope_low);
		const int column_max = round_half_down(row->depth * row->slope_high);
		bool prev_tile_is_wall = false;

		for (int column = column_min; column <= column_max; ++column)
		{
			const int map_x = row->pov_x + row->depth * xx + column * xy;
			const int map_y = row->pov_y + row->depth * yx + column * yy;

			if (!map->in_bounds(map_x, map_y, row->pov_z))
				continue;

			const bool is_wall = map->get_tile(map_x, map_y, row->pov_z)->is_opaque();

			if (is_wall || is_symmetric(row, column))
			{
				if (radius > 0)
				{
					const int dx = map_x - row->pov_x;
					const int dy = map_y - row->pov_y;

					if ((dx * dx) + (dy * dy) < radius_squared)
						map->fov_mark_visible(map_x, map_y, row->pov_z);
				}
				else
				{
					map->fov_mark_visible(map_x, map_y, row->pov_z);
				}
			}

			if (prev_tile_is_wall && !is_wall)
			{
				row->slope_low = slope(row->depth, column);
			}

			if (column != column_min && !prev_tile_is_wall && is_wall)
			{
				Row next_row = {
				  .pov_x = row->pov_x,
				  .pov_y = row->pov_y,
				  .depth = row->depth + 1,
				  .slope_low = row->slope_low,
				  .slope_high = slope(row->depth, column),
				  .quadrant = row->quadrant,
				};
				scan(map, &next_row, radius, radius_squared);
			}

			prev_tile_is_wall = is_wall;
		}

		if (!prev_tile_is_wall)
		{
			row->depth += 1;
			scan(map, row, radius, radius_squared);
		}
	}
}

namespace sym_shadowcast3d
{
	struct Tile
	{
		int row;
		int column;
		int layer;
	};

	static int round_ties_up(double n)
	{
		return static_cast<int>(std::floor(n + 0.5));
	}

	static int round_ties_down(double n)
	{
		return static_cast<int>(std::ceil(n - 0.5));
	}

	static double slope(int depth, int column)
	{
		return (2.0f * double(column) - 1.0f) / (2.0f * double(depth));
	}

	struct Hextant
	{
		enum CardinalDirection : int
		{
			NORTH = 0,
			EAST = 1,
			SOUTH = 2,
			WEST = 3,
			UP = 4,
			DOWN = 5,
		};

		Position origin;
		CardinalDirection direction;

		Position transform(Tile tile) const
		{
			switch (direction)
			{
			case CardinalDirection::NORTH:
				return Position(origin.x + tile.column, origin.y - tile.row, origin.z + tile.layer);
			case CardinalDirection::EAST:
				return Position(origin.x + tile.column, origin.y + tile.row, origin.z + tile.layer);
			case CardinalDirection::SOUTH:
				return Position(origin.x + tile.row, origin.y + tile.column, origin.z + tile.layer);
			case CardinalDirection::WEST:
				return Position(origin.x - tile.row, origin.y + tile.column, origin.z + tile.layer);
			case CardinalDirection::UP:
				return Position(origin.x + tile.layer, origin.y - tile.column, origin.z + tile.row);
			case CardinalDirection::DOWN:
				return Position(origin.x + tile.layer, origin.y - tile.column, origin.z - tile.row);
			}

			CERR("Invalid direction given to hextant! " << direction);
			return Position();
		}
	};

	struct Plane
	{
		int depth;

		double start_slope_layer;
		double start_slope_column;
		double end_slope_layer;
		double end_slope_column;

		Hextant hextant;

		Plane next() const
		{
			return Plane
			{
				.depth = depth + 1,

				.start_slope_layer = start_slope_layer,
				.start_slope_column = start_slope_column,
				.end_slope_layer = end_slope_layer,
				.end_slope_column = end_slope_column,

				.hextant = hextant
			};
		}
	};

	static bool is_symmetrical(const Plane& plane, Tile tile)
	{
		return (tile.column >= plane.depth * plane.start_slope_column) && (tile.column <= plane.depth * plane.end_slope_column) &&
			(tile.layer >= plane.depth * plane.start_slope_layer) && (tile.layer <= plane.depth * plane.end_slope_layer);
	}

	static void scan(TileMap* tile_map, Plane plane, const Coord& radius)
	{
		if (radius > 0 && plane.depth > radius)
			return;

		int layer_min = round_ties_up(plane.depth * plane.start_slope_layer);
		int layer_max = round_ties_down(plane.depth * plane.end_slope_layer);

		int column_min = round_ties_up(plane.depth * plane.start_slope_column);
		int column_max = round_ties_down(plane.depth * plane.end_slope_column);

		auto world = tile_map->get_world();

		auto reveal = [&](const Tile& tile)
			{
				Position pos = plane.hextant.transform(tile);
				tile_map->fov_mark_visible(pos.x, pos.y, pos.z);
			};

		auto is_wall = [&](const Tile& tile)
			{
				Position pos = plane.hextant.transform(tile);
				return world->is_opaque(pos, nullptr, pos.z - plane.hextant.origin.z);
			};

		auto is_floor = [&](const Tile& tile)
			{
				return !is_wall(tile);
			};

		for (int layer = layer_min; layer < layer_max + 1; layer++)
		{
			Tile prev_tile{};

			for (int column = column_min; column < column_max + 1; column++)
			{
				Tile tile = Tile(plane.depth, column, layer);

				if (is_wall(tile) || is_symmetrical(plane, tile))
				{
					reveal(tile);
				}

				if (is_wall(prev_tile) && is_floor(tile))
				{
					plane.start_slope_column = slope(plane.depth, column);
				}

				if (is_floor(prev_tile) && is_wall(tile))
				{
					Plane next_plane = plane.next();
					next_plane.end_slope_column = slope(plane.depth, column);

					scan(tile_map, next_plane, radius);
				}

				prev_tile = tile;
			}
		}

		//scan(tile_map, plane.next(), radius);
	}
}

namespace FOV
{
	static void compute_fov_symmetric_shadowcast2d(TileMap* tile_map, const Position& pov, const Coord& radius)
	{
		using namespace sym_shadowcast2d;

		tile_map->fov_mark_visible(pov.x, pov.y, pov.z);
		const int radius_squared = radius * radius;

		for (int quadrant = 0; quadrant < 4; ++quadrant)
		{
			Row row =
			{
				.pov_x = pov.x,
				.pov_y = pov.y,
				.pov_z = pov.z,
				.depth = 1,
				.slope_low = -1.0f,
				.slope_high = 1.0f,
				.quadrant = quadrant
			};

			scan(tile_map, &row, radius, radius_squared);
		}
	}

	static void compute_fov_tcod_way(TileMap* tile_map, const Position& pov, const Coord& radius)
	{
		size_t bottom_lyr = 0;
		size_t top_lyr = tile_map->get_depth();

		auto tile_below = tile_map->get_tile(pov + Position(0, 0, -1));
		auto tile_above = tile_map->get_tile(pov + Position(0, 0, 1));

		if (!tile_below || tile_below->is_opaque() || tile_below->has_ceiling())
		{
			bottom_lyr = size_t(pov.z);
		}

		if (!tile_above || tile_above->is_opaque() || tile_above->has_floor())
		{
			top_lyr = size_t(pov.z + 1);
		}

		for (size_t z_layer = bottom_lyr; z_layer < top_lyr; z_layer++)
		{
			auto tcod_map = tile_map->get_tcod_map_layer((size_t)z_layer);
			tcod_map->computeFov(pov.x, pov.y, radius, true, FOV_SHADOW);
		}
	}

	static void compute_fov_symmetric_shadowcast3d(TileMap* tile_map, const Position& pov, const Coord& radius)
	{
		using namespace sym_shadowcast3d;

		tile_map->fov_mark_visible(pov.x, pov.y, pov.z);

		for (int hextant_num = 0; hextant_num < 6; hextant_num++)
		{
			Plane plane
			{
				.depth = 1,

				.start_slope_layer = -1,
				.start_slope_column = -1,

				.end_slope_layer = 1,
				.end_slope_column = 1,

				.hextant = {
					.origin = pov,
					.direction = Hextant::CardinalDirection(hextant_num)
				}
			};

			scan(tile_map, plane, radius);
		}
	}

	void compute_fov(TileMap* tile_map, const Position& pov, const Coord& radius)
	{
		if (!tile_map)
		{
			CERR("tile_map is null!");
			return;
		}

		if (!tile_map->in_bounds(pov))
		{
			CERR("pov is out of bounds!");
			return;
		}

		compute_fov_tcod_way(tile_map, pov, radius);

		// compute_fov_symmetric_shadowcast2d(tile_map, pov, radius);
		//compute_fov_symmetric_shadowcast3d(tile_map, pov, radius);
	}
}