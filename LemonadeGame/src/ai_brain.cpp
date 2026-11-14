#include "ai_brain.h"

#include "game_states.h"
#include "world_state.h"
#include "interactions.h"

#include "snowy_macros.h"

#include "entity.h"

#include <random>
#include "snowy_rng.h"

AIBrain::AIBrain(const AIBrain& other)
{
	m_manager = other.m_manager;
	m_entity = other.m_entity;
}

AIBrain::AIBrain(AIBrain&& other)
{
	m_manager = other.m_manager;
	m_entity = other.m_entity;
	m_state_stack = std::move(other.m_state_stack);
}

void AIBrain::push_state(AIState* state)
{
	std::unique_ptr<AIState> new_state(state);

	new_state->set_brain(this);
	new_state->init();

	if (!new_state->check())
		return;

	new_state->reinit();

	m_state_stack.push_back(std::move(new_state));
}

void AIBrain::pop_state()
{
	if (m_state_stack.empty())
		return;

	current_state()->remove();
	m_state_stack.pop_back();

	if (current_state())
	{
		current_state()->reinit();
	}
}

void AIBrain::reset_state(AIState* state)
{
	while (!m_state_stack.empty())
	{
		pop_state();
	}

	if (state)
	{
		push_state(state);
	}
}

AIState* AIBrain::current_state() const
{
	if (m_state_stack.empty())
		return nullptr;
	return m_state_stack.back().get();
}

void AIBrain::step(const TimeUnit& time_delta)
{
	auto entity = get_entity();

	if (!entity || entity->ai_brain() != this)
	{
		CERR("entity's brain is not this brain!");
		return;
	}

	if (!entity || entity->should_remove())
	{
		return;
	}

	if (entity->has_living() && entity->living()->is_dead())
	{
		return;
	}

	if (auto state = current_state())
	{
		auto result = state->step(time_delta);

		switch (result.type)
		{
		case StateResultType::POP:
			pop_state();
			break;
		case StateResultType::PUSH:
			push_state(result.next);
			break;
		case StateResultType::RESET:
			reset_state(result.next);
			break;
		}
	}
}

