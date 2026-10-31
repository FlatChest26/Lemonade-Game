#pragma once

#ifndef LEMONADE_GAME_SRC_CREATURE_DATA_H
#define LEMONADE_GAME_SRC_CREATURE_DATA_H

#include "thing.h"
#include "species.h"

/* Data about a creature that exists even when the creature is not on screen. */
// It is mutable and unique to each creature

class Entity;

struct CreatureSoul
{
	Name name;
	RawMaster::Ref<Gender> gender;
	color_t color{ DEFAULT_FG_COLOR };
	std::string description;

	Renderable renderable() const { return Renderable(007, color::get("white"), color::get("black")); } // •
	Noun noun() const { return Noun("soul", "souls", false); }
};

#endif // !LEMONADE_GAME_SRC_CREATURE_DATA_H