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

	double angle_in_degrees_to_dir(double angle)
	{
		Direction dir;

		if (angle < 45.0)
		{
			dir |= NORTH;
		}
		else if (angle < 90.0)
		{
			dir |= NORTHEAST;
		}
		else if (angle < 135.0)
		{
			dir |= EAST;
		}
		else if (angle < 180.0)
		{
			dir |= SOUTHEAST;
		}
		else if (angle < 225.0)
		{
			dir |= SOUTH;
		}
		else if (angle < 270.0)
		{
			dir |= SOUTHWEST;
		}
		else if (angle < 315.0)
		{
			dir |= WEST;
		}
		else 
		{
			dir |= NORTHWEST;
		}

		return dir;
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
		return ((start - destination).unit() * Position(near_radius)) + destination;
	}

	Direction rotate_direction_clockwise(Direction dir, int amount)
	{
		Direction new_dir = dir;
		
		//new_dir |= ~ABOVE;
		//new_dir |= ~BELOW;

		int count = amount;

		while (count > 0)
		{
			switch (new_dir)
			{
			case NORTH:
				new_dir = NORTHEAST;
				break;
			case NORTHEAST:
				new_dir = EAST;
				break;
			case EAST:
				new_dir = SOUTHEAST;
				break;
			case SOUTHEAST:
				new_dir = SOUTH;
				break;
			case SOUTH:
				new_dir = SOUTHWEST;
				break;
			case SOUTHWEST:
				new_dir = WEST;
				break;
			case WEST:
				new_dir = NORTHWEST;
				break;
			case NORTHWEST:
				new_dir = NORTH;
				break;
			}

			count--;
		}

		return new_dir;
	}

	Direction rotate_direction_counterclockwise(Direction dir, int amount)
	{
		Direction new_dir = dir;

		//new_dir |= ~ABOVE;
		//new_dir |= ~BELOW;

		int count = amount;

		while (count > 0)
		{
			switch (new_dir)
			{
			case NORTH:
				new_dir = NORTHWEST;
				break;
			case NORTHWEST:
				new_dir = WEST;
				break;
			case WEST:
				new_dir = SOUTHWEST;
				break;
			case SOUTHWEST:
				new_dir = SOUTH;
				break;
			case SOUTH:
				new_dir = SOUTHEAST;
				break;
			case SOUTHEAST:
				new_dir = EAST;
				break;
			case EAST:
				new_dir = NORTHEAST;
				break;
			case NORTHEAST:
				new_dir = NORTH;
				break;
			}

			count--;
		}

		return new_dir;
	}
}