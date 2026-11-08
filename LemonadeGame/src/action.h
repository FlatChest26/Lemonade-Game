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

using QueuedAction = std::unique_ptr<Action>;
using ActionQueue = std::vector<QueuedAction>;
using ActionResponse = GameMessage;

struct ActionResult
{
	bool success{ false };
	std::vector<ActionResponse> responses{};
	Action* next;
};

class Action
{
protected:
	TimeUnit m_time_spent_on_action{ TimeUnit::none() };

public:

	virtual ActionResult perform();

	virtual bool reset_progress(const TimeUnit& delta = TimeUnit::none())
	{
		m_time_spent_on_action = delta;
		return is_action_complete();
	}

	virtual bool progress(const TimeUnit& delta)
	{
		m_time_spent_on_action += delta;
		return is_action_complete();
	}

	virtual bool is_action_complete() const { return m_time_spent_on_action >= get_action_length(); }

	virtual TimeUnit get_action_length() const { return TimeUnit::from_seconds(1.0); }
	virtual TimeUnit remaining_action_length() const { return get_action_length() - m_time_spent_on_action; }

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
};

class WaitAction : public Action
{
protected:
	TimeUnit m_wait_time;

public:
	WaitAction(const TimeUnit& wait_time) :
		m_wait_time(wait_time)
	{
	}

	virtual TimeUnit get_action_length() const override { return m_wait_time; }
	virtual ActionResult perform() override { return ActionResult(true); }

	static std::string action_name() { return "wait"; }
};

class EntityAction : public Action
{
protected:
	Entity* m_actor;

public:
	EntityAction(Entity* actor = nullptr) :
		m_actor(actor)
	{
	}

	virtual TimeUnit get_action_length() const override;

	virtual void set_actor(Entity* actor) override;
	virtual bool check_action() const override;
	virtual Entity* get_actor() const override;

	static std::string action_name() { return "<unnamed entity action>"; }
};



#endif // !LEMONADE_GAME_SRC_ACTION_H