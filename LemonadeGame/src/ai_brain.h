#pragma once

#ifndef LEMONADE_GAME_SRC_AI_BRAIN_H
#define LEMONADE_GAME_SRC_AI_BRAIN_H

#include "time_units.h"

#include "ai_behavior.h"

#include <vector>

class Entity;

class AIManager;

class AIBrain
{
protected:
	AIManager* m_manager{ nullptr };
	Entity* m_entity{ nullptr };

	std::vector<std::unique_ptr<Behavior>> m_behavior_stack;

public:
	AIBrain(AIManager* manager = nullptr, Entity* entity = nullptr) :
		m_manager(manager), m_entity(entity)
	{}

	~AIBrain();

	void step(TimeUnit time_delta);

	void set_entity(Entity* entity);
	Entity* get_entity();

	void add_behavior(std::unique_ptr<Behavior> behavior);
	Behavior* current_behavior();
	void cancel_behavior();

};

#endif // !LEMONADE_GAME_SRC_AI_BRAIN_H
