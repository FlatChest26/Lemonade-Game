#pragma once

#ifndef LEMONADE_GAME_SRC_ACTION_H
#define LEMONADE_GAME_SRC_ACTION_H

#include "time_units.h"
#include "keybinds.h"
#include "game_message.h"

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

using QueuedAction = Action*;
using ActionQueue = std::vector<QueuedAction>;
using ActionResponse = GameMessage;

struct ActionResult
{
	bool success{ false };
	std::vector<ActionResponse> responses{};
	ActionQueue next;

	constexpr bool has_next_action() const
	{
		return !next.empty();
	}

	void add_next_action(QueuedAction action)
	{
		next.push_back(action);
	}

	QueuedAction get_next_action()
	{
		if (next.front())
		{
			auto next_action = next.front();
			next.erase(next.begin());

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
	{
	}

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

	virtual bool is_action_complete() const { return m_time_spent_on_action >= m_action_length; }

	virtual TimeUnit get_action_length() const { return m_action_length; }
	virtual TimeUnit remaining_action_length() const { return m_action_length - m_time_spent_on_action; }

	static std::string action_name() { return "<unnamed action>"; }
	virtual bool check_action() const { return true; }

	virtual void set_actor(Entity* actor);
	virtual void set_subject(Entity* subject);

	virtual bool has_actor() const;
	virtual bool has_subject() const;

	virtual Entity* get_actor() const;
	virtual Entity* get_subject() const;

	virtual World* get_world() const;
	virtual TileMap* get_tile_map() const;
	virtual WorldGameState* get_world_state() const;

	virtual bool is_movement_action() const { return false; }
};

class WaitAction : public Action
{
public:
	WaitAction(const TimeUnit& action_length = TimeUnit::from_seconds(1.0)) :
		Action(action_length)
	{
	}

	virtual ActionResult perform() override { return ActionResult(true); }

	static std::string action_name() { return "wait"; }
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

	virtual void set_actor(Entity* actor) override;
	virtual bool check_action() const override;
	virtual Entity* get_actor() const override;

	static std::string action_name() { return "<unnamed entity action>"; }
};



#endif // !LEMONADE_GAME_SRC_ACTION_H