#include "tile.h"

Tile::Tile(const TileType* type, size_t idx) :
	m_type(type)
{
	if (type == nullptr)
	{
		CERR("Tile does not have a type!");
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