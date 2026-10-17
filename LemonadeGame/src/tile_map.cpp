#include "tile_map.h"
#include "tile_type.h"
#include "fov.h"
#include <variant>
#include <cmath>

#include "raw_master.h"

TileMap::TileMap(World* world, const Length& width, const Length& height, const Length& depth) :
	m_world(world), m_width(width), m_height(height), m_depth(depth)
{
	m_void_tile = Tile(RawMaster::get<TileType>("TILE_VOID"));

	m_tiles.reserve(width * height * depth);
	m_nodes.reserve(width * height * depth);

	for (Coord z = 0; (Length)z < depth; ++z)
	{
		m_tcod_map_layers.push_back(std::move(std::make_unique<TCODMap>(m_width, m_height)));
	}

	for (Coord x = 0; (Length)x < width; ++x) for (Coord y = 0; (Length)y < height; ++y) for (Coord z = 0; (Length)z < depth; ++z)
	{
		m_tiles.push_back(Tile(RawMaster::get<TileType>("TILE_AIR")));
		m_nodes.push_back(Pathfinding::Node{
			.pos = {x, y, z}
		});
	}

	for (Coord x = 0; (Length)x < m_width; ++x) for (Coord y = 0; (Length)y < m_height; ++y) for (Coord z = 0; (Length)z < m_depth; ++z)
	{
		update_tile_at(x, y, z);
	}

	reset_nodes();
}

TileMap::~TileMap()
{
}

World* TileMap::get_world()
{
	return m_world;
}

void TileMap::set_world(World* world)
{
	m_world = world;
}

double TileMap::euclidian_distance_between(Position p1, Position p2)
{
	return std::sqrt(
		std::pow(static_cast<double>(p1.x) - static_cast<double>(p2.x), 2) + 
		std::pow(static_cast<double>(p1.y) - static_cast<double>(p2.y), 2) +
		std::pow(static_cast<double>(p1.z) - static_cast<double>(p2.z), 2)
	);
}

void TileMap::set_tile(const Coord& x, const Coord& y, const Coord& z, RawMaster::Ref<TileType> type)
{
	if (!in_bounds(x, y, z))
	{
		return; // Do nothing if tile out of bounds
	}

	//m_tiles[get_index(x, y, z)].disconnect_tiles();
	m_tiles[get_index(x, y, z)] = Tile(type);

	update_tile_at(x, y, z);

	update_tile_at(x + 1, y, z);
	update_tile_at(x - 1, y, z);
	update_tile_at(x, y + 1, z);
	update_tile_at(x, y - 1, z);
	update_tile_at(x + 1, y + 1, z);
	update_tile_at(x + 1, y - 1, z);
	update_tile_at(x - 1, y + 1, z);
	update_tile_at(x - 1, y - 1, z);
}

void TileMap::update_tile_at(const Coord& x, const Coord& y, const Coord& z)
{
	if (!in_bounds(x, y, z))
	{
		return; // Do nothing if tile out of bounds
	}

	auto tile = get_tile(x, y, z);
	//tile->disconnect_tiles();
	
	auto tcod_map = get_tcod_map_layer((size_t)z);
	tcod_map->setProperties(x, y, tile->is_transparent(), tile->is_walkable());
	
	// Update wall textures
	if (tile->is_wall_texture())
	{
		int mask_value = 0;

		auto tile_above = get_tile(x, y - 1, z);
		auto tile_below = get_tile(x, y + 1, z);
		auto tile_left = get_tile(x - 1, y, z);
		auto tile_right = get_tile(x + 1, y, z);

		if (tile_above && (tile->can_connect_with(tile_above) || tile_above->can_connect_with(tile)))
		{
			mask_value += 1;
			//tile->connect_tile(tile_above);
		}

		if (tile_below && (tile->can_connect_with(tile_below) || tile_below->can_connect_with(tile)))
		{
			mask_value += 2;
			//tile->connect_tile(tile_below);
		}

		if (tile_left && (tile->can_connect_with(tile_left) || tile_left->can_connect_with(tile)))
		{
			mask_value += 4;
			//tile->connect_tile(tile_left);
		}

		if (tile_right && (tile->can_connect_with(tile_right) || tile_right->can_connect_with(tile)))
		{
			mask_value += 8;
			//tile->connect_tile(tile_right);
		}

		if (mask_value >= 0 && mask_value <= 16)
		{
			auto decoration_index = DECORATION_MASK[mask_value];

			tile->set_texture_idx(decoration_index);
		}
		else
		{
			tile->set_texture_idx(0);
		}
	}
}

void TileMap::reset_nodes()
{
	for (Coord x = 0; (Length)x < get_width(); ++x) for (Coord y = 0; (Length)y < get_height(); ++y) for (Coord z = 0; (Length)z < get_depth(); ++z)
	{
		m_nodes[get_index(x, y, z)] = Pathfinding::Node{
			.pos = {x, y, z},
			.visited = false,
			.parent = nullptr,
			.global_goal = INFINITY,
			.local_goal = INFINITY,

		};
	}
}

void TileMap::compute_fov(const Coord& pov_x, const Coord& pov_y, const Coord& pov_z, const Coord& radius)
{
	//for (Coord x = 0; (Length)x < get_width(); ++x) for (Coord y = 0; (Length)y < get_height(); ++y) for (Coord z = 0; (Length)z < get_depth(); ++z)
	//{
	//	fov_mark_visible(x, y, z, false);
	//}

	//FOV::compute_fov(this, pov_x, pov_y, pov_z, radius);

	FOV::compute_fov2(this, pov_x, pov_y, pov_z, radius);

	auto tcod_map = get_tcod_map_layer((size_t)pov_z);
	for (Coord x = 0; (Length)x < get_width(); ++x) for (Coord y = 0; (Length)y < get_height(); ++y)
	{
		fov_mark_visible(x, y, pov_z, tcod_map->isInFov(x, y));
	}
}

bool TileMap::is_opaque(const Coord& x, const Coord& y, const Coord& z) const
{
	if (!in_bounds(x, y, z))
	{
		return false;
	}

	if (auto tile = get_tile(x, y, z))
	{
		return tile->is_opaque();
	}

	return false;
}

void TileMap::fov_mark_visible(const Coord& x, const Coord& y, const Coord& z, bool visible)
{
	if (!in_bounds(x, y, z))
	{
		return;
	}

	if (auto tile = get_tile(x, y, z))
	{
		tile->set_visible(visible);
	}

	//if (auto tcod_map = get_tcod_map_layer((size_t)z))
	//{
	//	tcod_map->setInFov(x, y, visible);
	//}
}

bool TileMap::is_visible(const Coord& x, const Coord& y, const Coord& z) const
{
	if (!in_bounds(x, y, z))
	{
		return false;
	}

	//if (auto tcod_map = get_tcod_map_layer((size_t)z))
	//{
	//	return tcod_map->isInFov(x, y);
	//}

	if (auto tile = get_tile(x, y, z))
	{
		return tile->is_visible();
	}

	return false;
}

bool TileMap::is_explored(const Coord& x, const Coord& y, const Coord& z) const
{
	if (!in_bounds(x, y, z))
	{
		return false;
	}

	if (auto tile = get_tile(x, y, z))
	{
		return tile->is_explored();
	}

	return false;
}

void TileMap::step(TimeUnit time_delta)
{
}

void TileMap::update()
{
}