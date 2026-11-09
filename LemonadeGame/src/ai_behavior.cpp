#include "ai_behavior.h"
#include "ai_handler.h"
#include "entity.h"
#include "tile_map.h"
#include "direction_action.h"
#include "pathfinding.h"
#include "snowy_rng.h"
#include "world.h"
#include "ai_state.h"

bool Behavior_MoveTo::perform(AIState* state, const TimeUnit& delta)
{
	auto brain = state->get_brain();
	auto entity = brain->get_entity();

	if (!entity)
	{
		CERR("entity is null!");
		return true;
	}

	if (!entity->has_movable())
	{
		CERR("entity has no movable! " << entity->ID());
		return true;
	}

	if (entity->get_movable()->pos() == m_move_to_pos)
	{
		//CERR("entity is already at m_move_to!");
		return true;
	}

	if (!entity->can_move_on_own())
	{
		return true;
	}

	auto tile_map = entity->get_tile_map();

	if (!tile_map)
	{
		CERR("tile_map is null!");
		return true;
	}

	if (!tile_map->in_bounds(m_move_to_pos))
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

	if (entity->has_current_action())
	{
		return false;
	}

	if (m_path.empty() || tile_map->should_update_path() || entity->get_movable()->pos() != m_current_pos || !m_callback(this))
	{
		m_path.clear();
		m_current_pos = entity->get_movable()->pos();

		m_path = Pathfinding::compute_path(entity->get_tile_map(), m_current_pos, m_move_to_pos, entity);
	}

	if (!m_path.empty())
	{
		m_current_pos = m_path.back();
		m_path.pop_back();

		Position delta = m_current_pos - entity->get_movable()->pos();

		entity->start_action(new BumpAction(entity, delta.unit()));
	}

	return m_path.empty();
}

bool Behavior_Wander::perform(AIState* state, const TimeUnit& delta)
{
	auto brain = state->get_brain();
	auto entity = brain->get_entity();

	if (!entity)
	{
		CERR("entity is null!");
		return true;
	}
	
	if (!entity->can_move_on_own())
	{
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

		state->add_behavior(new Behavior_MoveTo({ .move_to_pos = wander_location }));

		if ((m_min_wait > TimeUnit::none()) && (m_max_wait > m_min_wait))
		{
			std::uniform_int_distribution<TimeUnit_t> wait_distribution(m_min_wait, m_max_wait);
			m_wait_time = wait_distribution(snowy::rng::generator);
		}
	}

	return false;
}

bool Behavior_GetAdjacentToEntity::perform(AIState* state, const TimeUnit& delta)
{
	auto entity = state->get_entity();

	if (!entity || !entity->has_movable())
	{
		return true;
	}

	if (!m_target || !m_target->has_movable())
	{
		return true;
	}

	auto entity_pos = entity->get_movable()->pos();
	auto target_pos = m_target->get_movable()->pos();

	if (transforms::is_adjacent(entity_pos, target_pos))
	{
		return true;
	}

	auto move_to = transforms::get_closest_adjacent(entity_pos, target_pos);
	auto world = state->get_world();

	if (world->is_blocked(move_to, entity))
	{
		std::list<Position> valid_positions;

		for (const auto& p : transforms::get_surrounding_locations(target_pos))
		{
			if (!world->is_blocked(p, entity))
			{
				valid_positions.push_back(p);
			}
		}

		if (!valid_positions.empty())
		{
			valid_positions.sort(
				[&](const Position& p1, const Position& p2)
				{
					return (p1 - entity_pos).length_squared() > (p2 - entity_pos).length_squared();
				}
			);

			move_to = valid_positions.front();
		}
	}

	state->add_behavior(new Behavior_MoveTo({
		.move_to_pos = move_to,
		.callback = [=](Behavior_MoveTo* behavior)
			{
				if (m_target->get_movable()->pos() != target_pos)
				{
					behavior->set_move_to(m_target->get_movable()->pos());
					return true;
				}

				return false;
			}
		})
	);

	return false;
}