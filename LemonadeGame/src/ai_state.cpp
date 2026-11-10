#include "ai_state.h"
#include "ai_brain.h"
#include "entity.h"
#include "world.h"
#include "ai_handler.h"
#include "interactions.h"

AIState::AIState(const AIState& other)
{
	m_brain = other.m_brain;

}

AIState::AIState(AIState&& other)
{
	m_brain = other.m_brain;
	m_behavior_stack = std::move(other.m_behavior_stack);
}

AIManager* AIState::get_ai_manager() const
{
	return get_brain()->get_manager();
}

Entity* AIState::get_entity() const
{
	return get_brain()->get_entity();
}

World* AIState::get_world() const
{
	if (auto entity = get_entity())
	{
		return entity->get_world();
	}

	return nullptr;
}

TileMap* AIState::get_tile_map() const
{
	if (auto world = get_world())
	{
		return get_world()->get_tile_map();
	}

	return nullptr;
}


void AIState::add_behavior(Behavior* behavior)
{
	if (!behavior)
	{
		return;
	}

	m_behavior_stack.push_back(std::unique_ptr<Behavior>(behavior));
}

Behavior* AIState::current_behavior() const
{
	if (m_behavior_stack.empty())
	{
		return nullptr;
	}

	return m_behavior_stack.back().get();
}

void AIState::cancel_behavior()
{
	m_behavior_stack.pop_back();
}

void AIState::run_behaviors(const TimeUnit& time_delta)
{
	if (auto behavior = current_behavior())
	{
		if (behavior->perform(this, time_delta))
		{
			m_behavior_stack.pop_back();
		}
	}
}
StateResult AIState::step(const TimeUnit& time_delta)
{
	run_behaviors(time_delta);
	return StateResult(StateResultType::NONE);
}

StateResult AIState_Wander::step(const TimeUnit& time_delta)
{
	run_behaviors(time_delta);

	if (!has_behavior())
	{
		add_behavior(new Behavior_Wander(
			{
				.wander_radius = 5,
				.center = get_entity()->get_movable()->pos(),
				.min_wait = TimeUnit::from_seconds(30.0),
				.max_wait = TimeUnit::from_seconds(45.0)
			}
		));
	}

	return StateResult(StateResultType::NONE);
}


void AIState_WalkAlongSidewalk::reinit()
{
	if (auto entity = get_entity())
	{
		entity->get_gait()->set_current_gait("stroll");
	}
}

StateResult AIState_WalkAlongSidewalk::step(const TimeUnit& time_delta)
{
	run_behaviors(time_delta);

	if (!has_behavior())
	{
		auto entity = get_entity();
		auto& entity_pos = entity->get_movable()->pos();

		auto sidewalk_1 = get_ai_manager()->get_sidewalk_end();
		auto sidewalk_2 = get_ai_manager()->get_sidewalk_end() + Position(1, 0, 0);

		if (transforms::is_adjacent(entity_pos, sidewalk_1) || transforms::is_adjacent(entity_pos, sidewalk_2))
		{
			entity->queue_remove();
		}
		else
		{
			add_behavior(new Behavior_MoveTo({ .move_to_pos = m_preferred_sidewalk_end } ));
			entity->get_gait()->set_current_gait("stroll");
		}
	}

	return StateResult(StateResultType::NONE);
}

void AIState_Horny::reinit()
{
	if (auto entity = get_entity())
	{
		entity->get_gait()->set_current_gait("walk");
	}
}

StateResult AIState_Horny::step(const TimeUnit& time_delta)
{
	auto entity = get_entity();

	auto& entity_pos = entity->get_movable()->pos();
	auto& target_pos = m_horny_target->get_movable()->pos();

	run_behaviors(time_delta);

	if (!has_behavior())
	{
		if (entity->start_action(new KissAction(entity, m_horny_target)))
		{
			return StateResult(StateResultType::POP);
		}
		else
		{
			add_behavior(new Behavior_GetAdjacentToEntity({ .target = m_horny_target }));
			entity->get_gait()->set_current_gait("walk");
		}
	}

	return StateResult(StateResultType::NONE);
}