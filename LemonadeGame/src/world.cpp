#include "world.h"
#include "world_state.h"

World::World(GameState* game_state) :
	m_game_state(game_state)
{
}

void World::queue_remove_entity(Entity* entity)
{
	for (auto& check_entity : m_entities)
	{
		if (check_entity->ID() == entity->ID())
		{
			check_entity->queue_remove();
		}
	}
}

void World::remove_entity(Entity* entity)
{
	if (auto entity_holder = entity->get_entity_holder())
	{
		entity_holder->remove_from_inventory(entity);
	}

	if (auto brain = get_world_game_state()->get_ai_manager()->get_entity_brain(entity))
	{
		brain->queue_remove();
	}
}

Entity* World::add_entity(std::unique_ptr<Entity> entity)
{
	if (m_entities.size() >= MAX_ENTITY_COUNT)
		return nullptr;

	m_entities.push_back(std::move(entity));
	m_entities.back()->set_location(this);

	return m_entities.back().get();
}

Entity* World::add_entity_at(std::unique_ptr<Entity> entity, const Coord& x, const Coord& y, const Coord& z)
{
	auto new_entity = add_entity(std::move(entity));
	if (new_entity == nullptr)
		return nullptr;

	new_entity->movable()->set_position(x, y, z);

	return new_entity;
}

std::vector<Entity*> World::get_entities_at(const Coord& x, const Coord& y, const Coord& z)
{
	std::vector<Entity*>  entities_at_location;

	for (const auto& entity : m_entities)
	{
		if (!entity->has_movable())
			continue;

		if (entity->get_movable()->get_pos() == Position(x, y, z))
		{
			entities_at_location.push_back(entity.get());
		}
	}

	return entities_at_location;
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

	for (auto& entity : m_entities)
	{
		entities.push_back(entity.get());
	}

	return entities;
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

bool World::has_blocking_entities_at(const Position& pos) const
{
	auto blocking_entity = std::find_if(
		m_entities.begin(), m_entities.end(),
		[&pos](const auto& entity)
		{
			return entity->has_movable() && entity->is_blocking() && entity->get_movable()->contains_point(pos);
		}
	);

	return blocking_entity != m_entities.end();
}

std::vector<Entity*> World::get_blocking_entities_at(const Position& pos) const
{
	std::vector<Entity*> entities_at;

	for (const auto& entity : m_entities)
	{
		if (!entity->has_movable())
			continue;
		
		if (entity->is_blocking() && entity->get_movable()->contains_point(pos))
		{
			entities_at.push_back(entity.get());
		}
	}

	return entities_at;
}

int World::count_entities_at(const Coord& x, const Coord& y, const Coord& z) const
{
	Position pos{ x, y, z };

	auto count = std::count_if(
		m_entities.begin(), m_entities.end(),
		[&pos](const auto& entity)
		{
			return entity->has_movable() && entity->is_blocking() && entity->get_movable()->contains_point(pos);
		}
	);

	return static_cast<int>(count);
}

bool World::is_blocked(const Coord& x, const Coord& y, const Coord& z, const Entity* fit, const Coord& z_dir)
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

	if (std::abs(z_dir) > 0)
	{
		if (z_dir > 0 && m_tile_map->get_tile(x, y, z)->has_floor())
			return true;

		if (z_dir < 0 && m_tile_map->get_tile(x, y, z)->has_ceiling())
			return true;
	}

	for (const auto& check_entity : get_entities_at(x, y, z))
	{
		if (check_entity == fit)
		{
			continue;
		}

		if (check_entity->is_blocking())
		{
			return true;
		}
	}

	return false;
}

bool World::is_opaque(const Coord& x, const Coord& y, const Coord& z, const Entity* fit)
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

	auto checking_entities = get_entities_at(x, y, z);

	for (const auto& check_entity : checking_entities)
	{
		if (check_entity == fit)
			continue;
		if (!check_entity->is_transparent())
			return true;
	}

	return false;
}

void World::step(TimeUnit time_delta)
{
	std::vector<size_t> remove_entities;

	for (size_t i = 0; i < m_entities.size(); i++)
	{
		auto& entity = m_entities[i];

		if (entity->should_remove())
		{
			remove_entities.push_back(i);
		}
		else
		{
			entity->step(time_delta);
		}
	}

	for (const auto& remove_idx : remove_entities)
	{
		if (remove_idx > m_entities.size() - 1)
			continue;

		remove_entity(m_entities[remove_idx].get());
		m_entities.erase(m_entities.begin() + remove_idx);
	}

	m_current_time.add(time_delta);
}