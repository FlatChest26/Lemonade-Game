#pragma once

#ifndef LEMONADE_GAME_SRC_CREATURE_DATA_H
#define LEMONADE_GAME_SRC_CREATURE_DATA_H

#include "thing.h"
#include "species.h"

/* Data about a creature that exists even when the creature is not on screen. */
// It is mutable and unique to each creature

class Entity;

class CreatureSoul : public Thing
{
protected:
	Entity* m_physical_form;

	ThingID m_ID;
	std::string m_description;
	Name m_name;
	Gender m_gender;
	color_t m_color;

public:

	constexpr bool is_creature_soul() const override { return true; }
	constexpr CreatureSoul* as_creature_soul() override { return this; }

	struct initializer
	{
		ThingID ID;
		Name name{};;
		Gender gender{};
		color_t color{ DEFAULT_FG_COLOR };
		std::string description;
	};

	CreatureSoul(const initializer& initializer = {});

	virtual ThingID ID() const override { return m_ID; }
	virtual std::string description() const override { return m_description; }

	// Physical Form

	void set_physical_form(Entity* form);
	Entity* get_physical_form() const;
	bool has_physical_form() const;

	// Name

	Name get_name() const override { return m_name; }

	// Gender

	// What to call this thing in a gendered context. (e.g. "man", "woman", "person")
	Gender get_gender() const { return m_gender; }

	Noun gender_noun() const { return m_gender.noun; }
	Noun young_gender_noun() const { return m_gender.young_noun; }

	// What pronouns to use for this thing
	Pronouns get_pronouns() const override { return m_gender.pronouns; }

	bool is_neuter() const { return m_gender.gender_type & Gender::type::NEUTER; }
	bool is_epicene() const { return m_gender.gender_type & Gender::type::EPICENE; }
	bool is_masculine() const { return m_gender.gender_type & Gender::type::MASCULINE; }
	bool is_feminine() const { return m_gender.gender_type & Gender::type::FEMININE; }

	// Species & Renderable

	virtual const Species* get_species() const;
	virtual Noun species_noun() const;

	Noun get_noun() const override
	{
		if (species_noun().empty())
			return gender_noun();
		else
			return species_noun();
	}

	virtual Renderable get_renderable() const { return Renderable(007, color::get("white"), color::get("black")); } // •
	virtual color_t fg() const override { return m_color; }
};

#endif // !LEMONADE_GAME_SRC_CREATURE_DATA_H