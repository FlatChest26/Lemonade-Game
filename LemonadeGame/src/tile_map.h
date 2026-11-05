#pragma once

#ifndef LEMONADE_GAME_SRC_TILE_MAP_H
#define LEMONADE_GAME_SRC_TILE_MAP_H

#include "tile.h"
#include "time_units.h"
#include "render_params.h"
#include "tile_type.h"
#include "raw_master.h"
#include "material_def.h"

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
	std::vector<Tile> m_tiles{};
	std::vector<Material> m_materials{};

	Length m_width{ DEFAULT_MAP_WIDTH };
	Length m_height{ DEFAULT_MAP_HEIGHT };
	Length m_depth{ DEFAULT_MAP_DEPTH };

	Tile m_void_tile;
	Material m_null_material;

	std::vector<std::unique_ptr<TCODMap>> m_tcod_map_layers{};

	bool m_should_update_path{ false };
	bool m_should_update_fov{ false };

public:
	TileMap(World* world = {}, Length width = DEFAULT_MAP_WIDTH, Length height = DEFAULT_MAP_HEIGHT, Length depth = DEFAULT_MAP_DEPTH);
	~TileMap();

public:
	// -- Getters & Setters -- //

	World* get_world();
	void set_world(World* world);

	// Returns the width, height, and depth of this tile map as a `Size`.
	constexpr Size get_size() const { return Size(m_width, m_height, m_depth); }

	// Returns the width of this tile map.
	constexpr Length get_width() const { return m_width; }
	// Returns the height of this tile map.
	constexpr Length get_height() const { return m_height; }
	// Returns the depth of this tile map.
	constexpr Length get_depth() const { return m_depth; }

	// Returns a pointer to the `Tile` at the given coordinates. If the coordinates are out of bounds, returns a pointer to the void tile.
	Tile* get_tile(Coord x, Coord y, Coord z);
	// Returns a pointer to the `Tile` at the given position. If the position is out of bounds, returns a pointer to the void tile.
	Tile* get_tile(const Position& position) { return get_tile(position.x, position.y, position.z); }

	// Returns a constant pointer to the `Tile` at the given coordinates. If the coordinates are out of bounds, returns a constant pointer to the void tile.
	const Tile* get_tile(Coord x, Coord y, Coord z) const;
	// Returns a constant pointer to the `Tile` at the given position. If the position is out of bounds, returns a constant pointer to the void tile.
	const Tile* get_tile(const Position& position) const { return get_tile(position.x, position.y, position.z); }

	// Returns a constant pointer to the `Material` at the given coordinates.
	Material& get_material(Coord x, Coord y, Coord z);
	// Returns a constant pointer to the `Material` at the given position.
	Material& get_material(const Position& position) { return get_material(position.x, position.y, position.z); }

	// Returns a constant pointer to the `Material` at the given coordinates.
	const Material& get_material(Coord x, Coord y, Coord z) const;
	// Returns a constant pointer to the `Material` at the given position.
	const Material& get_material(const Position& position) const { return get_material(position.x, position.y, position.z); }

	// Returns true if the given coordinates are in bounds.
	constexpr bool in_bounds(Coord x, Coord y, Coord z) const;
	// Returns true if the given position is in bounds.
	constexpr bool in_bounds(const Position& position) const { return in_bounds(position.x, position.y, position.z); }

	// Returns the TCODMap at the given z layer.
	TCODMap* get_tcod_map_layer(const size_t& layer) { return m_tcod_map_layers[layer].get(); }
	// Returns the TCODMap at the given z layer.
	const TCODMap* get_tcod_map_layer(const size_t& layer) const { return get_tcod_map_layer(layer); }

	// Returns the index of the tile at the given coordinates. `index = (x * height * depth) + (y * depth) + z`
	constexpr size_t get_index(Coord x, Coord y, Coord z) const;
	// Returns the index of the tile at the given position. `index = (x * height * depth) + (y * depth) + z`
	constexpr size_t get_index(const Position& position) const { return get_index(position.x, position.y, position.z); }

public:
	// -- Utilities -- //

	// Sets the tile at the given coordinates to the type, and optionally updates the tiles around it.
	void set_tile(Coord x, Coord y, Coord z, RawMaster::Ref<TileType> type, bool update = true);
	// Sets the tile at the given position to the type, and optionally updates the tiles around it.
	void set_tile(const Position& pos, RawMaster::Ref<TileType> type, bool update = true) { set_tile(pos.x, pos.y, pos.z, type, update); }

	// Sets the material at the given coordinates to the type, and optionally updates the tiles around it.
	void set_material(Coord x, Coord y, Coord z, RawMaster::Ref<MaterialType> type, MaterialAmount amount = 1, units::Temperature temperature = 60.0_degF, bool update = true);
	void set_material(const Position& pos, RawMaster::Ref<MaterialType> type, MaterialAmount amount = 1, units::Temperature temperature = 60.0_degF, bool update = true)
	{
		set_material(pos.x, pos.y, pos.z, type, amount, temperature, update);
	}

	void set_temperature(Coord x, Coord y, Coord z, units::Temperature temperature, bool update = true);
	void set_temperature(const Position& pos, units::Temperature temperature, bool update = true)
	{
		set_temperature(pos.x, pos.y, pos.z, temperature, update);
	}

	void add_material(Coord x, Coord y, Coord z, RawMaster::Ref<MaterialType> type, MaterialAmount amount, bool update = true);
	void add_material(const Position& pos, RawMaster::Ref<MaterialType> type, MaterialAmount amount, bool update = true)
	{
		add_material(pos.x, pos.y, pos.z, type, amount, update);
	}

	// Updates the tile at the given coordinates.
	void update_tile_at(Coord x, Coord y, Coord z);
	// Updates the tile at the given position.
	void update_tile_at(const Position& pos) { update_tile_at(pos.x, pos.y, pos.z); }

	// Field of View //

	// Computes what tiles would be visible from the given pov and radius.
	void compute_fov(Coord pov_x, Coord pov_y, Coord pov_z, Coord radius);
	// Marks a tile as being visible.
	void fov_mark_visible(Coord x, Coord y, Coord z, bool visible = true);

	// -- Checks -- //

	// Returns true if the given coordinates are visible to the previously computed FOV.
	bool is_visible(Coord x, Coord y, Coord z) const;
	// Returns true if the given position is visible to the previously computed FOV.
	bool is_visible(const Position& pos) const { return is_visible(pos.x, pos.y, pos.z); }

	// Returns true if the given coordinate were once visible to a previously computed FOV.
	bool is_explored(Coord x, Coord y, Coord z) const;
	// Returns true if the given position was once visible to a previously computed FOV.
	bool is_explored(const Position& pos) const { return is_explored(pos.x, pos.y, pos.z); }

	bool should_update_fov() const { return m_should_update_fov; }
	void request_fov_update() { m_should_update_fov = true; }

	bool should_update_path() const { return m_should_update_path; }
	void set_update_path(bool should_update_path) { m_should_update_path = should_update_path; }
};

#endif // !LEMONADE_GAME_SRC_TILE_MAP_H