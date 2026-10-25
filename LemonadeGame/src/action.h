#pragma once

#ifndef LEMONADE_GAME_SRC_ACTION_H
#define LEMONADE_GAME_SRC_ACTION_H

#include "time_units.h"
#include <memory>
#include <vector>
#include <string>
#include <utility>

#define _LG_USE_ACTION_WARNINGS

#ifdef _LG_USE_ACTION_WARNINGS
#define ACTION_CERR(...) CERR(__VA_ARGS__)
#else
#define ACTION_CERR(...)
#endif // _LG_USE_ACTION_WARNINGS



class Action;
class Entity;
class World;
class WorldGameState;
class TileMap;

using QueuedAction = std::unique_ptr<Action>;
using ActionQueue = std::vector<QueuedAction>;
using ActionResponse = std::string;

struct ActionResult
{
	bool success{ false };
	ActionResponse response{ "" };
	ActionQueue next_actions;

	ActionResult(bool success, ActionResponse response, ActionQueue next_actions) :
		success(success), response(response)
	{
		for (auto& a : next_actions)
		{
			add_next_action(std::move(a));
		}
	}

	ActionResult(bool success, ActionResponse response, QueuedAction next_action) :
		success(success), response(response)
	{
		add_next_action(std::move(next_action));
	}

	ActionResult(bool success, ActionQueue next_actions) :
		success(success), response("")
	{
		for (auto& a : next_actions)
		{
			add_next_action(std::move(a));
		}
	}

	ActionResult(bool success, QueuedAction next_action) :
		success(success), response("")
	{
		add_next_action(std::move(next_action));
	}

	ActionResult(bool success, ActionResponse response) :
		success(success), response(response)
	{
	}

	ActionResult(bool success) :
		success(success), response("")
	{
	}

	constexpr bool has_next_action() const
	{
		return !next_actions.empty();
	}

	void add_next_action(std::unique_ptr<Action> action)
	{
		next_actions.push_back(std::move(action));
	}

	std::unique_ptr<Action> get_next_action()
	{
		if (next_actions.front())
		{
			auto next_action = std::move(next_actions.front());
			next_actions.erase(next_actions.begin());

			return next_action;
		}

		return nullptr;
	}
};

class Action
{
protected:
	TimeUnit m_action_length{ TimeUnit::none() };
	TimeUnit m_time_spent_on_action{ TimeUnit::none() };

public:
	Action(const TimeUnit& action_length = TimeUnit::none()) :
		m_action_length(action_length)
	{}

	virtual ActionResult perform();

	virtual bool reset_progress(const TimeUnit& delta)
	{
		m_time_spent_on_action = delta;
		return is_action_complete();
	}

	virtual bool progress(const TimeUnit& delta)
	{
		m_time_spent_on_action += delta;
		return is_action_complete();
	}

	virtual bool is_action_complete() const
	{
		return m_time_spent_on_action >= m_action_length;
	}

	virtual TimeUnit get_action_length() const
	{
		return m_action_length;
	}

	virtual std::string get_action_name() const
	{
		return "default action";
	}

	virtual bool check_action() const
	{
		return true;
	}

	virtual bool has_actor() const;

	virtual Entity* get_actor() const;
	virtual World* get_world() const;
	virtual TileMap* get_tile_map() const;
	virtual WorldGameState* get_world_game_state() const;
};

class WaitAction : public Action
{
public:
	WaitAction(const TimeUnit& action_length = TimeUnit::from_seconds(1.0)) :
		Action(action_length)
	{
	}

	virtual ActionResult perform() override
	{
		return ActionResult(true);
	}

	virtual std::string get_action_name() const override
	{
		return "wait action";
	}
};

class EntityAction : public Action
{
protected:
	Entity* m_actor;

public:
	EntityAction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr) :
		Action(action_length), m_actor(actor)
	{
	}

	virtual bool check_action() const override;

	virtual Entity* get_actor() const override;

	virtual std::string get_action_name() const override
	{
		return "entity action";
	}
};

#endif // !LEMONADE_GAME_SRC_ACTION_H