#include "ai_behavior.h"
#include "ai_handler.h"
#include "entity.h"
#include "tile_map.h"
#include "direction_action.h"
#include "pathfinding.h"
#include "snowy_rng.h"
#include "world.h"

bool Behavior_MoveTo::perform(AIBrain* brain, TimeUnit delta)
{
	auto entity = brain->get_entity();

	if (!entity)
	{
		CERR("entity is null!");
		return true;
	}

	if (entity->get_movable()->pos() == m_move_to)
	{
		//CERR("entity is already at m_move_to!");
		return true;
	}

	auto tile_map = entity->get_tile_map();

	if (!tile_map)
	{
		CERR("tile_map is null!");
		return true;
	}

	if (!tile_map->in_bounds(m_move_to))
	{
		CERR("Move to location is out of bounds!");
		return true;
	}

	auto world = entity->get_world();

	if (!world)
	{
		CERR("world is null!");
		return true;
	}

	//if (world->is_blocked(m_move_to, entity, m_move_to.z - entity->get_movable()->posz()))
	//{
	//	CERR("Move to location is blocked!");
	//	return true;
	//}

	if (m_path.empty() || entity->get_movable()->pos() != m_current_pos)
	{
		m_path.clear();
		m_current_pos = entity->get_movable()->pos();

		m_path = Pathfinding::compute_path(entity->get_tile_map(), m_current_pos, m_move_to, entity);
	}

	if (!m_path.empty() && !entity->has_current_action())
	{
		m_current_pos = m_path.back();
		m_path.pop_back();

		Position delta = m_current_pos - entity->get_movable()->pos();
		
		//if (entity->can_collide() && world->has_blocking_entities_at(entity->get_movable()->pos(), {.ignore_entities = {entity}}))
		//{
		//	Position escape_wall = entity->escape_wall(entity->get_movable()->pos());
		//	delta = escape_wall - entity->get_movable()->pos();

		//	//std::uniform_int_distribution<int> coin_flip(0, 1);

		//	//bool change_x = std::abs(delta.x) == 0;
		//	//bool change_y = std::abs(delta.y) == 0;

		//	//if (!(change_x || change_y))
		//	//{
		//	//	change_x = coin_flip(snowy::rng::generator) == 0;
		//	//	change_y = !change_x;
		//	//}

		//	//if (change_x)
		//	//{
		//	//	delta.x += coin_flip(snowy::rng::generator) == 0 ? 1 : -1;
		//	//}
		//	//
		//	//if (change_y)
		//	//{
		//	//	delta.y += coin_flip(snowy::rng::generator) == 0 ? 1 : -1;
		//	//}

		//	m_path.clear();
		//}
		
		entity->start_action(std::make_unique<BumpAction>(entity, delta.unit()));
	}

	if (m_path.empty())
	{
		m_attempts--;
	}

	return m_attempts < 1;
}

bool Behavior_Wander::perform(AIBrain* brain, TimeUnit delta)
{
	auto entity = brain->get_entity();

	if (!entity)
	{
		CERR("entity is null!");
		return true;
	}

	auto tile_map = entity->get_tile_map();
	if (!tile_map)
	{
		CERR("tile_map is null!");
		return true;
	}

	if (m_wait_time > TimeUnit::none())
	{
		m_wait_time -= delta;
	}
	else
	{
		std::uniform_int_distribution<Coord> wander_distribution(-m_wander_radius, m_wander_radius);
		Position wander_location{ -1, -1, -1 };

		int checking_attempts = 10;

		while (!tile_map->in_bounds(wander_location))
		{
			auto center = entity->get_movable()->pos();

			if (tile_map->in_bounds(m_center))
			{
				center = m_center;
			}

			wander_location = center + Position{
				wander_distribution(snowy::rng::generator), 
				wander_distribution(snowy::rng::generator), 
				0
			};

			checking_attempts--;

			if (checking_attempts < 1)
			{
				return true;
			}
		}

		brain->add_behavior(std::make_unique<Behavior_MoveTo>(wander_location));

		if ((m_min_wait > TimeUnit::none()) && (m_max_wait > m_min_wait))
		{
			std::uniform_int_distribution<TimeUnit_t> wait_distribution(m_min_wait, m_max_wait);
			m_wait_time = wait_distribution(snowy::rng::generator);
		}
	}

	return false;
}
