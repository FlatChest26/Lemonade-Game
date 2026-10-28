#pragma once

#ifndef LEMONADE_GAME_SRC_RENDER_PARAMS_H
#define LEMONADE_GAME_SRC_RENDER_PARAMS_H

#include "camera.h"
#include "transforms.h"

enum class UIAttachPosition
{
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};

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

	UIAttachPosition sidebar_position;
	UIAttachPosition gamelog_position;

	Vec2D<Coord> world_inner_frame_p1() const
	{
		return world_render_frame.pos + world_render_frame.padding;
	}

	Vec2D<Coord> world_inner_frame_p2() const
	{
		return (world_render_frame.pos + static_cast<Vec2D<Coord>>(world_render_frame.size)) - world_render_frame.padding;
	}

	Vec2D<Coord> world_outer_frame_p1() const
	{
		return world_render_frame.pos;
	}

	Vec2D<Coord> world_outer_frame_p2() const
	{
		return (world_render_frame.pos + static_cast<Vec2D<Coord>>(world_render_frame.size));
	}

	bool in_world_frame(const Vec2D<Coord>& pos) const
	{
		return world_outer_frame_p1() <= pos && pos < (world_outer_frame_p2() - world_render_frame.padding * 2);
	}

	bool in_world_frame(const Coord& x, const Coord& y) const
	{
		return in_world_frame(Vec2D<Coord>(x, y));
	}
};

#endif // !LEMONADE_GAME_SRC_RENDER_PARAMS_H