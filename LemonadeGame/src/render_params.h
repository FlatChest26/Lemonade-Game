#pragma once

#ifndef LEMONADE_GAME_SRC_RENDER_PARAMS_H
#define LEMONADE_GAME_SRC_RENDER_PARAMS_H

#include "camera.h"
#include "transforms.h"

struct Rect2D
{
	Vec2D<Coord> pos{0, 0};
	Vec2D<Length> size{48, 32};
	Coord padding = 1;
};

struct RenderParams
{
	const Camera* camera{ nullptr };
	Rect2D world_render_frame;
	bool use_fov = true;
	bool show_explored_tiles = true;
	bool render_world = true;
	bool render_entities = true;
	bool render_tile_map = true;

	Vec2D<Coord> world_inner_frame() const
	{
		return { world_render_frame.pos.x + world_render_frame.padding, world_render_frame.pos.y + world_render_frame.padding };
	}
};

#endif // !LEMONADE_GAME_SRC_RENDER_PARAMS_H