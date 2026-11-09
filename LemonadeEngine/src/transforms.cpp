#include "transforms.h"

namespace transforms
{
	Direction opposite_direction(Direction dir)
	{
		Direction opposite_direction = NO_DIRECTION;

		if (dir & NORTH)
		{
			opposite_direction |= SOUTH;
		}
		else if (dir & SOUTH)
		{
			opposite_direction |= NORTH;
		}

		if (dir & EAST)
		{
			opposite_direction |= WEST;
		}
		else if (dir & WEST)
		{
			opposite_direction |= EAST;
		}

		if (dir & ABOVE)
		{
			opposite_direction |= BELOW;
		}
		else if (dir & BELOW)
		{
			opposite_direction |= ABOVE;
		}

		return opposite_direction;
	}

	double dir_to_angle_in_degrees(Direction dir)
	{
		double degrees = 0.0;

		if (dir & NORTH)
		{
			if (dir & EAST)
			{
				degrees = 45.0;
			}
			else if (dir & WEST)
			{
				degrees = 315.0;
			}
			else
			{
				degrees = 0.0;
			}
		}
		else if (dir & SOUTH)
		{
			if (dir & EAST)
			{
				degrees = 135.0;
			}
			else if (dir & WEST)
			{
				degrees = 225.0;
			}
			else
			{
				degrees = 180.0;
			}
		}
		else
		{
			if (dir & EAST)
			{
				degrees = 90.0;
			}
			else if (dir & WEST)
			{
				degrees = 270.0;
			}
		}

		return degrees;
	}


	Position rotate_by_direction(const Position& point, Direction dir, const Position& origin)
	{
		if (dir == NO_DIRECTION || dir == NORTH || point == origin)
			return point;

		double radians = degrees_to_radians(dir_to_angle_in_degrees(dir));

		PositionF tpoint = point - origin;

		PositionF tpoint2;
		
		tpoint2.x = std::round((tpoint.x * std::cos(radians)) - (tpoint.y * std::sin(radians)));
		tpoint2.y = std::round((tpoint.x * std::sin(radians)) + (tpoint.y * std::cos(radians)));
		tpoint2.z = tpoint.z;


		return Position(tpoint2 + origin);
	}

	bool is_adjacent(const Position& p1, const Position& p2, Coord min_dist)
	{
		//return ((p2 - p1).unit() * min_dist).length_squared() >= ((p2 - p1).length_squared()) + DBL_EPSILON;

		for (const auto& adjacent_pos : get_surrounding_locations(p1))
		{
			if (adjacent_pos == p2)
				return true;
		}

		return false;
	}

	bool is_directly_adjacent(const Position& p1, const Position& p2)
	{
		return (p2 - p1).length_squared() <= (1.4 * 1.4) + DBL_EPSILON;
	}

	Position get_closest_adjacent(const Position& start, const Position& destination, Coord near_radius)
	{
		return ((start - destination).unit() * near_radius) + destination;
	}
}