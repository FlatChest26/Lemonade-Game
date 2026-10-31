#pragma once

#ifndef LEMONADE_GAME_SRC_LINE_OF_SIGHT_H
#define LEMONADE_GAME_SRC_LINE_OF_SIGHT_H

#include "transforms.h"

class World;
class Entity;

namespace los
{
	struct RaycastParams
	{
		double max_range = 100.0f;
		bool stop_when_blocked = false; // Stop at an unwalkable tile
		bool stop_when_opaque = false; // Stop at an opauqe tile
		const Entity* fit = nullptr;
		bool store_positions_travelled = false;
	};

	struct RaycastResult
	{
		bool reached_end = false;
		bool was_blocked = false;
		bool was_opaque = false;
		bool out_of_bounds = false;
		double distance_travelled = 0.0f;
		std::vector<Position> positions_travelled;
		Position last_position;
	};

	extern RaycastResult raycast3D(const World* world, const Position& start_pos, const Position& end_pos, const RaycastParams& params);
}

#endif // !LEMONADE_GAME_SRC_LINE_OF_SIGHT_H
