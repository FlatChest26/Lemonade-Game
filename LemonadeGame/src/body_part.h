#pragma once

#ifndef LEMONADE_GAME_SRC_BODY_PART_H
#define LEMONADE_GAME_SRC_BODY_PART_H

#include "body_part_type.h"
#include "thing.h"
#include "stat_values.h"
#include "body_part_components.h"

#include "raw_master.h"

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

class BodyPart : public Thing
{
protected:

	Entity* m_owner;
	RawMaster::Ref<BodyPartType> m_type;

	ThingID m_ID;

	double m_rel_size;
	BodyPartPosition m_position;

	std::string m_prefix;

	// Components //

	std::unique_ptr<Allure> m_allure;

public:
	BodyPart(Entity* owner = nullptr, ThingID ID = "", RawMaster::Ref<BodyPartType> type = nullptr, double rel_size = 25.0, BodyPartPosition position = BodyPartPosition::CENTER, std::string prefix = "");

	// -- Getters -- //

	const BodyPartType* get_type() const { ASSERT(m_type, "Body part has no type"); return m_type.get(); }

	Entity* get_owner() const;
	Anatomy* get_anatomy() const;

	// -- Thing -- //

	BodyPartID ID() const { return m_ID; }

	virtual Noun noun() const override { return m_type->noun; }

	bool is_plural() const override { return m_type->plural; }

	BodyPartCategory category() const { return m_type->category; }

	virtual std::string prefix() const override;

	// -- Allure -- //

	virtual Allure* get_allure() const { return m_allure.get(); }
	bool has_allure() const { return m_allure != nullptr; }

	StatInt_t get_lewdness() const
	{
		if (!has_allure())
			return 0;

		return m_allure->lewdness * m_type->base_allure.lewdness;
	}

	StatInt_t get_suggestiveness() const
	{
		if (!has_allure())
			return 0;

		return m_allure->suggestiveness * m_type->base_allure.suggestiveness;
	}

	StatInt_t get_kinkiness() const
	{
		if (!has_allure())
			return 0;

		return m_allure->kinkiness * m_type->base_allure.kinkiness;
	}

	StatInt_t get_beauty() const
	{
		if (!has_allure())
			return 0;

		return m_allure->beauty * m_type->base_allure.beauty;
	}

public:
	// -- Flag Checks -- //

	BodyPartFlag flags() const { return m_type->flags; }

	bool has_flag(BodyPartFlag flag) { return m_type->flags & flag; }

	bool is_sensory() const { return m_type->flags & BodyPartFlag::SENSORY; }

	bool can_choke() const { return m_type->flags & BodyPartFlag::CHOKABLE; }

	bool is_grasp() const { return m_type->flags & BodyPartFlag::GRASP; }
	bool is_limb() const { return m_type->flags & BodyPartFlag::LIMB; }
	bool is_stance() const { return m_type->flags & BodyPartFlag::STANCE; }

	bool is_internal() const { return m_type->flags & BodyPartFlag::INTERNAL; }

	bool is_used_for_thought() const { return m_type->flags & BodyPartFlag::THOUGHT; }
	bool is_used_for_circulation() const { return m_type->flags & BodyPartFlag::CIRCULATION; }
	bool is_used_for_digestion() const { return m_type->flags & BodyPartFlag::DIGESTION; }

	bool is_used_to_eat() const { return m_type->flags & BodyPartFlag::EAT; }
	bool is_used_to_speak() const { return m_type->flags & BodyPartFlag::SPEAK; }

	bool is_phallus() const { return m_type->flags & BodyPartFlag::PHALLUS; }
	bool is_orifice() const { return m_type->flags & BodyPartFlag::ORIFICE; }
	bool is_handle() const { return m_type->flags & BodyPartFlag::HANDLE; }
	bool is_cleavage() const { return m_type->flags & BodyPartFlag::CLEAVAGE; }

	bool is_lewd() const { return m_type->flags & BodyPartFlag::LEWD; }
	bool is_suggestive() const { return m_type->flags & BodyPartFlag::SUGGESTIVE; }
	bool is_kinky() const { return m_type->flags & BodyPartFlag::KINKY; }
	bool is_pretty() const { return m_type->flags & BodyPartFlag::PRETTY; }

public:

	// -- Utilities -- //

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
