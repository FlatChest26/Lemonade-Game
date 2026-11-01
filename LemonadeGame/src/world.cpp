#include "world.h"
#include "world_state.h"
#include "game_settings.h"

#include <algorithm>

World::World(GameState* game_state) :
	m_game_state(game_state)
{
	m_entity_by_id.reserve(MAX_ENTITY_COUNT);
	m_entities.reserve(MAX_ENTITY_COUNT);
}

GameState* World::get_game_state() const
{
	if (auto game_state = m_game_state)
	{
		return game_state;
	}

	return nullptr;
}

WorldGameState* World::get_world_game_state() const
{
	if (auto game_state = get_game_state())
	{
		if (game_state->is_world_game_state())
		{
			return game_state->as_world_game_state();
		}
	}

	return nullptr;
}

Game* World::get_game() const
{
	if (auto game = m_game_state)
	{
		return game->get_game();
	}

	return nullptr;
}

std::vector<Entity*> World::get_entities() const
{
	return m_entities;
}

bool World::has_entity(const EntityID& ID) const
{
	return m_entity_by_id.contains(ID);
}

bool World::has_entity(const Entity* entity) const
{
	if (!entity)
	{
		CERR("Entity is null!");
		return false;
	}

	return has_entity(entity->ID());
}

EntityID World::get_id(const Entity* entity) const
{
	if (!entity)
	{
		CERR("Entity is null!");
		return EntityID();
	}

	if (!has_entity(entity->ID()))
	{
		CERR("Entity not found in world: " << entity->ID());
		return EntityID();
	}

	return entity->ID();
}

Entity* World::get_entity(const EntityID& ID) const
{
	if (has_entity(ID))
	{
		return m_entity_by_id.at(ID).get();
	}

	return nullptr;
}

Entity* World::add_entity(Entity* entity)
{
	if (m_entity_by_id.size() >= MAX_ENTITY_COUNT)
	{
		CERR("Max entity count reached!");
		return nullptr;
	}

	EntityID entity_ID = entity->ID();

	if (has_entity(entity_ID))
	{
		return nullptr;
	}

	m_entity_by_id[entity_ID] = std::unique_ptr<Entity>(entity);
	m_entity_by_id[entity_ID]->set_location(this);
	m_entities.push_back(m_entity_by_id[entity_ID].get());

	return m_entities.back();
}

Entity* World::add_entity_at(Entity* entity, Coord x, Coord y, Coord z)
{
	auto new_entity = add_entity(entity);

	if (!new_entity)
		return nullptr;

	if (!new_entity->has_movable())
	{
		new_entity->set_movable(Position(x, y, z));
	}
	else
	{
		new_entity->get_movable()->pos() = Position(x, y, z);
	}

	return new_entity;
}

void World::queue_remove(Entity* entity)
{
	queue_remove(get_id(entity));
}

void World::queue_remove(const EntityID& ID)
{
	m_entity_by_id.at(ID)->queue_remove();
}

bool World::in_bounds(Coord x, Coord y, Coord z) const
{
	if (!m_tile_map)
	{
		CERR("No tile map found on world!");
		return false;
	}

	return m_tile_map->in_bounds(x, y, z);
}

Tile* World::get_tile(Coord x, Coord y, Coord z) const
{
	if (!m_tile_map)
	{
		CERR("No tile map found on world!");
		return nullptr;
	}

	return m_tile_map->get_tile(x, y, z);
}

std::vector<Entity*> World::get_entities(const EntityFilter& filter) const
{
	std::vector<Entity*> entities_filtered;
	std::copy_if(
		m_entities.begin(), m_entities.end(), std::back_inserter(entities_filtered),
		[&filter](const auto& entity)
		{
			if (entity->should_remove())
				return false;

			if (filter.should_ignore(entity))
				return false;

			if (!filter.is_required(entity))
				return false;

			return true;
		}
	);

	return entities_filtered;
}

bool World::has_entities(const EntityFilter& filter) const
{
	return std::any_of(
		m_entities.begin(), m_entities.end(),
		[&filter](const auto& entity)
		{
			if (entity->should_remove())
				return false;

			if (filter.should_ignore(entity))
				return false;

			if (!filter.is_required(entity))
				return false;

			return true;
		}
	);
}

size_t World::count_entities(const EntityFilter& filter) const
{
	if (!m_entities.size())
		return 0;

	if (!filter.has_required() && !filter.has_ignore())
		return static_cast<size_t>(m_entities.size());

	auto count = std::count_if(
		m_entities.begin(), m_entities.end(),
		[&filter](const auto& entity)
		{
			if (entity->should_remove())
				return false;

			if (filter.should_ignore(entity))
				return false;

			if (!filter.is_required(entity))
				return false;

			return true;
		}
	);

	return static_cast<size_t>(count);
}

bool World::has_blocking_entities_at(Coord x, Coord y, Coord z, const EntityFilter& filter) const
{
	EntityFilter new_filter = filter;

	new_filter.required_flags |= EntityFlag::IS_BLOCKING;
	return has_entities_at(x, y, z, new_filter);
}

std::vector<Entity*> World::get_blocking_entities_at(Coord x, Coord y, Coord z, const EntityFilter& filter)
{
	EntityFilter new_filter = filter;

	new_filter.required_flags |= EntityFlag::IS_BLOCKING;
	return get_entities_at(x, y, z, new_filter);
}

std::vector<Entity*> World::get_entities_at(Coord x, Coord y, Coord z, const EntityFilter& filter)
{
	EntityFilter new_filter = filter;

	new_filter.required_positions.push_back(Position(x, y, z));
	return get_entities(new_filter);
}

size_t World::count_entities_at(Coord x, Coord y, Coord z, const EntityFilter& filter) const
{
	EntityFilter new_filter = filter;

	new_filter.required_positions.push_back(Position(x, y, z));
	return count_entities(new_filter);
}

bool World::has_entities_at(Coord x, Coord y, Coord z, const EntityFilter& filter) const
{
	EntityFilter new_filter = filter;

	new_filter.required_positions.push_back(Position(x, y, z));
	return has_entities(new_filter);
}

bool World::is_blocked(Coord x, Coord y, Coord z, const Entity* fit, Coord z_dir) const
{
	if (!m_tile_map)
	{
		CERR("No tile map found on world!");
		return true;
	}

	if (m_tile_map->get_tile(x, y, z)->is_blocked())
	{
		return true;
	}

	if (z_dir > 0) // Moving up
	{
		if (m_tile_map->get_tile(x, y, z)->has_floor()) // Check if destination tile has floor.
			return true;

		if (m_tile_map->get_tile(x, y, z - 1)->has_ceiling()) // Check if tile below destination tile has ceiling.
			return true;
	}

	if (z_dir < 0) // Moving down
	{
		if (m_tile_map->get_tile(x, y, z)->has_ceiling()) // Check if destination tile has ceiling.
			return true;

		if (m_tile_map->get_tile(x, y, z + 1)->has_floor()) // Check if tile above destination tile has floor.
			return true;
	}

	return has_entities(
		EntityFilter{
			.ignore_held = true,
			.ignore_entities{ fit },

			.required_flags = { EntityFlag::IS_BLOCKING },
			.required_positions = { Position(x, y, z) }
		}
	);
}

bool World::is_opaque(Coord x, Coord y, Coord z, const Entity* fit, Coord z_dir)  const
{
	if (!m_tile_map)
	{
		CERR("No tile map found on world!");
		return false;
	}

	if (m_tile_map->get_tile(x, y, z)->is_opaque())
	{
		return true;
	}

	if (z_dir > 0) // Moving up
	{
		if (m_tile_map->get_tile(x, y, z)->has_opaque_floor()) // Check if destination tile has floor.
			return true;

		if (m_tile_map->get_tile(x, y, z - 1)->has_opaque_ceiling()) // Check if tile below destination tile has ceiling.
			return true;
	}

	if (z_dir < 0) // Moving down
	{
		if (m_tile_map->get_tile(x, y, z)->has_opaque_ceiling()) // Check if destination tile has ceiling.
			return true;

		if (m_tile_map->get_tile(x, y, z + 1)->has_opaque_floor()) // Check if tile above destination tile has floor.
			return true;
	}

	return false;
}

void World::prepare_remove_entity(Entity* entity)
{
	if (auto inventory = entity->get_inventory())
	{
		for (auto& item : inventory->get_items())
		{
			queue_remove(item);
		}
	}

	if (auto entity_holder = entity->get_entity_holder())
	{
		entity_holder->remove_from_inventory(entity);
	}
}

void World::step(const TimeUnit& time_delta)
{
	std::vector<size_t> remove_entities;

	for (size_t idx = 0; idx < m_entities.size(); ++idx)
	{
		auto& entity = m_entities[idx];

		if (entity->should_remove())
		{
			remove_entities.push_back(idx);
		}
	}

	for (const auto& remove_idx : remove_entities)
	{
		auto& entity = m_entities[remove_idx];

		if (!has_entity(entity))
			continue;

		prepare_remove_entity(entity);

		m_entity_by_id.erase(entity->ID());
		m_entities.erase(m_entities.begin() + remove_idx);
	}

	for (const auto& [ID, entity] : m_entity_by_id)
	{
		if (!entity->should_remove())
		{
			entity->step(time_delta);
		}
	}

	m_current_time.add(time_delta * GameSettings::WORLD_RELATIVE_SPEED);
}