#pragma once

#ifndef LEMONADE_GAME_SRC_DEBUG_INFO_H
#define LEMONADE_GAME_SRC_DEBUG_INFO_H

#include "transforms.h"
#include "renderable.h"

#include <unordered_map>

namespace DebugInfo
{
	struct DebugTile
	{
		Renderable renderable;

		bool change_glyph = false;
		bool change_fg = false;
		bool change_bg = false;
	};

	inline std::unordered_map<Position, DebugTile> debug_tiles;

	extern void set_debug_tile(Position pos, DebugTile debug_tile);
	extern void clear_debug_tiles();

	extern std::unordered_map<Position, DebugTile> get_debug_tiles();
}


#endif // !LEMONADE_GAME_SRC_DEBUG_INFO_H
