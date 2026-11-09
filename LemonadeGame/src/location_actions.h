#pragma once

#ifndef LEMONADE_GAME_SRC_LOCATION_ACTIONS_H
#define LEMONADE_GAME_SRC_LOCATION_ACTIONS_H

#include "action.h"
#include "transforms.h"

class Tile;

class ActionWithLocation : public EntityAction
{
protected:
	Coord m_loc_x{ 0 }, m_loc_y{ 0 }, m_loc_z{ 0 };

public:
	ActionWithLocation(Entity* actor, Coord loc_x, Coord loc_y, Coord loc_z) :
		EntityAction(actor), m_loc_x(loc_x), m_loc_y(loc_y), m_loc_z(loc_z)
	{
	}
	ActionWithLocation(Entity* actor, const Position& location) :
		ActionWithLocation(actor, location.x, location.y, location.z)
	{
	}

	// -- Getters -- //

	Coord loc_x() const { return m_loc_x; }
	Coord loc_y() const { return m_loc_y; }
	Coord loc_z() const { return m_loc_z; }
	Position location() const { return Position(m_loc_x, m_loc_y, m_loc_z); }

	Tile* get_tile_at_dest();
	const Tile* get_tile_at_dest() const;

	// -- Misc -- //

	virtual bool check_action() const override;
};

class TransformAction : public ActionWithLocation
{
protected:
	bool m_open{ true };

public:
	TransformAction(Entity* actor, Coord loc_x, Coord loc_y, Coord loc_z, bool open) :
		ActionWithLocation(actor, loc_x, loc_y, loc_z), m_open(open)
	{
	}
	TransformAction(Entity* actor, const Position& location, bool open) :
		ActionWithLocation(actor, location), m_open(open)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();
	static std::string action_name() { return "transform action"; }
};

class OpenAction : public TransformAction
{
public:
	OpenAction(Entity* actor, Coord loc_x, Coord loc_y, Coord loc_z) :
		TransformAction(actor, loc_x, loc_y, loc_z, true)
	{
	}
	OpenAction(Entity* actor, const Position& location) :
		TransformAction(actor, location, true)
	{
	}

	virtual bool check_action() const override;
	static std::string action_name() { return "open action"; }
};

class CloseAction : public TransformAction
{
public:
	CloseAction(Entity* actor, Coord loc_x, Coord loc_y, Coord loc_z) :
		TransformAction(actor, loc_x, loc_y, loc_z, false)
	{
	}
	CloseAction(Entity* actor, const Position& location) :
		TransformAction(actor, location, false)
	{
	}

	virtual bool check_action() const override;
	static std::string action_name() { return "close action"; }
};

#endif // !LEMONADE_GAME_SRC_LOCATION_ACTIONS_H