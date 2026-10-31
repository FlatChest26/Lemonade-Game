#include "line_of_sight.h"

#include "world.h"

namespace los
{
	RaycastResult raycast3D(const World* world, const Position& start_pos, const Position& end_pos, const RaycastParams& params)
	{
		RaycastResult result;

		auto tile_map = world->get_tile_map();

		if (!tile_map)
		{
			CERR("No tile map on world!");
			return result;
		}

		PositionF ray_start = start_pos;
		PositionF ray_dir = PositionF(end_pos - start_pos).normalized();

		PositionF ray_unit_step_size = {
			std::abs(1 / ray_dir.x),
			std::abs(1 / ray_dir.y),
			std::abs(1 / ray_dir.z),
		};

		Position check_pos = start_pos;
		PositionF ray_length1D;

		Position step = Position(0, 0, 0);

		if (ray_dir.x < 0)
		{
			step.x = -1;
			ray_length1D.x = (ray_start.x - double(check_pos.x)) * ray_unit_step_size.x;
		}
		else if (ray_dir.x > 0)
		{
			step.x = 1;
			ray_length1D.x = (double(check_pos.x + 1) - ray_start.x) * ray_unit_step_size.x;
		}

		if (ray_dir.y < 0)
		{
			step.y = -1;
			ray_length1D.y = (ray_start.y - double(check_pos.y)) * ray_unit_step_size.y;
		}
		else if (ray_dir.y > 0)
		{
			step.y = 1;
			ray_length1D.y = (double(check_pos.y + 1) - ray_start.y) * ray_unit_step_size.y;
		}

		if (ray_dir.z < 0)
		{
			step.z = -1;
			ray_length1D.z = (ray_start.z - double(check_pos.z)) * ray_unit_step_size.z;
		}
		else if (step.z > 0)
		{
			step.z = 1;
			ray_length1D.z = (double(check_pos.z + 1) - ray_start.z) * ray_unit_step_size.z;
		}

		while (result.distance_travelled < params.max_range)
		{
			if (ray_length1D.z < std::min(ray_length1D.x, ray_length1D.y))
			{
				check_pos.z += step.z;
				result.distance_travelled = ray_length1D.z;
				ray_length1D.z += ray_unit_step_size.z;
			}
			else if (ray_length1D.x < ray_length1D.y)
			{
				check_pos.x += step.x;
				result.distance_travelled = ray_length1D.x;
				ray_length1D.x += ray_unit_step_size.x;
			}
			else
			{
				check_pos.y += step.y;
				result.distance_travelled = ray_length1D.y;
				ray_length1D.y += ray_unit_step_size.y;
			}

			result.last_position = check_pos;

			if (params.store_positions_travelled)
			{
				result.positions_travelled.push_back(check_pos);
			}

			if (!tile_map->in_bounds(check_pos))
			{
				result.out_of_bounds = true;
				break;
			}

			if (world->is_blocked(check_pos, params.fit, step.z))
			{
				result.was_blocked = true;
			}

			if (params.stop_when_blocked && result.was_blocked)
			{
				break;
			}

			if (world->is_opaque(check_pos, params.fit, step.z))
			{
				result.was_opaque = true;
			}

			if (params.stop_when_opaque && result.was_opaque)
			{
				break;
			}

			if ((end_pos - check_pos).length_squared() < 2.0f)
			{
				result.reached_end = true;
				break;
			}
		}

		return result;
	}
}