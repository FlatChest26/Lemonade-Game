#include "ai_handler.h"
#include "direction_action.h"
#include "game_states.h"
#include "world_state.h"

#include "snowy_macros.h"

#include <random>
#include "snowy_rng.h"

void AIBrain::step(TimeUnit time_delta)
{
	auto entity = get_entity();

	if (!entity)
	{
		queue_remove();
		return;
	}

	if (auto goal = current_behavior())
	{
		if (goal->perform(this, time_delta))
		{
			m_behavior_stack.pop_back();
		}
	}
	else
	{
		add_behavior(std::make_unique<Behavior_Wander>(
			2, Position(-1, -1, -1), TimeUnit::from_seconds(0.5), TimeUnit::from_seconds(4.0)
		));
	}
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

AIManager::AIManager(GameState* game_state, std::vector<std::unique_ptr<AIBrain>> brains) :
	m_game_state(game_state)
{
	for (auto& brain : brains)
	{
		if (brain)
		{
			m_brains.push_back(std::move(brain));
		}
	}

	m_npc_generator = std::make_unique<NPCGenerator>(this);
}

GameState* AIManager::get_game_state() const
{
	return m_game_state;
}

WorldGameState* AIManager::get_world_state() const
{
	if (m_game_state && m_game_state->is_world_game_state())
	{
		return m_game_state->as_world_game_state();
	}

	return nullptr;
}

World* AIManager::get_world() const
{
	if (auto world_state = get_world_state())
	{
		return world_state->get_world();
	}

	return nullptr;
}

std::vector<AIBrain*> AIManager::get_brains()
{
	std::vector<AIBrain*> brains;

	for (auto& brain : m_brains)
		brains.push_back(brain.get());

	return brains;
}

AIBrain* AIManager::add_brain_to_entity(Entity* entity)
{
	if (!entity)
		return nullptr;

	if (get_entity_brain(entity)) // Already has a brain
		return nullptr;

	auto brain = std::make_unique<AIBrain>(this, entity);
	m_brains.push_back(std::move(brain));

	return get_entity_brain(entity);
}

AIBrain* AIManager::get_entity_brain(Entity* entity)
{
	for (auto& brain : m_brains)
	{
		if (brain->get_entity() == entity)
			return brain.get();
	}

	return nullptr;
}

void AIManager::step(TimeUnit time_delta)
{
	std::vector<size_t> remove_brains;
	
	for (size_t i = 0; i < m_brains.size(); i++)
	{
		auto& brain = m_brains[i];

		if (brain->should_remove())
		{
			remove_brains.push_back(i);
		}
		else
		{
			brain->step(time_delta);
		}
	}

	for (const auto& remove_idx : remove_brains)
	{
		if (remove_idx > m_brains.size() - 1)
			continue;
		m_brains.erase(m_brains.begin() + remove_idx);
	}

	if (m_time_until_next_NPC > TimeUnit::none())
	{
		m_time_until_next_NPC.subtract(time_delta);
	}
	else
	{
		if (!m_npc_generator)
		{
			CERR("No NPC Generator on AI Manager!");
			return;
		}
		
		auto world = get_world();
			
		if (!world)
		{
			CERR("World is null!");
			return;
		}

		std::uniform_real_distribution<float> time_dis(15, 30);
		m_time_until_next_NPC = TimeUnit::from_seconds(time_dis(snowy::rng::generator));

		std::vector<double> spawn_group_weights = { 4.0, 5.0, 1.0, 3.0 };

		std::discrete_distribution<int> npc_count_dis(spawn_group_weights.begin(), spawn_group_weights.end());
		auto spawn_count = npc_count_dis(snowy::rng::generator) + 1;

		for (;spawn_count > 0; spawn_count--)
		{
			Coord x = (spawn_count % m_sidewalk_width);
			Coord y = (spawn_count / m_sidewalk_width);

			auto npc = world->add_entity_at(std::unique_ptr<Entity>(m_npc_generator->generate_npc()), m_sidewalk_start + Position(x, y, 0));

			if (npc)
			{
				auto npc_brain = add_brain_to_entity(npc);
				npc_brain->add_behavior(std::move(std::make_unique<Behavior_MoveTo>(m_sidewalk_end + Position(x, 0, 0))));
				npc->change_gait_type("STROLL");
			}
		}
	}
}