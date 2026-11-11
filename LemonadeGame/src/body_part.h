#pragma once

#ifndef LEMONADE_GAME_SRC_BODY_PART_H
#define LEMONADE_GAME_SRC_BODY_PART_H

#include "body_part_type.h"
#include "thing.h"
#include "stat_values.h"
#include "body_part_components.h"

#include "raw_master.h"
#include "weapon_info.h"


class Entity;
class Anatomy;
struct BodyPartPlan;

enum class BodyPartPosition
{
	CENTER,
	SIDES,
	FRONT,
	BACK,
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	INSIDE,
};

enum class BodyPartRelativePosition
{
	INSIDE,
	AROUND,

	ABOVE,
	BELOW,

	IN_FRONT,
	BEHIND,

	SIDES,
	BETWEEN,

	LEFT,
	RIGHT,
};

inline std::map<std::string, BodyPartPosition> AllBodyPartPositions
{
	{ "CENTER", BodyPartPosition::CENTER },
	{ "SIDES", BodyPartPosition::SIDES },
	{ "FRONT", BodyPartPosition::FRONT },
	{ "BACK", BodyPartPosition::BACK },
	{ "TOP", BodyPartPosition::TOP },
	{ "BOTTOM", BodyPartPosition::BOTTOM },
	{ "LEFT", BodyPartPosition::LEFT },
	{ "RIGHT", BodyPartPosition::RIGHT },
	{ "INSIDE", BodyPartPosition::INSIDE },
};

inline std::map<std::string, BodyPartRelativePosition> AllBodyPartRelativePosition
{
	{ "INSIDE", BodyPartRelativePosition::INSIDE },
	{ "AROUND", BodyPartRelativePosition::AROUND },
	{ "ABOVE", BodyPartRelativePosition::ABOVE },
	{ "BELOW", BodyPartRelativePosition::BELOW },
	{ "IN_FRONT", BodyPartRelativePosition::IN_FRONT },
	{ "BEHIND", BodyPartRelativePosition::BEHIND },
	{ "SIDES", BodyPartRelativePosition::SIDES },
	{ "BETWEEN", BodyPartRelativePosition::BETWEEN },
	{ "LEFT", BodyPartRelativePosition::LEFT },
	{ "RIGHT", BodyPartRelativePosition::RIGHT },
};

using BodyPartID = std::string;

class BodyPart : public Thing
{
protected:

	Entity* m_owner;

	Noun m_noun;
	RawMaster::Ref<BodyPartType> m_type;

	BodyPartID m_ID;

	double m_rel_size;
	BodyPartPosition m_position;

	std::string m_prefix;

	// Components //

	Allure m_allure;
	Health m_health;

public:
	BodyPart(Entity* owner = nullptr, BodyPartID ID = "", RawMaster::Ref<BodyPartType> type = nullptr, double rel_size = 25.0, BodyPartPosition position = BodyPartPosition::CENTER, std::string prefix = "");

	// -- Getters -- //

	const BodyPartType* get_type() const { ASSERT(m_type, "Body part has no type"); return m_type.get(); }

	void set_owner(Entity* entity);
	Entity* get_owner() const;
	Anatomy* get_anatomy() const;

	double get_relative_size() const { return m_rel_size; }

	Health& health() { return m_health; }
	const Health& health() const { return m_health; }

	Allure& allure() { return m_allure; }
	const Allure& allure() const { return m_allure; }

	void take_damage(const Damage& damage);


	// -- Thing -- //

	BodyPartID ID() const { return m_ID; }

	void set_noun(const Noun& noun) { m_noun = noun; }

	virtual Noun noun() const override { return m_noun; }
	virtual GrammarNoun display_noun() const override;

	bool is_plural() const override { return m_type->multiple > 1; }

	BodyPartCategory category() const { return m_type->category; }

	virtual std::string prefix() const override;

	// -- Allure -- //

	StatInt_t get_lewdness() const
	{
		return m_allure.lewdness * m_type->base_allure.lewdness;
	}

	StatInt_t get_suggestiveness() const
	{
		return m_allure.suggestiveness * m_type->base_allure.suggestiveness;
	}

	StatInt_t get_kinkiness() const
	{
		return m_allure.kinkiness * m_type->base_allure.kinkiness;
	}

	StatInt_t get_beauty() const
	{
		return m_allure.beauty * m_type->base_allure.beauty;
	}

public:
	// -- Flag Checks -- //

	BodyPartFlag flags() const { return m_type->flags; }

	bool has_flag(BodyPartFlag flag) const { return m_type->flags & flag; }

	bool is_important() const { return m_type->flags & BodyPartFlag::IMPORTANT; }

	bool is_grasp() const { return m_type->flags & BodyPartFlag::GRASP; }
	bool is_stance() const { return m_type->flags & BodyPartFlag::STANCE; }

	bool is_embedded() const { return m_type->flags & BodyPartFlag::EMBEDDED; }
	bool is_internal() const { return m_type->flags & BodyPartFlag::INTERNAL; }

	bool is_phallus() const { return m_type->flags & BodyPartFlag::PHALLUS; }
	bool is_orifice() const { return m_type->flags & BodyPartFlag::ORIFICE; }

	bool is_small() const { return m_type->flags & BodyPartFlag::SMALL; }

	bool is_used_to_sight() const { return m_type->flags & BodyPartFlag::SIGHT; }

	//bool is_sensory() const { return m_type->flags & BodyPartFlag::SENSORY; }

	//bool can_choke() const { return m_type->flags & BodyPartFlag::CHOKABLE; }
	//bool is_limb() const { return m_type->flags & BodyPartFlag::LIMB; }
	//bool is_stance() const { return m_type->flags & BodyPartFlag::STANCE; }
	
	//bool is_used_for_thought() const { return m_type->flags & BodyPartFlag::THOUGHT; }
	//bool is_used_for_circulation() const { return m_type->flags & BodyPartFlag::CIRCULATION; }
	//bool is_used_for_digestion() const { return m_type->flags & BodyPartFlag::DIGESTION; }

	//bool is_used_to_eat() const { return m_type->flags & BodyPartFlag::EAT; }
	//bool is_used_to_speak() const { return m_type->flags & BodyPartFlag::SPEAK; }

	//
	//
	//bool is_handle() const { return m_type->flags & BodyPartFlag::HANDLE; }
	//bool is_cleavage() const { return m_type->flags & BodyPartFlag::CLEAVAGE; }

	//bool is_lewd() const { return m_type->flags & BodyPartFlag::LEWD; }
	//bool is_suggestive() const { return m_type->flags & BodyPartFlag::SUGGESTIVE; }
	//bool is_kinky() const { return m_type->flags & BodyPartFlag::KINKY; }
	//bool is_pretty() const { return m_type->flags & BodyPartFlag::PRETTY; }

	//bool is_small() const { return m_type->flags & BodyPartFlag::SMALL; }

public:

	// -- Utilities -- //

	bool operator==(const BodyPart& other)
	{
		return ID() == other.ID();
	}

	bool operator!=(const BodyPart& other)
	{
		return ID() != other.ID();
	}

	static BodyPartRelativePosition opposite_of(BodyPartRelativePosition rel_pos)
	{
		switch (rel_pos)
		{
		case BodyPartRelativePosition::INSIDE:
			return BodyPartRelativePosition::AROUND;
		case BodyPartRelativePosition::AROUND:
			return BodyPartRelativePosition::INSIDE;

		case BodyPartRelativePosition::ABOVE:
			return BodyPartRelativePosition::BELOW;
		case BodyPartRelativePosition::BELOW:
			return BodyPartRelativePosition::ABOVE;

		case BodyPartRelativePosition::IN_FRONT:
			return BodyPartRelativePosition::BEHIND;
		case BodyPartRelativePosition::BEHIND:
			return BodyPartRelativePosition::IN_FRONT;

		case BodyPartRelativePosition::SIDES:
			return BodyPartRelativePosition::BETWEEN;
		case BodyPartRelativePosition::BETWEEN:
			return BodyPartRelativePosition::SIDES;

		case BodyPartRelativePosition::LEFT:
			return BodyPartRelativePosition::RIGHT;
		case BodyPartRelativePosition::RIGHT:
			return BodyPartRelativePosition::LEFT;

		default:
			return BodyPartRelativePosition::INSIDE;
		}
	}
};

#endif // !BODY_PART_H
