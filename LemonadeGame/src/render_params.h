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
	UIPosition pos{ 0, 0 };
	UISize size{ 48, 32 };
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

	bool render_debug = false;

	UIAttachPosition sidebar_position;
	UIAttachPosition gamelog_position;

	UIPosition world_inner_frame_p1() const
	{
		return world_render_frame.pos + world_render_frame.padding;
	}

	UIPosition world_inner_frame_p2() const
	{
		return (world_render_frame.pos + static_cast<UIPosition>(world_render_frame.size)) - world_render_frame.padding;
	}

	UIPosition world_outer_frame_p1() const
	{
		return world_render_frame.pos;
	}

	UIPosition world_outer_frame_p2() const
	{
		return (world_render_frame.pos + static_cast<UIPosition>(world_render_frame.size));
	}

	UISize world_inner_frame_size() const
	{
		return world_render_frame.size - (world_render_frame.padding * 2);
	}

	UISize world_outer_frame_size() const
	{
		return world_render_frame.size;
	}


	bool in_world_frame(const UIPosition& pos) const
	{
		return world_outer_frame_p1() <= pos && pos < (world_outer_frame_p2() - world_render_frame.padding * 2);
	}

	bool in_world_frame(const Coord& x, const Coord& y) const
	{
		return in_world_frame(UIPosition(x, y));
	}
};

#endif // !LEMONADE_GAME_SRC_RENDER_PARAMS_H