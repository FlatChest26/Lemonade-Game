#include "ai_brain.h"

#include "game_states.h"
#include "world_state.h"
#include "misc_actions.h"

#include "snowy_macros.h"

#include "entity.h"

#include <random>
#include "snowy_rng.h"

AIBrain::~AIBrain()
{
}

void AIBrain::step(TimeUnit time_delta)
{
	auto entity = get_entity();

	if (!entity || entity->should_remove())
	{
		return;
	}

	if (auto behavior = current_behavior())
	{
		if (behavior->perform(this, time_delta))
		{
			m_behavior_stack.pop_back();
		}
	}
	else
	{
		//add_behavior(std::make_unique<Behavior_Wander>(2, Position(-1, -1, -1), TimeUnit::from_seconds(0.5), TimeUnit::from_seconds(4.0)));

		double required_distance_squared = 2.0f * 2.0f;
		double dist_1 = (m_manager->get_sidewalk_end() - entity->get_movable()->pos()).length_squared();
		double dist_2 = ((m_manager->get_sidewalk_end() + Position(1, 0, 0)) - entity->get_movable()->pos()).length_squared();

		if (dist_1 < required_distance_squared && dist_2 < required_distance_squared)
		{
			entity->queue_remove();
		}
	}
}

void AIBrain::set_entity(Entity* entity)
{
	m_entity = entity;
}

Entity* AIBrain::get_entity()
{
	return m_entity;
}

void AIBrain::add_behavior(std::unique_ptr<Behavior> behavior)
{
	if (!behavior)
	{
		return;
	}

	m_behavior_stack.push_back(std::move(behavior));
}

Behavior* AIBrain::current_behavior()
{
	if (m_behavior_stack.empty())
	{
		return nullptr;
	}

	return m_behavior_stack.back().get();
}

void AIBrain::cancel_behavior()
{
	m_behavior_stack.pop_back();
}