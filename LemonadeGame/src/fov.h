#pragma once

#ifndef LEMONADE_GAME_SRC_FOV_H
#define LEMONADE_GAME_SRC_FOV_H

#include "color.h"
#include "transforms.h"

class TileMap;

namespace FOV
{
	extern void compute_fov(TileMap* map, int player_x, int player_y, int player_z, int radius = 0);
	extern void compute_fov2(TileMap* map, int player_x, int player_y, int player_z, int radius = 0);
}

#endif // !LEMONADE_GAME_SRC_FOV_H