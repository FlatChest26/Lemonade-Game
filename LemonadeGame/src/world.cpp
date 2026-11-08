#include "world.h"
#include "world_state.h"
#include "game_settings.h"

#include <algorithm>

void EntityFilterCache::reserve(size_t size)
{
	all_entities.reserve(size);
	all_creatures.reserve(size);
	all_items.reserve(size);
	all_furniture.reserve(size);
	all_blocking.reserve(size);
	all_opaque.reserve(size);
}

void EntityFilterCache::clear()
{
	all_entities.clear();
	all_creatures.clear();
	all_items.clear();
	all_furniture.clear();
	all_blocking.clear();
	all_opaque.clear();
}

void EntityFilterCache::rebuild(std::vector<Entity*> entities)
{
	clear();

	all_entities.insert(all_entities.end(), entities.begin(), entities.end());

	for (auto& entity : entities)
	{
		add(entity);
	}
}

void EntityFilterCache::add(Entity* entity)
{
	if (std::find(all_entities.begin(), all_entities.end(), entity) != all_entities.end())
		return;

	all_entities.push_back(entity);

	if (entity->is_creature())
	{
		all_creatures.push_back(entity);
	}

	if (entity->is_item())
	{
		all_items.push_back(entity);
	}

	if (entity->is_furniture())
	{
		all_furniture.push_back(entity);
	}

	if (entity->is_blocking())
	{
		all_blocking.push_back(entity);
	}

	if (entity->is_opaque())
	{
		all_opaque.push_back(entity);
	}
}

void EntityFilterCache::remove(Entity* entity)
{
	if (std::find(all_entities.begin(), all_entities.end(), entity) == all_entities.end())
		return;

	auto check = [&](const std::vector<Entity*>& vec)
		{
			return std::find(vec.begin(), vec.end(), entity) != vec.end();
		};

	std::erase(all_entities, entity);

	if (entity->is_creature() || check(all_creatures))
	{
		std::erase(all_creatures, entity);
	}

	if (entity->is_item() || check(all_items))
	{
		std::erase(all_items, entity);
	}

	if (entity->is_furniture() || check(all_furniture))
	{
		std::erase(all_furniture, entity);
	}

	if (entity->is_blocking() || check(all_blocking))
	{
		std::erase(all_blocking, entity);
	}

	if (entity->is_opaque() || check(all_opaque))
	{
		std::erase(all_opaque, entity);
	}
}

const std::vector<Entity*>& EntityFilterCache::get_cache(const EntityFilter& filter) const
{
	if (filter.required_flags & EntityFlag::IS_CREATURE)
	{
		return all_creatures;
	}
	else if (filter.required_flags & EntityFlag::IS_ITEM)
	{
		return all_items;
	}
	else if (filter.required_flags & EntityFlag::IS_FURNITURE)
	{
		return all_furniture;
	}
	else if (filter.required_flags & EntityFlag::IS_BLOCKING)
	{
		return all_blocking;
	}
	else if (filter.required_flags & EntityFlag::IS_OPAQUE)
	{
		return all_opaque;
	}

	return all_entities;
}

World::World(WorldGameState* world_state) :
	m_world_state(world_state)
{
	m_entity_by_id.reserve(MAX_ENTITY_COUNT);
	m_entities.reserve(MAX_ENTITY_COUNT);
	m_entity_filter_cache.reserve(MAX_ENTITY_COUNT);
}

WorldGameState* World::get_world_state() const
{
	return m_world_state;
}

Game* World::get_game() const
{
	if (auto world_state = get_world_state())
	{
		return world_state->get_game();
	}

	return nullptr;
}

EntityID World::get_next_entity_id(Entity* entity)
{
	m_entity_id_count++;

	std::string type_name = "ENTITY";

	if (entity)
	{
		std::string def_id = "";

		if (entity->is_creature())
		{
			type_name = fmt::format("CREATURE");

			if (entity->has_species())
			{
				def_id = entity->get_species()->ID;
			}
		}
		else if (entity->is_item())
		{
			type_name = fmt::format("ITEM");

			if (entity->has_item_def())
			{
				def_id = entity->get_item_def()->ID;
			}
		}
		else if (entity->is_furniture())
		{
			type_name = fmt::format("FURNITURE");

			if (entity->has_furniture_def())
			{
				def_id = entity->get_furniture_def()->ID;
			}
		}

		if (!def_id.empty())
		{
			type_name = fmt::format("{}_{}", type_name, def_id);
		}
	}

	return fmt::format("{}_{}", type_name, m_entity_id_count);
}

bool World::has_entity(const EntityID& ID) const
{
	return m_entity_by_id.contains(ID);
}

bool World::has_entity(const Entity& entity) const
{
	return has_entity(entity.ID());
}

bool World::has_entity(const Entity* entity) const
{
	return has_entity(entity->ID());
}

EntityID World::get_id(const Entity& entity) const
{
	return get_id(&entity);
}

EntityID World::get_id(const Entity* entity) const
{
	if (!has_entity(entity->ID()))
	{
		CERR("Entity not found in world: " << entity->ID());
		return EntityID();
	}

	return entity->ID();
}

Entity* World::get_entity(const EntityID& ID) const
{
	if (!m_entity_by_id.contains(ID))
	{
		CERR("Entity not found in world: " << ID);
		return nullptr;
	}

	return m_entities.at(m_entity_by_id.at(ID)).get();
}

Entity* World::add_entity(Entity* entity)
{
	if (!entity)
	{
		CERR("entity is null!");
		return nullptr;
	}

	if (m_entity_by_id.size() >= MAX_ENTITY_COUNT)
	{
		CERR("Max entity count reached!");
		return nullptr;
	}

	if (entity->ID().empty())
	{
		entity->set_ID(get_next_entity_id(entity));
	}

	EntityID entity_ID = entity->ID();

	if (has_entity(entity_ID))
	{
		CERR("Entity already part of world! " << entity_ID);
		return nullptr;
	}

	size_t entity_idx = m_entities.size();

	m_entities.push_back(std::unique_ptr<Entity>(entity));
	m_entities.back()->set_location(this);
	m_entity_filter_cache.add(entity);

	m_entity_by_id[entity_ID] = entity_idx;

	return m_entities.back().get();
}

Entity* World::add_entity(const Entity& entity)
{
	return add_entity(new Entity(entity));
}

Entity* World::add_entity_at(Entity* entity, Coord x, Coord y, Coord z, Direction dir)
{
	Entity* new_entity = add_entity(entity);

	if (!new_entity)
		return nullptr;

	if (new_entity->has_movable())
	{
		new_entity->get_movable()->pos() = Position(x, y, z);
	}
	else
	{
		new_entity->set_movable(Position(x, y, z));
	}

	new_entity->get_movable()->direction() = dir;

	return new_entity;
}

Entity* World::add_entity_at(const Entity& entity, Coord x, Coord y, Coord z, Direction dir)
{
	return add_entity_at(new Entity(entity), x, y, z, dir);
}

void World::queue_remove(Entity* entity)
{
	queue_remove(get_id(entity));
}

void World::queue_remove(Entity& entity)
{
	queue_remove(get_id(entity));
}

void World::queue_remove(const EntityID& ID)
{
	m_entities.at(m_entity_by_id.at(ID))->queue_remove();
}

std::vector<Entity*> World::get_entities(const EntityFilter& filter) const
{
	if (m_entities.empty())
		return {};

	std::vector<Entity*> entities_filtered;
	auto& entities = m_entity_filter_cache.get_cache(filter);

	std::copy_if(
		entities.begin(), entities.end(), std::back_inserter(entities_filtered),
		[&filter](const auto& entity)
		{
			if (entity->should_remove())
				return false;

			return filter.check(entity);
		}
	);

	return entities_filtered;
}

bool World::has_entities(const EntityFilter& filter) const
{
	if (m_entities.empty())
		return false;

	if (!filter.has_required() && !filter.has_ignore())
		return false;

	auto& entities = m_entity_filter_cache.get_cache(filter);

	return std::any_of(
		entities.begin(), entities.end(),
		[&filter](const auto& entity)
		{
			if (entity->should_remove())
				return false;

			return filter.check(entity);
		}
	);
}

size_t World::count_entities(const EntityFilter& filter) const
{
	if (m_entities.empty())
		return 0;

	if (!filter.has_required() && !filter.has_ignore())
		return static_cast<size_t>(m_entities.size());

	auto& entities = m_entity_filter_cache.get_cache(filter);

	auto count = std::count_if(
		entities.begin(), entities.end(),
		[&filter](auto& entity)
		{
			if (entity->should_remove())
				return false;

			return filter.check(entity);
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

std::vector<Entity*> World::get_blocking_entities_at(Coord x, Coord y, Coord z, const EntityFilter& filter) const
{
	EntityFilter new_filter = filter;

	new_filter.required_flags |= EntityFlag::IS_BLOCKING;
	return get_entities_at(x, y, z, new_filter);
}

std::vector<Entity*> World::get_entities_at(Coord x, Coord y, Coord z, const EntityFilter& filter) const
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

bool World::is_blocked(Coord x, Coord y, Coord z, const Entity* fit, Coord z_dir) const
{
	if (!m_tile_map)
	{
		CERR("No tile map found on world!");
		return true;
	}

	if (m_tile_map->is_blocked(x, y, z))
	{
		return true;
	}

	if (z_dir > 0) // Moving up
	{
		if (m_tile_map->get_tile(x, y, z)->has_floor() || m_tile_map->is_blocked(x, y, z)) // Check if destination tile has floor.
			return true;

		if (m_tile_map->get_tile(x, y, z - 1)->has_ceiling() || m_tile_map->is_blocked(x, y, z - 1)) // Check if tile below destination tile has ceiling.
			return true;
	}

	if (z_dir < 0) // Moving down
	{
		if (m_tile_map->get_tile(x, y, z)->has_ceiling() || m_tile_map->is_blocked(x, y, z)) // Check if destination tile has ceiling.
			return true;

		if (m_tile_map->get_tile(x, y, z + 1)->has_floor() || m_tile_map->is_blocked(x, y, z + 1)) // Check if tile above destination tile has floor.
			return true;
	}

	return has_entities(
		EntityFilter{
			.ignore_held = true,
			.ignore_ids{ fit != nullptr ? fit->ID() : "" },

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

	if (m_tile_map->is_opaque(x, y, z))
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

	return has_entities(
		EntityFilter{
			.ignore_held = true,
			.ignore_ids{ fit != nullptr ? fit->ID() : "" },

			.required_flags = { EntityFlag::IS_OPAQUE },
			.required_positions = { Position(x, y, z) }
		}
	);
}

void World::prepare_remove_entity(Entity* entity)
{
	if (entity->has_inventory())
	{
		entity->get_inventory()->clear_items(true);
	}

	if (auto entity_holder = entity->get_entity_holder())
	{
		entity_holder->remove_from_inventory(entity);
	}
}

void World::remove_entity(size_t idx)
{
	if (idx >= m_entities.size())
	{
		CERR("Invalid remove index: " << idx);
		return;
	}

	auto entity = m_entities[idx].get();
	m_entity_filter_cache.remove(entity);

	prepare_remove_entity(entity);

	m_entity_by_id.erase(entity->ID());

	if (m_entities[idx] != m_entities.back()) // Swap with back
	{
		EntityID swap_id = m_entities.back()->ID();

		std::swap(m_entities[idx], m_entities.back());

		m_entity_by_id[swap_id] = idx;
	}

	m_entities.pop_back();
}

void World::step(const TimeUnit& time_delta)
{
	// Step tile map

	if (m_tile_map)
	{
		m_tile_map->step(time_delta);
	}

	// Step entities

	std::vector<size_t> remove_entities;

	for (auto& entity : m_entities)
	{
		if (entity->should_remove())
		{
			remove_entities.push_back(m_entity_by_id.at(entity->ID()));
			continue;
		}

		entity->step(time_delta);
	}

	// Remove entities from the world
	for (size_t remove_idx : std::views::reverse(remove_entities))
	{
		remove_entity(remove_idx);
	}

	// Step current time
	m_current_time.add(time_delta * GameSettings::WORLD_RELATIVE_SPEED);
}


