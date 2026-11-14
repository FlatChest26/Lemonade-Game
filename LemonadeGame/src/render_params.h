#pragma once

#ifndef LEMONADE_GAME_SRC_RENDER_PARAMS_H
#define LEMONADE_GAME_SRC_RENDER_PARAMS_H

#include "camera.h"
#include "transforms.h"
#include "game_settings.h"

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

	FrameInt current_frame = 0;

	UIAttachPosition sidebar_position;
	UIAttachPosition gamelog_position;

	UIPosition world_inner_frame_p1() const
	{
		return world_render_frame.pos + UIPosition(world_render_frame.padding);
	}

	UIPosition world_inner_frame_p2() const
	{
		return (world_render_frame.pos + static_cast<UIPosition>(world_render_frame.size)) - UIPosition(world_render_frame.padding);
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
		return world_render_frame.size - UIPosition(world_render_frame.padding * 2);
	}

	UISize world_outer_frame_size() const
	{
		return world_render_frame.size;
	}

	bool in_world_frame(const UIPosition& pos) const
	{
		return in_world_frame(pos.x, pos.y);
	}

	bool in_world_frame(Coord x, Coord y) const
	{
		return
			world_inner_frame_p1().x <= x && x < world_inner_frame_p2().x &&
			world_inner_frame_p1().y <= y && y < world_inner_frame_p2().y;
	}
};

#endif // !LEMONADE_GAME_SRC_RENDER_PARAMS_H