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
	virtual bool perform(AIState* state, const TimeUnit& delta) = 0; // returns true when behavior is complete
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

	virtual bool perform(AIState* state, const TimeUnit& delta) override;
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

	virtual bool perform(AIState* state, const TimeUnit& delta) override;
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

	virtual bool perform(AIState* state, const TimeUnit& delta) override;
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

	virtual bool perform(AIState* state, const TimeUnit& delta) override;
};

struct EntityFilter;

struct Behavior_SearchForEntity : public Behavior
{
protected:
	EntityFilter* m_filter;
	Coord m_search_radius;

	std::function<bool(Entity*)> m_is_valid_callback;
	std::function<double(Entity*)> m_get_weight_callback;

	std::function<void(Entity*)> m_on_found_callback;

	int m_search_attempts{ 20 };

public:
	struct initializer
	{
		EntityFilter* filter;
		Coord search_radius{ 10 };

		std::function<bool(Entity*)> is_valid_callback;
		std::function<double(Entity*)> get_weight_callback;

		std::function<void(Entity*)> on_found_callback;
	};
	
	Behavior_SearchForEntity(initializer init):
		m_filter(init.filter), m_search_radius(init.search_radius),
		m_is_valid_callback(init.is_valid_callback), 
		m_get_weight_callback(init.get_weight_callback),
		m_on_found_callback(init.on_found_callback)
	{}

	~Behavior_SearchForEntity();

	virtual bool perform(AIState* state, const TimeUnit& delta) override;
};

#endif // !LEMONADE_GAME_SRC_AI_BEHAVIOR_H