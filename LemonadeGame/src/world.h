#pragma once

#ifndef LEMONADE_GAME_SRC_WORLD_H
#define LEMONADE_GAME_SRC_WORLD_H

#include "render_params.h"

#include "entity.h"
#include "time_units.h"
#include "tile_map.h"
#include "game_states.h"

class Game;
class WorldGameState;

inline constexpr size_t MAX_ENTITY_COUNT = 32;

class World : public std::enable_shared_from_this<World>
{
private:
	GameState* m_game_state;

	std::vector<std::unique_ptr<Entity>> m_entities;
	std::unique_ptr<TileMap> m_tile_map;

	// By default, starts at 12:00 PM on June 11, 2025
	TimeDate m_current_time = TimeDate
	{
		.time_point = TimeUnit::from_hours(12.0),
		.day = 11,
		.month = 6,
		.year = 2025
	};

public:
	World(GameState* game_state = {});

	// -- Getters & Setters -- //

	GameState* get_game_state() const;
	WorldGameState* get_world_game_state() const;
	Game* get_game() const;
	TileMap* get_tile_map() const { return m_tile_map.get(); }

	std::vector<Entity*> get_entities();

	void set_tile_map(std::unique_ptr<TileMap> tile_map) { m_tile_map = std::move(tile_map); m_tile_map->set_world(this); }

	TimeDate get_time() const { return m_current_time; }
	void set_time(const TimeDate& time) { m_current_time = time; }
	void add_time(TimeUnit time_delta) { m_current_time.add(time_delta); }

	// -- Utilities -- //

	void queue_remove_entity(Entity* entity);
	void remove_entity(Entity* entity);

	Entity* add_entity(std::unique_ptr<Entity> entity);
	Entity* add_entity_at(std::unique_ptr<Entity> entity, const Coord& x, const Coord& y, const Coord& z);
	Entity* add_entity_at(std::unique_ptr<Entity> entity, const Position& pos)
	{
		return add_entity_at(std::move(entity), pos.x, pos.y, pos.z);
	}

	std::vector<Entity*> get_entities_at(const Coord& x, const Coord& y, const Coord& z);
	std::vector<Entity*> get_entities_at(const Position& pos)
	{
		return get_entities_at(pos.x, pos.y, pos.z);
	}

	int count_entities_at(const Coord& x, const Coord& y, const Coord& z) const;
	int count_entities_at(const Position& pos)  const
	{
		return count_entities_at(pos.x, pos.y, pos.z);
	}

	// -- Checks -- //

	bool in_bounds(const Coord& x, const Coord& y, const Coord& z) const;
	bool in_bounds(const Position& pos) const
	{
		return in_bounds(pos.x, pos.y, pos.z);
	}

	Tile* get_tile(const Coord& x, const Coord& y, const Coord& z) const;
	Tile* get_tile(const Position& pos) const
	{
		return get_tile(pos.x, pos.y, pos.z);
	}

	bool has_blocking_entities_at(const Position& pos) const;
	bool has_blocking_entities_at(const Coord& x, const Coord& y, const Coord& z) const
	{
		return has_blocking_entities_at(Position(x, y, z));
	}

	std::vector<Entity*> get_blocking_entities_at(const Position& pos) const;
	std::vector<Entity*> get_blocking_entities_at(const Coord& x, const Coord& y, const Coord& z) const
	{
		return get_blocking_entities_at(Position(x, y, z));
	}

	bool is_blocked(const Coord& x, const Coord& y, const Coord& z, const Entity* fit = nullptr, const Coord& z_dir = 0);
	bool is_blocked(const Position& pos, const Entity* fit = nullptr, const Coord& z_dir = 0)
	{
		return is_blocked(pos.x, pos.y, pos.z, fit, z_dir);
	}

	bool is_opaque(const Coord& x, const Coord& y, const Coord& z, const Entity* fit = nullptr);
	bool is_opaque(const Position& pos, const Entity* fit = nullptr)
	{
		return is_opaque(pos.x, pos.y, pos.z, fit);
	}

	// -- Game Loop -- //

	void step(TimeUnit time_delta); // Tick the world's time forward

	// void render(RenderParams params);
};

#endif // !LEMONADE_GAME_SRC_WORLD_H