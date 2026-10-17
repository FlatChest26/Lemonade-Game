#include "tile.h"

Tile::Tile(RawMaster::Ref<TileType> type) :
	m_type(type)
{
	if (!type)
	{
		CERR("Tile was not given a type!");
		m_type = RawMaster::get<TileType>("TILE_VOID");
		
	}
	
	if (m_type->has_random_texture())
	{
		std::uniform_int_distribution<size_t> distrib(0, type->get_texture().size() - 1);
		set_texture_idx(distrib(snowy::rng::generator));
	}
	else
	{
		set_texture_idx(0);
	}
}

std::string Tile::get_display_name() const
{
	return snowy::grammar::a(get_noun(), false);
}
