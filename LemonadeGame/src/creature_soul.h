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
	ThingID ID;
	Name name{};
	Gender gender{};
	color_t color{ DEFAULT_FG_COLOR };
	std::string description;

	// Gender

	// What to call this thing in a gendered context. (e.g. "man", "woman", "person")
	Noun gender_noun() const { return gender.noun; }
	Noun young_gender_noun() const { return gender.young_noun; }

	// What pronouns to use for this thing
	Pronouns get_pronouns() const { return gender.pronouns; }

	bool is_neuter() const { return gender.gender_type & Gender::type::NEUTER; }
	bool is_epicene() const { return gender.gender_type & Gender::type::EPICENE; }
	bool is_masculine() const { return gender.gender_type & Gender::type::MASCULINE; }
	bool is_feminine() const { return gender.gender_type & Gender::type::FEMININE; }

	Renderable renderable() const { return Renderable(007, color::get("white"), color::get("black")); } // •
	Noun noun() const { return Noun("soul", "souls", false); }
};

#endif // !LEMONADE_GAME_SRC_CREATURE_DATA_H