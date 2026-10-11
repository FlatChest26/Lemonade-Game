#pragma once

#ifndef LEMONADE_GAME_SRC_BODY_PART_H
#define LEMONADE_GAME_SRC_BODY_PART_H

#include "body_part_type.h"
#include "thing.h"
#include "stat_values.h"
#include "body_part_components.h"

class Creature;
class Anatomy;
struct BodyPartPlan;

class BodyPart : public Thing
{
public:
	enum class Position
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

	enum class RelativePosition
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

protected:

	Creature* m_owner;
	const BodyPartType* m_type;

	ThingID m_ID;

	double m_rel_size;
	Position m_position;

	std::string m_prefix;

	std::unique_ptr<Allure> m_allure;

public:
	BodyPart(Creature* owner = nullptr, ThingID ID = "", const BodyPartType* type = nullptr, double rel_size = 25.0, BodyPart::Position position = BodyPart::Position::CENTER, std::string prefix = "");

	// -- Getters -- //

	constexpr const BodyPartType* get_type() const { ASSERT(m_type, "Body part has no type"); return m_type; }

	virtual constexpr Creature* get_owner() const;
	virtual constexpr Anatomy* get_anatomy() const;

	// -- Thing -- //

	constexpr BodyPartID ID() const override { return m_ID; }

	constexpr Noun get_noun() const override { return get_type()->noun; }
	constexpr Pronouns get_pronouns() const override { return pronouns::IT_ITS; }

	constexpr bool is_plural() const override { return get_type()->plural; }

	constexpr BodyPartType::Category category() const { return get_type()->category; }

	virtual constexpr std::string prefix() const override;

	// -- Allure -- //

	virtual Allure* get_allure() const { return m_allure.get(); }
	bool has_allure() const { return m_allure != nullptr; }

	StatInt_t get_lewdness() const
	{
		if (!has_allure())
			return 0;

		return m_allure->lewdness * get_type()->base_allure.lewdness;
	}

	StatInt_t get_suggestiveness() const
	{
		if (!has_allure())
			return 0;

		return m_allure->suggestiveness * get_type()->base_allure.suggestiveness;
	}

	StatInt_t get_kinkiness() const
	{
		if (!has_allure())
			return 0;

		return m_allure->kinkiness * get_type()->base_allure.kinkiness;
	}

	StatInt_t get_beauty() const
	{
		if (!has_allure())
			return 0;

		return m_allure->beauty * get_type()->base_allure.beauty;
	}

public:
	// -- Flag Checks -- //

	bool is_sensory() const { return get_type()->flags & BodyPartType::Flag::SENSORY; }

	bool can_choke() const { return get_type()->flags & BodyPartType::Flag::CHOKABLE; }

	bool is_grasp() const { return get_type()->flags & BodyPartType::Flag::GRASP; }
	bool is_limb() const { return get_type()->flags & BodyPartType::Flag::LIMB; }
	bool is_stance() const { return get_type()->flags & BodyPartType::Flag::STANCE; }

	bool is_internal() const { return get_type()->flags & BodyPartType::Flag::INTERNAL; }

	bool is_used_for_thought() const { return get_type()->flags & BodyPartType::Flag::THOUGHT; }
	bool is_used_for_circulation() const { return get_type()->flags & BodyPartType::Flag::CIRCULATION; }
	bool is_used_for_digestion() const { return get_type()->flags & BodyPartType::Flag::DIGESTION; }

	bool is_used_to_eat() const { return get_type()->flags & BodyPartType::Flag::EAT; }
	bool is_used_to_speak() const { return get_type()->flags & BodyPartType::Flag::SPEAK; }

	bool is_phallus() const { return get_type()->flags & BodyPartType::Flag::PHALLUS; }
	bool is_orifice() const { return get_type()->flags & BodyPartType::Flag::ORIFICE; }
	bool is_handle() const { return get_type()->flags & BodyPartType::Flag::HANDLE; }
	bool is_cleavage() const { return get_type()->flags & BodyPartType::Flag::CLEAVAGE; }

	bool is_lewd() const { return get_type()->flags & BodyPartType::Flag::LEWD; }
	bool is_suggestive() const { return get_type()->flags & BodyPartType::Flag::SUGGESTIVE; }
	bool is_kinky() const { return get_type()->flags & BodyPartType::Flag::KINKY; }
	bool is_pretty() const { return get_type()->flags & BodyPartType::Flag::PRETTY; }

public:

	// -- Utilities -- //

	static RelativePosition opposite_of(RelativePosition rel_pos)
	{
		switch (rel_pos)
		{
		case RelativePosition::INSIDE:
			return RelativePosition::AROUND;
		case RelativePosition::AROUND:
			return RelativePosition::INSIDE;

		case RelativePosition::ABOVE:
			return RelativePosition::BELOW;
		case RelativePosition::BELOW:
			return RelativePosition::ABOVE;

		case RelativePosition::IN_FRONT:
			return RelativePosition::BEHIND;
		case RelativePosition::BEHIND:
			return RelativePosition::IN_FRONT;

		case RelativePosition::SIDES:
			return RelativePosition::BETWEEN;
		case RelativePosition::BETWEEN:
			return RelativePosition::SIDES;

		case RelativePosition::LEFT:
			return RelativePosition::RIGHT;
		case RelativePosition::RIGHT:
			return RelativePosition::LEFT;

		default:
			return RelativePosition::INSIDE;
		}
	}
};

#endif // !BODY_PART_H
