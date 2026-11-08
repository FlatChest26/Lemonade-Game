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
	ActionWithDirection(Entity* actor = nullptr, Coord dx = 0, Coord dy = 0, Coord dz = 0) :
		EntityAction(actor), m_dx(dx), m_dy(dy), m_dz(dz)
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
		ActionWithDirection(actor, dx, dy, dz)
	{
	}

	BumpAction(Entity* actor = nullptr, const Position& delta = { 0, 0, 0 }) :
		BumpAction(actor, delta.x, delta.y, delta.z)
	{
	}

	virtual TimeUnit get_action_length() const override { return TimeUnit::none(); }

	virtual bool check_action() const override;
	virtual ActionResult perform() override;

	static std::string action_name() { return "bump action"; }
};

class MovementAction : public ActionWithDirection
{
public:
	MovementAction(Entity* actor = nullptr, Coord dx = 0, Coord dy = 0, Coord dz = 0) :
		ActionWithDirection(actor, dx, dy, dz)
	{
	}

	MovementAction(Entity* actor = nullptr, const Position& delta = { 0, 0, 0 }) :
		ActionWithDirection(actor, delta.x, delta.y, delta.z)
	{
	}

	virtual TimeUnit get_action_length() const override;

	virtual bool check_action() const override;
	virtual ActionResult perform() override;
	static std::string action_name() { return "move"; }
};

#endif // !LEMONADE_GAME_SRC_DIRECTION_ACTION_H