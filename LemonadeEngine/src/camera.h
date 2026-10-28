#pragma once

#ifndef LEMONADE_ENGINE_SRC_CAMERA_H
#define LEMONADE_ENGINE_SRC_CAMERA_H

#include "movable.h"
#include "output.h"

static constexpr Length CAMERA_VIEW_DEPTH = 2;

class Camera : public Movable<Position, Size>
{
public:
	// -- Getters -- //

	Position get_center_offset() const
	{
		return Position(static_cast<Coord>(get_width() / 2), static_cast<Coord>(get_height() / 2), 0);
	}

	Position get_p1() const { return get_pos(); }

	Coord get_x1() const { return get_p1().x; }
	Coord get_y1() const { return get_p1().y; }
	Coord get_z1() const { return get_p1().z; }

	Position get_p2() const { return get_pos() + Position(static_cast<Coord>(width()), static_cast<Coord>(height()), -static_cast<Coord>(depth())); }

	Coord get_x2() const { return get_p2().x; }
	Coord get_y2() const { return get_p2().y; }
	Coord get_z2() const { return get_p2().z; }

public:
	explicit Camera(TransformType transform = { DEFAULT_POSITION,{ 1, 1, CAMERA_VIEW_DEPTH }, BELOW }) :
		Movable(transform)
	{
	}

	~Camera() {}

public:
	// -- Utility -- //

	// Returns true if the coordinates lie in the view of this camera.

	// Positions the view so the input is at the exact center of the camera.
	void set_center(const Position& p)
	{
		m_transform.position = p - get_center_offset();
	}

	bool in_view(const Coord& x, const Coord& y, const Coord& z) const
	{
		return get_x1() <= x && x < get_x2() && get_y1() <= y && y < get_y2() && get_z2() < z && z <= get_z1();
	}

	bool in_view(const Position& p) const { return in_view(p.x, p.y, p.z); }
	bool in_view(const TileTransform& t) const
	{
		return in_view(t.position) || in_view(t.position + static_cast<Position>(t.size));
	}

	bool fully_in_view(const TileTransform& t) const
	{
		return in_view(t.position) && in_view(t.position + static_cast<Position>(t.size));
	}

	// Camera coordinates from world coordinates.
	Position get_screen(const Coord& x, const Coord& y, const Coord& z) const
	{
		return Position(x - get_x1(), y - get_y1(), z - get_z1());
	}

	// Camera coordinates from world coordinates.
	Position get_screen(const Position& p) const { return get_screen(p.x, p.y, p.z); }

	// World coordinates from camera coordinates.
	Position get_local(const Coord& x, const Coord& y, const Coord& z) const
	{
		return Position(x + get_x1(), y + get_y1(), z + get_z1());
	}

	// World coordinates from camera coordinates.
	Position get_local(const Position& p) const { return get_local(p.x, p.y, p.z); }
};

#endif // !LEMONADE_ENGINE_SRC_CAMERA_H