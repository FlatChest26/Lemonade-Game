#pragma once

#ifndef LEMONADE_GAME_SRC_AI_BEHAVIOR_H
#define LEMONADE_GAME_SRC_AI_BEHAVIOR_H

#include "transforms.h"
#include "time_units.h"

#include <vector>
#include <functional>

class AIBrain;
class AIState;
class Entity;
class Action;

class Behavior
{
public:
	virtual bool perform(AIState* state, const TimeUnit& delta = TimeUnit::none()) = 0; // returns true when behavior is complete
};

class Behavior_MoveTo : public Behavior
{
protected:
	Position m_move_to_pos{ -1, -1, -1 };
	Position m_current_pos{};
	Position m_previous_pos{ -1, -1, -1 };

	std::vector<Position> m_path{};

	std::function<bool(Behavior_MoveTo*)> m_callback;

public:
	struct initializer
	{
		Position move_to_pos{ -1, -1, -1 };
		std::function<bool(Behavior_MoveTo*)> callback{ [](Behavior_MoveTo* Behavior) { return true; } };
	};

public:
	Behavior_MoveTo(const initializer& init) :
		m_move_to_pos(init.move_to_pos),
		m_callback(init.callback)
	{
		m_current_pos = Position(-1, -1, -1);
	}

	void set_move_to(Position move_to) { m_move_to_pos = move_to; m_current_pos = Position(-1, -1, -1); }
	virtual bool perform(AIState* state, const TimeUnit& delta = TimeUnit::none()) override;
};

struct Behavior_Wander : public Behavior
{
protected:
	Coord m_wander_radius{ 3 };
	Position m_center{ -1, -1, -1 };

	TimeUnit m_min_wait{ TimeUnit::none() };
	TimeUnit m_max_wait{ TimeUnit::none() };

	TimeUnit m_wait_time{ TimeUnit::none() };

public:
	struct initializer
	{
		Coord wander_radius;
		Position center = { -1, -1, -1 };
		TimeUnit min_wait = TimeUnit::none();
		TimeUnit max_wait = TimeUnit::none();
	};

public:
	Behavior_Wander(const initializer& init) :
		m_wander_radius(init.wander_radius), 
		m_center(init.center), 
		m_min_wait(init.min_wait), 
		m_max_wait(init.max_wait)
	{
	}

	virtual bool perform(AIState* state, const TimeUnit& delta = TimeUnit::none()) override;
};

struct Behavior_GetToEntity : public Behavior
{
protected:
	Entity* m_target;

public:
	struct initializer
	{
		Entity* target;
	};

public:
	Behavior_GetToEntity(const initializer& init) :
		m_target(init.target)
	{
	}

	virtual bool perform(AIState* state, const TimeUnit& delta = TimeUnit::none()) override;
};


struct Behavior_GetAdjacentToEntity : public Behavior
{
protected:
	Entity* m_target;

public:
	struct initializer
	{
		Entity* target;
	};

public:
	Behavior_GetAdjacentToEntity(const initializer& init) :
		m_target(init.target)
	{
	}

	virtual bool perform(AIState* state, const TimeUnit& delta = TimeUnit::none()) override;
};

#endif // !LEMONADE_GAME_SRC_AI_BEHAVIOR_H