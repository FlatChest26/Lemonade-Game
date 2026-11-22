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

enum class UrgencyLevel : uint8_t
{
	RELAXED,
	NEUTRAL,
	TENSE,
	LIFE_OR_DEATH
};

class AIState
{
	friend class AIBrain;

protected:
	AIBrain* m_brain{ nullptr };
	std::vector<std::unique_ptr<Behavior>> m_behavior_stack{};
	UrgencyLevel m_urgency;

public:
	AIState(): 
		m_brain(nullptr), m_behavior_stack(), m_urgency(UrgencyLevel::NEUTRAL)
	{}

	AIState(const AIState& other);
	AIState(AIState&& other);

	UrgencyLevel get_urgency() const { return m_urgency; }
	void set_urgency(UrgencyLevel value) { m_urgency = value; }

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
	void set_entity_gait_based_on_urgency();

public:
	virtual void init() {}
	virtual void reinit() { set_entity_gait_based_on_urgency(); }

	virtual void remove() {}
	virtual bool check() const { return true; }

	virtual StateResult step(const TimeUnit& time_delta);

	virtual std::string state_name() const { return "blank state"; }

	bool operator==(const AIState& other) const
	{
		return state_name() == other.state_name();
	}

	bool operator!=(const AIState& other) const
	{
		return state_name() == other.state_name();
	}
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
		UrgencyLevel urgency{ UrgencyLevel::RELAXED };
	};

	AIState_WalkAlongSidewalk(initializer init):
		m_preferred_sidewalk_end(init.preferred_sidewalk_end)
	{
		set_urgency(init.urgency);
	}

public:
	virtual StateResult step(const TimeUnit& time_delta) override;
};

#endif // !LEMONADE_GAME_SRC_AI_STATE_H