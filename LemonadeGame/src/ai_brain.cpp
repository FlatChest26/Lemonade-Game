#include "ai_brain.h"

#include "game_states.h"
#include "world_state.h"
#include "interactions.h"

#include "snowy_macros.h"

#include "entity.h"
#include "need_states.h"

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

bool AIBrain::push_state(AIState* state)
{
	std::unique_ptr<AIState> new_state(state);

	new_state->set_brain(this);
	new_state->init();

	if (!new_state->check())
		return false;

	new_state->reinit();

	m_state_stack.push_back(std::move(new_state));

	return true;
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



bool AIBrain::reset_state(AIState* state)
{
	while (!m_state_stack.empty())
	{
		pop_state();
	}

	if (state)
	{
		return push_state(state);
	}

	return true;
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

	if (!entity || entity->should_remove() || (entity->has_living() && entity->living()->is_dead()))
	{
		return;
	}

	m_current_step += time_delta;
	while (m_current_step >= m_think_speed)
	{
		think(time_delta);
		m_current_step -= std::min(m_think_speed, m_current_step);
	}

	if (auto state = current_state())
	{
		StateResult result = StateResult(StateResultType::POP);

		if (state->check())
		{
			result = state->step(time_delta);
		}

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

template<typename T>
bool AIBrain::has_state_type() const
{
	for (const auto& state : m_state_stack)
	{
		if (typeid(*state) == typeid(T))
			return true;
	}

	return false;
}

void AIBrain::on_damage_taken(const Damage& damage)
{
	
}

void AIBrain::on_attacked(Entity* attacker)
{
	auto cur = current_state();

	if (cur && typeid(*current_state()) == typeid(AIState_Kill))
	{
		return;
	}

	push_state(new AIState_Kill({ .kill_target = attacker }));
}

void AIBrain::think(const TimeUnit& time_delta)
{
	auto entity = get_entity();

	if (!entity->has_living())
	{
		return;
	}

	snowy::rng::WeightedVector<std::function<AIState*()>> potential_states;

	auto has_type_id = [this](auto type_id)
		{
			for (const auto& state : m_state_stack)
			{
				if (typeid(*state) == type_id)
					return true;
			}

			return false;
		};

	for (auto& need : entity->living()->get_needs())
	{
		if (need == needs::AROUSAL)
		{
			double priority = entity->living()->get_stat(needs::AROUSAL).get_positive_percentage() * 100;
			
			if (priority > 50.0 && !has_state_type<AIState_Horny>())
			{
				potential_states.push_back(
					std::tuple(
						priority * 0.6, [=]()
						{
							return new AIState_Horny({
								.still_valid_callback = [entity]() { return entity->living()->get_stat(needs::AROUSAL).get_positive_percentage() * 100 >= 50.0; }
							});
						}
					)
				);
			}
		}

		if (need == needs::THIRST)
		{
			double priority = entity->living()->get_stat(needs::THIRST).get_positive_percentage() * 100;

			if (priority > 29.0 && !has_state_type<AIState_FindConsumableEffect>())
			{
				potential_states.push_back(
					std::tuple(
						priority * 0.8, [=]()
						{
							return new AIState_FindConsumableEffect({
									.stat_id = needs::THIRST,
									.still_valid_callback = [entity]() { return entity->living()->get_stat(needs::THIRST).get_positive_percentage() * 100 >= 30.0; }
								});
						}
					)
				);
			}

			
		}

		if (need == needs::HUNGER)
		{
			double priority = entity->living()->get_stat(needs::HUNGER).get_positive_percentage() * 100;

			if (priority > 29.0 && !has_state_type<AIState_FindConsumableEffect>())
			{
				potential_states.push_back(
					std::tuple(
						priority * 0.9, [=]()
						{
							return new AIState_FindConsumableEffect({
								.stat_id = needs::HUNGER,
								.still_valid_callback = [entity]() { return entity->living()->get_stat(needs::HUNGER).get_positive_percentage() * 100 >= 30.0; }
								});
						}
					)
				);
			}
		}

		if (need == needs::HYGIENE)
		{
			double priority = entity->living()->get_stat(needs::HYGIENE).get_positive_percentage() * 100;

			if (priority < 50.0)
				continue;

			potential_states.push_back(
				std::tuple(
					priority * 0.9, [=]()
					{
						return new AIState_FindConsumableEffect({
							.stat_id = needs::HYGIENE,
							.still_valid_callback = [entity]() { return entity->living()->get_stat(needs::HYGIENE).get_positive_percentage() * 100 >= 30.0; }
						});
					}
				)
			);
		}
	}

	if (!potential_states.empty())
	{
		auto new_state = snowy::rng::get_weighted_random(potential_states, snowy::rng::generator)();
		auto state = current_state();

		if ((state && typeid(*state) == typeid(*new_state)) || !push_state(new_state))
		{
			delete new_state;
		}
	}
}


