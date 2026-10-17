#pragma once

#ifndef LEMONADE_GAME_SRC_DIRECTION_ACTION_H
#define LEMONADE_GAME_SRC_DIRECTION_ACTION_H

#include "action.h"
#include "transforms.h"
#include "entity.h"

class ActionWithDirection : public Action
{
protected:
	Coord m_dx{ 0 };
	Coord m_dy{ 0 };
	Coord m_dz{ 0 };

public:
	ActionWithDirection(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, const Coord& dx = 0, const Coord& dy = 0, const Coord& dz = 0);

	// -- Getters -- //

	virtual bool check_action() override
	{
		if (!Action::check_action())
			return false;

		return m_actor->has_movable();
	}

	Position delta_pos() const { return Position(m_dx, m_dy, m_dz); }
	Position destination() const
	{
		if (!m_actor->has_movable())
		{
			CERR("Actor has no movable!" << m_actor->ID());
			return Position();
		}

		return m_actor->get_movable()->pos() + delta_pos();
	}

	Coord destx() const { return destination().x; }
	Coord desty() const { return destination().y; }
	Coord destz() const { return destination().z; }
};

class BumpAction : public ActionWithDirection
{
public:
	BumpAction(Entity* actor = nullptr, const Coord& dx = 0, const Coord& dy = 0, const Coord& dz = 0);
	BumpAction(Entity* actor = nullptr, const Position& delta = {0, 0, 0}) :
		BumpAction(actor, delta.x, delta.y, delta.z)
	{}

	virtual ActionResult perform() override;

	virtual std::string get_action_name() const override
	{
		return "bump action";
	}
};

class MovementAction : public ActionWithDirection
{
public:
	MovementAction(const TimeUnit& action_length = TimeUnit::none(), Entity* entity = nullptr, const Coord& dx = 0, const Coord& dy = 0, const Coord& dz = 0);

	virtual ActionResult perform() override;

	virtual std::string get_action_name() const override
	{
		return "movement action";
	}
};

#endif // !LEMONADE_GAME_SRC_DIRECTION_ACTION_H