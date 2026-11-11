#pragma once

#ifndef LEMONADE_GAME_SRC_AI_STATE_H
#define LEMONADE_GAME_SRC_AI_STATE_H

#include "ai_behavior.h"

class AIBrain;
class Entity;
class World;
class TileMap;
class AIManager;

enum class StateResultType : uint8_t
{
	NONE,
	POP,
	PUSH,
	RESET,
};

struct StateResult
{
	StateResultType type;
	AIState* next;
};

class AIState
{
protected:
	AIBrain* m_brain{ nullptr };
	std::vector<std::unique_ptr<Behavior>> m_behavior_stack{};

public:
	AIState(): 
		m_brain(nullptr), m_behavior_stack()
	{}

	AIState(const AIState& other);
	AIState(AIState&& other);

	void set_brain(AIBrain* brain) { m_brain = brain; }
	AIBrain* get_brain() const { return m_brain; }

	AIManager* get_ai_manager() const;
	Entity* get_entity() const;
	World* get_world() const;
	TileMap* get_tile_map() const;

public:

	void add_behavior(Behavior* behavior);
	Behavior* current_behavior() const;
	void cancel_behavior();
	bool has_behavior() const { return !m_behavior_stack.empty(); }
	

protected:
	void run_behaviors(const TimeUnit& time_delta);

public:
	virtual void init() {}
	virtual void reinit() {}
	virtual void remove() {}
	virtual bool check() const { return true; }

	virtual StateResult step(const TimeUnit& time_delta);
};


class AIState_Wander : public AIState
{
public:
	virtual StateResult step(const TimeUnit& time_delta) override;
};

class AIState_WalkAlongSidewalk : public AIState
{
protected:
	Position m_preferred_sidewalk_end;

public:
	struct initializer
	{
		Position preferred_sidewalk_end;
	};

	AIState_WalkAlongSidewalk(initializer init):
		m_preferred_sidewalk_end(init.preferred_sidewalk_end)
	{}

public:
	virtual void reinit() override;
	virtual StateResult step(const TimeUnit& time_delta) override;
};

class AIState_Horny : public AIState
{
protected:
	Entity* m_horny_target;

public:
	struct initializer
	{
		Entity* horny_target;
	};

	AIState_Horny(initializer init) :
		m_horny_target(init.horny_target)
	{
	}

public:
	virtual void reinit() override;
	virtual StateResult step(const TimeUnit& time_delta) override;
};


class AIState_Kill : public AIState
{
protected:
	Entity* m_kill_target;
	Entity* m_weapon_target{ nullptr };

public:
	struct initializer
	{
		Entity* kill_target;
	};

	AIState_Kill(initializer init) :
		m_kill_target(init.kill_target)
	{
	}

public:
	virtual void reinit() override;
	virtual StateResult step(const TimeUnit& time_delta) override;
};


#endif // !LEMONADE_GAME_SRC_AI_STATE_H