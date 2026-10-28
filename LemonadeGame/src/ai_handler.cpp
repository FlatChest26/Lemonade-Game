#include "ai_handler.h"
#include "game_states.h"
#include "world_state.h"
#include "misc_actions.h"

#include "snowy_macros.h"

#include "entity.h"

#include <random>
#include "snowy_rng.h"

AIManager::AIManager(GameState* game_state) :
	m_game_state(game_state)
{
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

AIBrain* AIManager::create_brain()
{
	auto brain = new AIBrain(this);
	return brain;
}

void AIManager::step(TimeUnit time_delta)
{
	if (m_time_until_next_NPC > TimeUnit::none())
	{
		m_time_until_next_NPC -= time_delta;
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
			Coord x = ((spawn_count - 1) % m_sidewalk_width);
			Coord y = ((spawn_count - 1) / m_sidewalk_width);

			auto npc = world->add_entity_at(std::unique_ptr<Entity>(m_npc_generator->generate_npc()), m_sidewalk_start + Position(x, y, 0));

			if (npc)
			{
				auto brain = npc->add_brain(create_brain());
				npc->change_gait_type("STROLL");
				brain->add_behavior(std::make_unique<Behavior_MoveTo>(m_sidewalk_end + Position(x, 0, 0)));
			}
		}
	}
}