#include "ai_brain.h"

#include "game_states.h"
#include "world_state.h"
#include "interactions.h"

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

	if (entity->get_brain() != this)
	{
		CERR("entity's brain is not this brain!");
		return;
	}

	if (!entity || entity->should_remove())
	{
		return;
	}

	if (entity->has_living() && entity->get_living()->is_dead())
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
		auto entity_pos = entity->get_movable()->pos();
		auto sidewalk_1 = m_manager->get_sidewalk_end();
		auto sidewalk_2 = m_manager->get_sidewalk_end() + Position(1, 0, 0);

		if (transforms::is_adjacent(entity_pos, sidewalk_1) || transforms::is_adjacent(entity_pos, sidewalk_2))
		{
			entity->queue_remove();
		}
		else
		{
			add_behavior(new Behavior_Wander(2, Position(-1, -1, -1), TimeUnit::from_seconds(0.5), TimeUnit::from_seconds(4.0)));
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

void AIBrain::add_behavior(Behavior* behavior)
{
	if (!behavior)
	{
		return;
	}

	m_behavior_stack.push_back(behavior);
}

Behavior* AIBrain::current_behavior()
{
	if (m_behavior_stack.empty())
	{
		return nullptr;
	}

	return m_behavior_stack.back();
}

const Behavior* AIBrain::current_behavior() const
{
	if (m_behavior_stack.empty())
	{
		return nullptr;
	}

	return m_behavior_stack.back();
}

void AIBrain::cancel_behavior()
{
	m_behavior_stack.pop_back();
}