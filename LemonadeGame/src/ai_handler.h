#pragma once

#ifndef LEMONADE_GAME_SRC_AI_HANDLER_H
#define LEMONADE_GAME_SRC_AI_HANDLER_H

#include "entity.h"
#include "time_units.h"
#include "npc_generator.h"

#include "ai_behavior.h"

#include <memory>

class AIManager;

class AIBrain
{
protected:
	AIManager* m_manager{ nullptr };
	Entity* m_entity{ nullptr };
	bool m_should_remove{ false };

	std::vector<std::unique_ptr<Behavior>> m_behavior_stack;

public:
	AIBrain(AIManager* manager = nullptr, Entity* entity = nullptr) :
		m_manager(manager), m_entity(entity)
	{
	}

	void step(TimeUnit time_delta);

	Entity* get_entity() { return m_entity; }

	void add_behavior(std::unique_ptr<Behavior> behavior);
	Behavior* current_behavior();
	void cancel_behavior();

	void queue_remove() { m_should_remove = true; }
	bool should_remove() const { return m_should_remove; }

};

class World;
class GameState;

class AIManager
{
protected:
	GameState* m_game_state{ nullptr };
	std::vector<std::unique_ptr<AIBrain>> m_brains{};
	std::unique_ptr<NPCGenerator> m_npc_generator{ nullptr };

	TimeUnit m_time_until_next_NPC{};

	Position m_sidewalk_start{ 0, 0, 0 };
	Position m_sidewalk_end{ 0, 0, 0 };
	Length m_sidewalk_width{ 0 };

public:
	AIManager(GameState* game_state = {}, std::vector<std::unique_ptr<AIBrain>> brains = {});


public:

	GameState* get_game_state() const;
	WorldGameState* get_world_state() const;
	World* get_world() const;
	std::vector<AIBrain*> get_brains();

	void set_sidewalk_start(Position pos) { m_sidewalk_start = pos; }
	void set_sidewalk_end(Position pos) { m_sidewalk_end = pos; }
	void set_sidewalk_width(Length size) { m_sidewalk_width = size; }

	Position get_sidewalk_start() const { return m_sidewalk_start; }
	Position get_sidewalk_end() const { return m_sidewalk_end; }
	Length get_sidewalk_width() const { return m_sidewalk_width; }

public:
	AIBrain* add_brain_to_entity(Entity* entity);
	AIBrain* get_entity_brain(Entity* entity);

	void step(TimeUnit time_delta);
};

#endif // !LEMONADE_GAME_SRC_AI_HANDLER_H