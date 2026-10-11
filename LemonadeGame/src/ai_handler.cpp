#include "ai_handler.h"
#include "direction_action.h"
#include "game_states.h"

#include "snowy_macros.h"

#include <random>
#include "snowy_rng.h"

void AIBrain::step(TimeUnit time_delta)
{
	if (!m_entity->has_any_action() && !m_entity->has_current_action())
	{
	}
}

AIManager::AIManager(GameState* game_state, std::vector<std::unique_ptr<AIBrain>> brains) :
	m_game_state(game_state)
{
	for (auto& brain : brains)
	{
		if (brain) m_brains.push_back(std::move(brain));
	}
}

bool AIManager::add_brain_to_entity(Entity* entity)
{
	if (!entity)
		return false;

	if (get_entity_brain(entity)) // Already has a brain
		return false;

	auto brain = std::make_unique<AIBrain>(this, entity);
	m_brains.push_back(std::move(brain));

	return true;
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
	for (auto& brain : m_brains)
	{
		brain->step(time_delta);
	}
}