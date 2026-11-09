#pragma once

#ifndef LEMONADE_GAME_SRC_AI_BEHAVIOR_H
#define LEMONADE_GAME_SRC_AI_BEHAVIOR_H

#include <vector>
#include "transforms.h"
#include "time_units.h"

class AIBrain;
class AIState;

class Behavior
{
public:
	virtual bool perform(AIState* state, const TimeUnit& delta = TimeUnit::none()) = 0; // returns true when behavior is complete
};

class Behavior_MoveTo : public Behavior
{
protected:
	Position m_move_to{};
	Position m_current_pos{};
	std::vector<Position> m_path{};
public:
	Behavior_MoveTo(Position move_to) :
		m_move_to(move_to)
	{
		m_current_pos = Position(-1, -1, -1);
	}

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
	Behavior_Wander(initializer init) :
		m_wander_radius(init.wander_radius), m_center(init.center), m_min_wait(init.min_wait), m_max_wait(init.max_wait)
	{
	}

	virtual bool perform(AIState* state, const TimeUnit& delta = TimeUnit::none()) override;
};

#endif // !LEMONADE_GAME_SRC_AI_BEHAVIOR_H