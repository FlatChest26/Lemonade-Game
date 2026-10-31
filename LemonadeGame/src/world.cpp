#include "world.h"
#include "world_state.h"
#include "game_settings.h"

#include <algorithm>

World::World(GameState* game_state) :
	m_game_state(game_state)
{
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

std::vector<Entity*> World::get_entities()
{
	std::vector<Entity*> entities;

	for (auto& [ID, entity] : m_entities)
	{
		entities.push_back(entity.get());
	}

	return entities;
}

std::vector<const Entity*> World::get_entities() const
{
	std::vector<const Entity*> entities;

	for (auto& [ID, entity] : m_entities)
	{
		entities.push_back(entity.get());
	}

	return entities;
}

void World::queue_remove_entity(Entity* entity)
{
	queue_remove_entity(get_entity_ID(entity));
}

void World::queue_remove_entity(EntityID ID)
{
	if (has_entity(ID))
	{
		auto entity = get_entity(ID);
		entity->queue_remove();
	}
}

bool World::has_entity(EntityID ID) const
{
	return m_entities.find(ID) != m_entities.end();
}

EntityID World::get_entity_ID(Entity* entity) const
{
	for (const auto& [check_ID, check_entity] : m_entities)
	{
		if (*check_entity == *entity)
		{
			return check_ID;
		}
	}

	return EntityID();
}

Entity* World::get_entity(EntityID ID)
{
	if (has_entity(ID))
	{
		return m_entities[ID].get();
	}

	return nullptr;
}

const Entity* World::get_entity(EntityID ID) const
{
	if (m_entities.find(ID) != m_entities.end())
	{
		return m_entities.at(ID).get();
	}

	return nullptr;
}

Entity* World::add_entity(Entity* entity)
{
	if (m_entities.size() >= MAX_ENTITY_COUNT)
	{
		CERR("Max entity count reached!");
		return nullptr;
	}

	EntityID entity_ID = entity->ID();

	if (has_entity(entity_ID))
	{
		return nullptr;
	}

	m_entities[entity_ID] = std::unique_ptr<Entity>(entity);
	m_entities[entity_ID]->set_location(this);

	if (m_entities[entity_ID]->is_creature())
	{
		m_creatures.push_back(entity_ID);
	}

	if (m_entities[entity_ID]->is_item())
	{
		m_items.push_back(entity_ID);
	}

	if (m_entities[entity_ID]->is_furniture())
	{
		m_furniture.push_back(entity_ID);
	}

	return m_entities[entity_ID].get();
}

Entity* World::add_entity_at(Entity* entity, const Coord& x, const Coord& y, const Coord& z)
{
	auto new_entity = add_entity(entity);

	if (new_entity == nullptr)
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

bool World::in_bounds(const Coord& x, const Coord& y, const Coord& z) const
{
	if (!m_tile_map)
	{
		CERR("No tile map found on world!");
		return false;
	}

	return m_tile_map->in_bounds(x, y, z);
}

Tile* World::get_tile(const Coord& x, const Coord& y, const Coord& z) const
{
	if (!m_tile_map)
	{
		CERR("No tile map found on world!");
		return nullptr;
	}

	return m_tile_map->get_tile(x, y, z);
}

std::vector<Entity*> World::get_entities(const EntityFilter& filter)
{
	std::vector<Entity*> entities_filtered;
	auto entities = get_entities();

	std::copy_if(
		entities.begin(), entities.end(), std::back_inserter(entities_filtered),
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

std::vector<const Entity*> World::get_entities(const EntityFilter& filter) const
{
	std::vector<const Entity*> entities_filtered;
	auto entities = get_entities();

	std::copy_if(
		entities.begin(), entities.end(), std::back_inserter(entities_filtered),
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
		[&filter](const auto& pair)
		{
			if (pair.second->should_remove())
				return false;

			if (filter.should_ignore(pair.second.get()))
				return false;

			if (!filter.is_required(pair.second.get()))
				return false;

			return true;
		}
	);
}

int World::count_entities(const EntityFilter& filter) const
{
	auto count = std::count_if(
		m_entities.begin(), m_entities.end(),
		[&filter](const auto& pair)
		{
			if (pair.second->should_remove())
				return false;

			if (filter.should_ignore(pair.second.get()))
				return false;

			if (!filter.is_required(pair.second.get()))
				return false;

			return true;
		}
	);

	return static_cast<int>(count);
}

bool World::has_blocking_entities_at(const Coord& x, const Coord& y, const Coord& z, const EntityFilter& filter) const
{
	EntityFilter new_filter = filter;

	new_filter.required_flags |= EntityFlag::IS_BLOCKING;
	return has_entities_at(x, y, z, new_filter);
}

std::vector<Entity*> World::get_blocking_entities_at(const Coord& x, const Coord& y, const Coord& z, const EntityFilter& filter)
{
	EntityFilter new_filter = filter;

	new_filter.required_flags |= EntityFlag::IS_BLOCKING;
	return get_entities_at(x, y, z, new_filter);
}

std::vector<Entity*> World::get_entities_at(const Coord& x, const Coord& y, const Coord& z, const EntityFilter& filter)
{
	EntityFilter new_filter = filter;

	new_filter.required_positions.push_back(Position(x, y, z));
	return get_entities(new_filter);
}

int World::count_entities_at(const Coord& x, const Coord& y, const Coord& z, const EntityFilter& filter) const
{
	EntityFilter new_filter = filter;

	new_filter.required_positions.push_back(Position(x, y, z));
	return count_entities(new_filter);
}

bool World::has_entities_at(const Coord& x, const Coord& y, const Coord& z, const EntityFilter& filter) const
{
	EntityFilter new_filter = filter;

	new_filter.required_positions.push_back(Position(x, y, z));
	return has_entities(new_filter);
}

bool World::is_blocked(const Coord& x, const Coord& y, const Coord& z, const Entity* fit, const Coord& z_dir) const
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

bool World::is_opaque(const Coord& x, const Coord& y, const Coord& z, const Entity* fit, const Coord& z_dir)  const
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

void World::prepare_remove_entity(Entity* entity)  const
{
	if (auto entity_holder = entity->get_entity_holder())
	{
		entity_holder->remove_from_inventory(entity);
	}
}

void World::step(const TimeUnit& time_delta)
{
	std::vector<EntityID> remove_entities;

	for (const auto& [ID, entity] : m_entities)
	{
		if (entity->should_remove())
		{
			remove_entities.push_back(ID);
		}
	}

	for (const auto& remove_idx : remove_entities)
	{
		if (!has_entity(remove_idx))
			continue;

		prepare_remove_entity(m_entities[remove_idx].get());
		m_entities.erase(remove_idx);

		if (auto find_idx = std::find(m_creatures.begin(), m_creatures.end(), remove_idx) != m_creatures.end())
		{
			m_creatures.erase(m_creatures.begin() + find_idx);
		}

		if (auto find_idx = std::find(m_items.begin(), m_items.end(), remove_idx) != m_items.end())
		{
			m_items.erase(m_items.begin() + find_idx);
		}

		if (auto find_idx = std::find(m_furniture.begin(), m_furniture.end(), remove_idx) != m_furniture.end())
		{
			m_furniture.erase(m_furniture.begin() + find_idx);
		}
	}

	for (const auto& [ID, entity] : m_entities)
	{
		if (!entity->should_remove())
		{
			entity->step(time_delta);
		}
	}

	m_current_time.add(time_delta * GameSettings::WORLD_RELATIVE_SPEED);
}