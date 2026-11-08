#pragma once

#ifndef LEMONADE_GAME_SRC_WORLD_H
#define LEMONADE_GAME_SRC_WORLD_H

#include "entity_id.h"

#include "entity.h"
#include "time_units.h"
#include "tile_map.h"
#include "game_states.h"

#include "entity_filter.h"

#include <unordered_map>

class Game;
class WorldGameState;

inline constexpr size_t MAX_ENTITY_COUNT = 64;

struct EntityFilterCache
{
	std::vector<Entity*> all_entities;
	std::vector<Entity*> all_creatures;
	std::vector<Entity*> all_items;
	std::vector<Entity*> all_furniture;

	std::vector<Entity*> all_blocking;
	std::vector<Entity*> all_opaque;

	void reserve(size_t size);
	void clear();
	void rebuild(std::vector<Entity*> entities);
	void add(Entity* entity);
	void remove(Entity* entity);

	const std::vector<Entity*>& get_cache(const EntityFilter& filter) const;
};

struct Ambience
{
	units::Temperature temperature{ 60_degF };
};

class World
{
private:
	WorldGameState* m_world_state;

	size_t m_entity_id_count;

	std::vector<std::unique_ptr<Entity>> m_entities;
	std::unordered_map<EntityID, size_t> m_entity_by_id;

	EntityFilterCache m_entity_filter_cache;

	std::unique_ptr<TileMap> m_tile_map;

	// By default, starts at 12:00 PM on June 11, 2025
	TimeDate m_current_time = TimeDate(TimeUnit::from_hours(12.0), 11, 6, 2025);

	Ambience m_ambience;

public:
	World(WorldGameState* world_state);

	// -- Getters & Setters -- //

	WorldGameState* get_world_state() const;
	Game* get_game() const;

	TileMap* get_tile_map() const { return m_tile_map.get(); }
	void set_tile_map(TileMap* tile_map) { m_tile_map = std::unique_ptr<TileMap>(tile_map); m_tile_map->set_world(this); }

	TimeDate get_time() const { return m_current_time; }
	void set_time(const TimeDate& time) { m_current_time = time; }
	void add_time(const TimeUnit& time_delta) { m_current_time.add(time_delta); }

	const Ambience& get_ambience() const { return m_ambience; }

public:
	// -- Entity Utilities -- //

	EntityID get_next_entity_id(Entity* entity = nullptr);

	// Adds an entity to the world. Returns a pointer to the entity if successful.
	Entity* add_entity(const Entity& entity);
	Entity* add_entity(Entity* entity);

	// Adds an entity to the world at the given coordinates. Returns a pointer to the entity if successful.
	Entity* add_entity_at(const Entity& entity, Coord x, Coord y, Coord z, Direction dir = NORTH);
	Entity* add_entity_at(Entity* entity, Coord x, Coord y, Coord z, Direction dir = NORTH);
	// Adds an entity to the world at the given position. Returns a pointer to the entity if successful.
	Entity* add_entity_at(const Entity& entity, const Position& pos, Direction dir = NORTH) { return add_entity_at(entity, pos.x, pos.y, pos.z, dir); }
	Entity* add_entity_at(Entity* entity, const Position& pos, Direction dir = NORTH) { return add_entity_at(entity, pos.x, pos.y, pos.z, dir); }

	// Sets the given entity to be removed on the next frame.
	void queue_remove(Entity& entity);
	void queue_remove(Entity* entity);
	void queue_remove(const EntityID& ID);

	// -- Entity Filter -- //

	bool has_entity(const EntityID& ID) const;
	bool has_entity(const Entity& entity) const;
	bool has_entity(const Entity* entity) const;

	EntityID get_id(const Entity& entity) const;
	EntityID get_id(const Entity* entity) const;

	Entity* get_entity(const EntityID& ID) const;

	std::vector<Entity*> get_entities(const EntityFilter& filter = {}) const;

	bool has_entities(const EntityFilter& filter) const;
	size_t count_entities(const EntityFilter& filter) const;

	// Returns a vector of entities at the given coordinates. EntityFilter is used to ignore certain entities.
	std::vector<Entity*> get_entities_at(Coord x, Coord y, Coord z, const EntityFilter& filter = { .ignore_held = true }) const;
	// Returns a vector of entities at the given position.
	std::vector<Entity*> get_entities_at(const Position& pos, const EntityFilter& filter = { .ignore_held = true }) const
	{
		return get_entities_at(pos.x, pos.y, pos.z, filter);
	}

	size_t count_entities_at(Coord x, Coord y, Coord z, const EntityFilter& filter = { .ignore_held = true }) const;
	size_t count_entities_at(const Position& pos, const EntityFilter& filter = { .ignore_held = true }) const { return count_entities_at(pos.x, pos.y, pos.z, filter); }

	bool has_entities_at(Coord x, Coord y, Coord z, const EntityFilter& filter = { .ignore_held = true }) const;
	bool has_entities_at(const Position& pos, const EntityFilter& filter = { .ignore_held = true }) const { return has_entities_at(pos.x, pos.y, pos.z, filter); }

	bool has_blocking_entities_at(Coord x, Coord y, Coord z, const EntityFilter& filter = { .ignore_held = true }) const;
	bool has_blocking_entities_at(const Position& pos, const EntityFilter& filter = { .ignore_held = true }) const { return has_blocking_entities_at(pos.x, pos.y, pos.z, filter); }

	std::vector<Entity*> get_blocking_entities_at(Coord x, Coord y, Coord z, const EntityFilter& filter = { .ignore_held = true }) const;
	std::vector<Entity*> get_blocking_entities_at(const Position& pos, const EntityFilter& filter = { .ignore_held = true }) const
	{
		return get_blocking_entities_at(pos.x, pos.y, pos.z, filter);
	}

public:
	// -- Tile Map Utilities -- //

	bool in_bounds(Coord x, Coord y, Coord z) const;
	bool in_bounds(const Position& pos) const
	{
		return in_bounds(pos.x, pos.y, pos.z);
	}

	Tile* get_tile(Coord x, Coord y, Coord z) const;
	Tile* get_tile(const Position& pos) const
	{
		return get_tile(pos.x, pos.y, pos.z);
	}

	bool is_blocked(Coord x, Coord y, Coord z, const Entity* fit = nullptr, Coord z_dir = 0) const;
	bool is_blocked(const Position& pos, const Entity* fit = nullptr, Coord z_dir = 0) const
	{
		return is_blocked(pos.x, pos.y, pos.z, fit, z_dir);
	}

	bool is_opaque(Coord x, Coord y, Coord z, const Entity* fit = nullptr, Coord z_dir = 0) const;
	bool is_opaque(const Position& pos, const Entity* fit = nullptr, Coord z_dir = 0) const
	{
		return is_opaque(pos.x, pos.y, pos.z, fit, z_dir);
	}

public:
	// -- Game Loop -- //

	void step(const TimeUnit& time_delta); // Tick the world's time forward

protected:
	void prepare_remove_entity(Entity* entity);
	void remove_entity(size_t idx);
	
};

#endif // !LEMONADE_GAME_SRC_WORLD_H