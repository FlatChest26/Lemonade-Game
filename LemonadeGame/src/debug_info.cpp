#include "debug_info.h"

namespace DebugInfo
{
	std::unordered_map<Position, DebugTile> get_debug_tiles()
	{
		return debug_tiles;
	}

	void set_debug_tile(Position pos, DebugTile debug_tile)
	{
#ifdef _DEBUG
		debug_tiles[pos] = debug_tile;
#endif
	}

	void clear_debug_tiles()
	{
#ifdef _DEBUG
		debug_tiles.clear();
#endif
	}
}
