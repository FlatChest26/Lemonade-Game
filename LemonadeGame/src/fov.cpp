#include "fov.h"
#include "tile_map.h"
#include "snowy_macros.h"

namespace sym_shadowcast
{
	static const int quadrant_table[4][4] = {
		{ 1,  0,  0,  1},
		{ 0,  1,  1,  0},
		{ 0, -1, -1,  0},
		{-1,  0,  0, -1},
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

namespace FOV
{
	void compute_fov(TileMap* map, int player_x, int player_y, int player_z, int radius)
	{
		if (map == nullptr)
			return;

		if (!map->in_bounds(player_x, player_y, player_z))
			return;

		using namespace sym_shadowcast;

		map->fov_mark_visible(player_x, player_y, player_z);
		const int radius_squared = radius * radius;

		for (int quadrant = 0; quadrant < 4; ++quadrant)
		{
			Row row =
			{
				.pov_x = player_x,
				.pov_y = player_y,
				.pov_z = player_z,
				.depth = 1,
				.slope_low = -1.0f,
				.slope_high = 1.0f,
				.quadrant = quadrant
			};

			scan(map, &row, radius, radius_squared);
		}
	}

	void compute_fov2(TileMap* map, int player_x, int player_y, int player_z, int radius)
	{
		if (!map)
		{
			CERR("map is null!");
			return;
		}

		size_t bottom_lyr = 0;
		size_t top_lyr = map->get_depth();

		auto tile_below = map->get_tile(player_x, player_y, player_z - 1);
		auto tile_above = map->get_tile(player_x, player_y, player_z + 1);

		if (!tile_below || tile_below->is_opaque() || tile_below->has_ceiling())
		{
			bottom_lyr = size_t(player_z);
		}

		if (!tile_above || tile_above->is_opaque() || tile_above->has_floor())
		{
			top_lyr = size_t(player_z + 1);
		}

		for (size_t z_layer = bottom_lyr; z_layer < top_lyr; z_layer++)
		{
			auto tcod_map = map->get_tcod_map_layer((size_t)z_layer);
			tcod_map->computeFov(player_x, player_y, radius, true, FOV_SHADOW);
		}
	}
}