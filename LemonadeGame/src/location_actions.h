#pragma once

#ifndef LEMONADE_GAME_SRC_LOCATION_ACTIONS_H
#define LEMONADE_GAME_SRC_LOCATION_ACTIONS_H

#include "action.h"
#include "transforms.h"

class Tile;

class ActionWithLocation : public Action
{
protected:
	Coord m_loc_x{ 0 }, m_loc_y{ 0 }, m_loc_z{ 0 };

public:
	ActionWithLocation(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, const Coord& loc_x = 0, const Coord& loc_y = 0, const Coord& loc_z = 0) :
		Action(action_length, actor), m_loc_x(loc_x), m_loc_y(loc_y), m_loc_z(loc_z)
	{
	}
	ActionWithLocation(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, const Position& location = Position(0, 0, 0)) :
		ActionWithLocation(action_length, actor, location.x, location.y, location.z)
	{
	}

	// -- Getters -- //

	Coord loc_x() const { return m_loc_x; }
	Coord loc_y() const { return m_loc_y; }
	Coord loc_z() const { return m_loc_z; }
	Position location() const { return Position(m_loc_x, m_loc_y, m_loc_z); }

	Tile* get_tile_at_dest();

	// -- Misc -- //

	virtual bool check_action() override;
};

class TransformAction : public ActionWithLocation
{
protected:
	bool m_open{ true };

public:
	TransformAction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, const Coord& loc_x = 0, const Coord& loc_y = 0, const Coord& loc_z = 0, bool open = true) :
		ActionWithLocation(action_length, actor, loc_x, loc_y, loc_z), m_open(open)
	{
	}
	TransformAction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, const Position& location = Position(0, 0, 0), bool open = true) :
		ActionWithLocation(action_length, actor, location), m_open(open)
	{
	}

	virtual ActionResult perform();
};

class OpenAction : public TransformAction
{
public:
	OpenAction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, const Coord& loc_x = 0, const Coord& loc_y = 0, const Coord& loc_z = 0) :
		TransformAction(action_length, actor, loc_x, loc_y, loc_z, true)
	{
	}
	OpenAction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, const Position& location = Position(0, 0, 0)) :
		TransformAction(action_length, actor, location, true)
	{
	}

	virtual bool check_action() override;
};

class CloseAction : public TransformAction
{
public:
	CloseAction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, const Coord& loc_x = 0, const Coord& loc_y = 0, const Coord& loc_z = 0) :
		TransformAction(action_length, actor, loc_x, loc_y, loc_z, false)
	{
	}
	CloseAction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, const Position& location = Position(0, 0, 0)) :
		TransformAction(action_length, actor, location, false)
	{
	}

	virtual bool check_action() override;
};

#endif // !LEMONADE_GAME_SRC_LOCATION_ACTIONS_H