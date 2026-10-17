#pragma once

#ifndef LEMONADE_ENGINE_SRC_CAMERA_H
#define LEMONADE_ENGINE_SRC_CAMERA_H

#include "movable.h"
#include "output.h"

static constexpr Length CAMERA_VIEW_DEPTH = 4;

class Camera : public Movable<Position, Size>
{
public:
	// -- Getters -- //

	Coord get_x1() const { return posx() - (static_cast<Coord>(width()) / 2); }
	Coord get_y1() const { return posy() - (static_cast<Coord>(height()) / 2); }
	Coord get_z1() const { return posz() - static_cast<Coord>(depth()) - 1; }

	Coord get_x2() const { return posx() + (static_cast<Coord>(width()) / 2); }
	Coord get_y2() const { return posy() + (static_cast<Coord>(height()) / 2); }
	Coord get_z2() const { return posz() + 1; }

public:
	explicit Camera(TransformType transform = { DEFAULT_POSITION, {1, 1, CAMERA_VIEW_DEPTH }, BELOW }) :
		Movable(transform)
	{
	}

	~Camera() {}

public:
	// -- Utility -- //

	// Returns true if the coordinates lie in the view of this camera.
	bool in_view(Coord x, Coord y, Coord z) const { return get_x1() <= x && x < get_x2() && get_y1() <= y && y < get_y2() && get_z1() <= z && z < get_z2(); }
	bool in_view(Position p) const { return in_view(p.x, p.y, p.z); }

	Position get_screen(Coord x, Coord y, Coord z) const
	{
		return Position(
			(x - get_x1()) + (int)((output::get_console_width() - width()) / 2),
			(y - get_y1()) + (int)((output::get_console_height() - height()) / 2),
			-(z + 1 - get_z2())
		);
	}

	Position get_screen(Position p) const { return get_screen(p.x, p.y, p.z); }

	Position get_local(Coord x, Coord y, Coord z) const { return Position(x + get_x1(), y + get_y1(), z - 1 + get_z2()); }
	Position get_local(Position p) const { return get_local(p.x, p.y, p.z); }
};

#endif // !LEMONADE_ENGINE_SRC_CAMERA_H