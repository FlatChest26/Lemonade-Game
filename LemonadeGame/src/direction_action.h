#pragma once

#ifndef LEMONADE_GAME_SRC_DIRECTION_ACTION_H
#define LEMONADE_GAME_SRC_DIRECTION_ACTION_H

#include "action.h"
#include "transforms.h"
#include "entity.h"

class ActionWithDirection : public EntityAction
{
protected:
	Coord m_dx{ 0 };
	Coord m_dy{ 0 };
	Coord m_dz{ 0 };

public:
	ActionWithDirection(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, Coord dx = 0, Coord dy = 0, Coord dz = 0) :
		EntityAction(action_length, actor), m_dx(dx), m_dy(dy), m_dz(dz)
	{
	}

	// -- Getters -- //

	virtual bool check_action() const override;

	Position delta_pos() const { return Position(m_dx, m_dy, m_dz); }
	Position destination() const { return m_actor->get_movable()->pos() + delta_pos(); }

	Coord destx() const { return destination().x; }
	Coord desty() const { return destination().y; }
	Coord destz() const { return destination().z; }
};

class BumpAction : public ActionWithDirection
{
public:
	BumpAction(Entity* actor = nullptr, Coord dx = 0, Coord dy = 0, Coord dz = 0) :
		ActionWithDirection(TimeUnit::none(), actor, dx, dy, dz)
	{
	}

	BumpAction(Entity* actor = nullptr, Position delta = { 0, 0, 0 }) :
		BumpAction(actor, delta.x, delta.y, delta.z)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform() override;

	static std::string action_name() { return "bump action"; }
};

class MovementAction : public ActionWithDirection
{
public:
	MovementAction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, Coord dx = 0, Coord dy = 0, Coord dz = 0) :
		ActionWithDirection(action_length, actor, dx, dy, dz)
	{
	}
	MovementAction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, Position delta = { 0, 0, 0 }) :
		ActionWithDirection(action_length, actor, delta.x, delta.y, delta.z)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform() override;
	static std::string action_name() { return "movement action"; }
};

#endif // !LEMONADE_GAME_SRC_DIRECTION_ACTION_H