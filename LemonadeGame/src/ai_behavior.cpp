#include "ai_behavior.h"
#include "ai_handler.h"
#include "entity.h"
#include "tile_map.h"
#include "direction_action.h"
#include "location_actions.h"
#include "pathfinding.h"
#include "snowy_rng.h"
#include "world.h"
#include "ai_state.h"
#include "entity_filter.h"

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

	auto entity_pos = entity->movable()->pos();

	if (entity_pos == m_move_to_pos)
	{
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

	bool has_dirty_path = false;

	if (tile_map->has_dirty_tiles())
	{
		for (const auto& pos : m_path)
		{
			if (tile_map->is_dirty(pos))
			{
				has_dirty_path = true;
				break;
			}
		}
	}

	if (
		m_path.empty() || 
		has_dirty_path || 
		entity_pos != m_current_pos ||
		m_callback(this)
		)
	{
		m_path.clear();
		m_current_pos = entity->movable()->pos();

		m_path = Pathfinding::compute_path(tile_map, m_current_pos, m_move_to_pos, entity);
	}

	bool close_behind = false;

	if (tile_map->in_bounds(m_previous_pos) && m_current_pos != m_previous_pos)
	{
		if (tile_map->get_tile(m_previous_pos)->can_close())
		{
			close_behind = entity->start_action(new CloseAction(entity, m_previous_pos));
		}
	}
	
	if (!m_path.empty() && !close_behind)
	{
		m_current_pos = m_path.back();
		m_path.pop_back();

		Position delta = (m_current_pos - entity_pos).unit();
		entity->start_action(new BumpAction(entity, delta));
		m_previous_pos = entity_pos;
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
			auto center = entity->movable()->pos();

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

bool Behavior_GetToEntity::perform(AIState* state, const TimeUnit& delta)
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

	auto entity_pos = entity->movable()->pos();
	auto target_pos = m_target->movable()->pos();

	if (entity_pos == target_pos)
	{
		return true;
	}

	auto get_move_to = [=]()
		{
			auto move_to = target_pos;
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

			return move_to;
		};

	state->add_behavior(new Behavior_MoveTo({
		.move_to_pos = get_move_to(),
		.callback = [=](Behavior_MoveTo* behavior)
			{
				if (m_target->movable()->pos() != target_pos)
				{
					behavior->set_move_to(get_move_to());
					return true;
				}

				return false;
			}
		})
	);

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

	auto entity_pos = entity->movable()->pos();
	auto target_pos = m_target->movable()->pos();

	if (transforms::is_adjacent(entity_pos, target_pos))
	{
		return true;
	}


	auto get_move_to = [=]()
		{
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

			return move_to;
		};
	
	state->add_behavior(new Behavior_MoveTo({
		.move_to_pos = get_move_to(),
		.callback = [=](Behavior_MoveTo* behavior)
			{
				if (m_target->movable()->pos() != target_pos)
				{
					behavior->set_move_to(get_move_to());
					return true;
				}

				return false;
			}
		})
	);

	return false;
}

Behavior_SearchForEntity::~Behavior_SearchForEntity()
{
	delete m_filter;
}

bool Behavior_SearchForEntity::perform(AIState* state, const TimeUnit& delta)
{
	if (m_search_attempts < 1)
		return true;

	snowy::rng::WeightedVector<Entity*> potential_targets;

	for (auto& other_entity : state->get_world()->get_entities(*m_filter))
	{
		if (m_is_valid_callback && !m_is_valid_callback(other_entity))
			continue;

		double weight = 1.0;

		if (m_get_weight_callback)
		{
			weight = m_get_weight_callback(other_entity);
		}


		if (weight < DBL_EPSILON)
			continue;

		potential_targets.push_back(std::tuple(weight, other_entity));
	}

	if (!potential_targets.empty())
	{
		Entity* selected_entity = snowy::rng::get_weighted_random(potential_targets, snowy::rng::generator);

		m_on_found_callback(selected_entity);
		return true;
	}
	else // Wander somewhere else to look
	{
		auto tile_map = state->get_tile_map();
		auto entity = state->get_entity();

		std::uniform_int_distribution<Coord> search_distribution(-m_search_radius, m_search_radius);
		Position search_location{ -1, -1, -1 };

		int checking_attempts = 10;

		while (!tile_map->in_bounds(search_location))
		{
			auto center = entity->movable()->pos();

			search_location = center + Position{
				search_distribution(snowy::rng::generator),
				search_distribution(snowy::rng::generator),
				0
			};

			checking_attempts--;

			if (checking_attempts < 1)
			{
				break;
			}
		}

		state->add_behavior(new Behavior_MoveTo({ .move_to_pos = search_location }));
		m_search_attempts--;
	}


	return false;
}