#pragma once

#ifndef LEMONADE_GAME_SRC_PATHFINDING_H
#define LEMONADE_GAME_SRC_PATHFINDING_H

#include "transforms.h"
#include <vector>

class TileMap;
class Entity;

namespace Pathfinding
{
	std::vector<Position> compute_path(TileMap* tile_map, Position start_pos, Position end_pos, Entity* entity = nullptr);
}

#endif // !LEMONADE_GAME_SRC_PATHFINDING_H
