#pragma once

#ifndef LEMONADE_GAME_SRC_TILE_MAP_H
#define LEMONADE_GAME_SRC_TILE_MAP_H

#include "tile.h"
#include "time_units.h"
#include "render_params.h"
#include "tile_type.h"

#include <iostream>

class World;

namespace tile_map
{
	inline constexpr size_t DEFAULT_WIDTH = 64;
	inline constexpr size_t DEFAULT_HEIGHT = 32;
	inline constexpr size_t DEFAULT_DEPTH = 4;

	inline Tile VOID_TILE{ &tile_types::NO_TILE, 0 };
}

class TileMap
{
protected:
	World* m_world;
	std::vector<Tile> m_tiles;

	Length m_width{ tile_map::DEFAULT_WIDTH };
	Length m_height{ tile_map::DEFAULT_HEIGHT };
	Length m_depth{ tile_map::DEFAULT_DEPTH };

public:
	TileMap(World* world = {}, const Length& width = tile_map::DEFAULT_WIDTH, const Length& height = tile_map::DEFAULT_HEIGHT, const Length& depth = tile_map::DEFAULT_DEPTH);
	~TileMap();

public:
	// -- Getters -- //

	Length get_width() const { return m_width; }
	Length get_height() const { return m_height; }
	Length get_depth() const { return m_depth; }

	// Returns a pointer to the tile at the given coordinates. If the coordinates are out of bounds, returns a pointer to the void tile.
	Tile* get_tile(const Coord& x, const Coord& y, const Coord& z)
	{
		if (!in_bounds(x, y, z))
		{
			return &tile_map::VOID_TILE; // Return void if tile out of bounds
		}
		return &m_tiles[get_index(x, y, z)];
	}

	Tile* get_tile(const Position& position) { return get_tile(position.x, position.y, position.z); }

	const Tile* get_tile(const Coord& x, const Coord& y, const Coord& z) const
	{
		if (!in_bounds(x, y, z))
		{
			return &tile_map::VOID_TILE; // Return void if tile out of bounds
		}
		return &m_tiles[get_index(x, y, z)];
	}

	const Tile* get_tile(const Position& position) const { return get_tile(position.x, position.y, position.z); }

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

public:
	// -- Utilities -- //

	void set_tile(const Coord& x, const Coord& y, const Coord& z, TileType* type);

	void update_all_tiles()
	{
		for (Coord x = 0; (Length)x < m_width; ++x) for (Coord y = 0; (Length)y < m_height; ++y) for (Coord z = 0; (Length)z < m_depth; ++z)
		{
			update_tile_at(x, y, z);
		}
	}

	void update_tile_at(const Coord& x, const Coord& y, const Coord& z);

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