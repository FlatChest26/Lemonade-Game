#pragma once

#ifndef LEMONADE_GAME_SRC_AI_BRAIN_H
#define LEMONADE_GAME_SRC_AI_BRAIN_H

#include "time_units.h"

#include "ai_behavior.h"
#include "ai_state.h"

#include "weapon_info.h"

#include <vector>

class Entity;

class AIManager;

class AIBrain
{
	friend class AIState;
protected:
	AIManager* m_manager{ nullptr };
	Entity* m_entity{ nullptr };

	std::vector<std::unique_ptr<AIState>> m_state_stack;

	TimeUnit m_current_step;
	TimeUnit m_think_speed{ TimeUnit::from_ticks(1.0) };

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

	void on_damage_taken(const Damage& damage);
	void on_attacked(Entity* attacker);

public:
	bool push_state(AIState* state);
	void pop_state();
	bool reset_state(AIState* state = nullptr);
	
	AIState* current_state() const;

public:
	void step(const TimeUnit& time_delta);

protected:
	void think(const TimeUnit& time_delta);

	template<typename T>
	bool has_state_type() const;
};

#endif // !LEMONADE_GAME_SRC_AI_BRAIN_H
