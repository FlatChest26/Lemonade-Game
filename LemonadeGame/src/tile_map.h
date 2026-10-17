#pragma once

#ifndef LEMONADE_GAME_SRC_TILE_MAP_H
#define LEMONADE_GAME_SRC_TILE_MAP_H

#include "tile.h"
#include "time_units.h"
#include "render_params.h"
#include "tile_type.h"
#include "pathfinding.h"


#include "raw_master.h"

#include <libtcod.hpp>

#include <iostream>

class World;

inline constexpr Length DEFAULT_MAP_WIDTH = 48;
inline constexpr Length DEFAULT_MAP_HEIGHT = 32;
inline constexpr Length DEFAULT_MAP_DEPTH = 4;

class TileMap
{
protected:
	World* m_world{ nullptr };
	std::vector<Tile> m_tiles{ };
	std::vector<Pathfinding::Node> m_nodes{ };

	Length m_width{ DEFAULT_MAP_WIDTH };
	Length m_height{ DEFAULT_MAP_HEIGHT };
	Length m_depth{ DEFAULT_MAP_DEPTH };

	Tile m_void_tile;

	std::vector<std::unique_ptr<TCODMap>> m_tcod_map_layers{};

public:
	TileMap(World* world = {}, const Length& width = DEFAULT_MAP_WIDTH, const Length& height = DEFAULT_MAP_HEIGHT, const Length& depth = DEFAULT_MAP_DEPTH);
	~TileMap();

public:
	// -- Getters -- //

	World* get_world();
	void set_world(World* world);

	Length get_width() const { return m_width; }
	Length get_height() const { return m_height; }
	Length get_depth() const { return m_depth; }

	// Returns a pointer to the tile at the given coordinates. If the coordinates are out of bounds, returns a pointer to the void tile.
	Tile* get_tile(const Coord& x, const Coord& y, const Coord& z)
	{
		if (!in_bounds(x, y, z))
		{
			return &m_void_tile; // Return void if tile out of bounds
		}
		return &m_tiles[get_index(x, y, z)];
	}

	Tile* get_tile(const Position& position) { return get_tile(position.x, position.y, position.z); }

	const Tile* get_tile(const Coord& x, const Coord& y, const Coord& z) const
	{
		if (!in_bounds(x, y, z))
		{
			return &m_void_tile; // Return void if tile out of bounds
		}
		return &m_tiles[get_index(x, y, z)];
	}

	const Tile* get_tile(const Position& position) const { return get_tile(position.x, position.y, position.z); }

	Pathfinding::Node* get_node(const Coord& x, const Coord& y, const Coord& z)
	{
		if (!in_bounds(x, y, z))
		{
			return nullptr; // Return void if tile out of bounds
		}
		return &m_nodes[get_index(x, y, z)];
	}

	Pathfinding::Node* get_node(const Position& position) { return get_node(position.x, position.y, position.z); }

	constexpr bool in_bounds(const Coord& x, const Coord& y, const Coord& z) const
	{
		return x >= 0 && (Length)x < m_width && y >= 0 && (Length)y < m_height && z >= 0 && (Length)z < m_depth;
	}

	constexpr bool in_bounds(const Position& position) const
	{
		return in_bounds(position.x, position.y, position.z);
	}

	constexpr size_t get_index(const Coord& x, const Coord& y, const Coord& z) const
	{
		return size_t((Length)x * m_height * m_depth + (Length)y * m_depth + (Length)z);
	}

	constexpr size_t get_index(const Position& position) const
	{
		return get_index(position.x, position.y, position.z);
	}

	TCODMap* get_tcod_map_layer(const size_t& layer)
	{
		return m_tcod_map_layers[layer].get();
	}

	const TCODMap* get_tcod_map_layer(const size_t& layer) const
	{
		return m_tcod_map_layers[layer].get();
	}

	double euclidian_distance_between(Position p1, Position p2);

public:
	// -- Utilities -- //

	void set_tile(const Coord& x, const Coord& y, const Coord& z, RawMaster::Ref<TileType> type);
	void set_tile(const Position& pos, RawMaster::Ref<TileType> type)
	{
		set_tile(pos.x, pos.y, pos.z, type);
	}

	void update_tile_at(const Coord& x, const Coord& y, const Coord& z);

	void reset_nodes();

	// Field of View //

	void compute_fov(const Coord& pov_x, const Coord& pov_y, const Coord& pov_z, const Coord& radius);
	void fov_mark_visible(const Coord& x, const Coord& y, const Coord& z, bool visible = true);

	bool is_opaque(const Coord& x, const Coord& y, const Coord& z) const;
	bool is_opaque(const Position& pos) const
	{
		return is_opaque(pos.x, pos.y, pos.z);
	}

	bool is_visible(const Coord& x, const Coord& y, const Coord& z) const;
	bool is_visible(const Position& pos) const { return is_visible(pos.x, pos.y, pos.z); }

	bool is_explored(const Coord& x, const Coord& y, const Coord& z) const;
	bool is_explored(const Position& pos) const { return is_explored(pos.x, pos.y, pos.z); }

	// -- Checks -- //

	bool is_blocked(const Position& pos) const
	{
		return is_blocked(pos.x, pos.y, pos.z);
	}

	bool is_blocked(const Coord& x, const Coord& y, const Coord& z) const
	{
		const Tile* tile = get_tile(x, y, z);
		return !tile->is_walkable();
	}

public:

	// -- Game Loop -- //

	virtual void step(TimeUnit time_delta);
	virtual void update();
	// virtual void render(RenderParams params) const;
};

#endif // !LEMONADE_GAME_SRC_TILE_MAP_H