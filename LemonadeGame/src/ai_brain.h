#pragma once

#ifndef LEMONADE_GAME_SRC_AI_BRAIN_H
#define LEMONADE_GAME_SRC_AI_BRAIN_H

#include "time_units.h"

#include "ai_behavior.h"
#include "ai_state.h"

#include <vector>

class Entity;

class AIManager;

class AIBrain
{
protected:
	AIManager* m_manager{ nullptr };
	Entity* m_entity{ nullptr };

	std::vector<std::unique_ptr<AIState>> m_state_stack;

public:
	AIBrain(AIManager* manager = nullptr, Entity* entity = nullptr) :
		m_manager(manager), m_entity(entity), m_state_stack()
	{
	}

	AIBrain(const AIBrain& other);
	AIBrain(AIBrain&& other);

public:
	AIManager* get_manager() const { return m_manager; }
	void set_manager(AIManager* manager) { m_manager = manager; }

	void set_entity(Entity* entity) { m_entity = entity; }
	Entity* get_entity() const { return m_entity; }

public:
	void push_state(AIState* state);
	void pop_state();
	void reset_state(AIState* state = nullptr);
	
	AIState* current_state() const
	{
		if (m_state_stack.empty())
			return nullptr;
		return m_state_stack.front().get();
	}

public:
	void step(const TimeUnit& time_delta);
};

#endif // !LEMONADE_GAME_SRC_AI_BRAIN_H
