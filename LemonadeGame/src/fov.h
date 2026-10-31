#pragma once

#ifndef LEMONADE_GAME_SRC_FOV_H
#define LEMONADE_GAME_SRC_FOV_H

#include "transforms.h"

class TileMap;

namespace FOV
{
	extern void compute_fov(TileMap* tile_map, const Position& pov, const Coord& radius = 0);
}

#endif // !LEMONADE_GAME_SRC_FOV_H