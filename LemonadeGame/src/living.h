#pragma once

#ifndef LEMONADE_GAME_SRC_LIVING_H
#define LEMONADE_GAME_SRC_LIVING_H

#include "species.h"
#include "time_units.h"
#include "stat_values.h"
#include "anatomy.h"
#include "snowy_grammar.h"


#include <variant>

class Entity;
struct ConsumableEffect;

using PosNegValue = StatValue<-100, 100>;
using StatID = std::string;
struct StatDescription
{
	std::string description = "";
	StatInt_t min = -1;
	StatInt_t max = -1;
	color_t color = color::get("gray");
};

struct StatRate
{
	StatInt_t amount = 0;
	TimeUnit tick = TimeUnit::none();
};

enum class StatType
{
	Need,
	Trait,
	Attribute,
	Skill,
};

/* The moment to moment needs of a person. Constantly changing. */

struct StatInfo
{
	StatID ID;
	StatType type;

	std::string positive_name;
	std::string negative_name;
	
	StatInt_t min_value = 0;
	StatInt_t max_value = 100;

	std::vector<StatDescription> description;
};

class Stat
{
protected:
	StatInfo m_info;
	StatInt_t m_value{ 0 };

public:

	Stat(const StatInfo& info = {}, StatInt_t starting_value = 0) :
		m_info(info), m_value(0)
	{
		set_value(starting_value);
	}

	const StatInfo& info() const { return m_info; }

	StatInt_t get_value() const;

	StatInt_t get_max_value() const { return m_info.max_value; }
	StatInt_t get_min_value() const { return m_info.min_value; }

	double get_positive_percentage() const
	{
		return static_cast<double>(std::max(get_value(), 0)) / get_max_value();
	}

	double get_negative_percentage() const
	{
		return static_cast<double>(std::min(get_value(), 0)) / get_min_value();
	}

	void set_value(StatInt_t value);
	void add_value(StatInt_t value);

	std::string name() const;
	std::string description() const;
	color_t description_color() const;

	std::string to_string() const;
};

namespace needs
{
	inline StatID AROUSAL = "arousal";
	inline StatID HUNGER = "hunger";
	inline StatID THIRST = "thirst";
	inline StatID HYGIENE = "hygiene";
	inline StatID FATIGUE = "fatigue";
	inline StatID MOOD = "mood";
}

namespace attributes
{
	inline StatID STRENGTH = "strength";
	inline StatID AGILITY = "agility";
	inline StatID ENDURANCE = "endurance";
}

using TimerID = std::string;

struct StatTimer
{
	TimerID ID;

	StatID modified_stat;
	StatRate rate;
	TimeUnit timer;

	bool operator==(const StatTimer& other) const
	{
		return ID == other.ID;
	}

	bool operator!=(const StatTimer& other) const
	{
		return ID != other.ID;
	}
};

enum class AllureType : uint8_t
{
	NONE = 0,

	LEWD = 1 << 0,
	SUGGESTIVE = 1 << 1,
	KINKY = 1 << 2,
	BEAUTY = 1 << 3,
};

IS_FLAG_ENUM(AllureType);

enum class AttractionFeatureType : uint8_t
{
	GENDER_TYPE,
	BODY_PART_CATEGORY,
	NEED,
	TRAIT,
	SKILL,
	ATTRIBUTE,
};

using AttractedFeature = std::variant<GenderType, StatID, BodyPartCategory>;

struct Attraction
{
	AllureType allure_type;

	AttractionFeatureType feature_type;
	AttractedFeature attracted_feature;

	double base_amount{ 1.0 };
	std::function<double(const Attraction*, const Entity*)> modifier_callback;

	bool holds_attraction_for(const Entity* entity) const;
	double get_attraction_amount(const Entity* entity) const;
};

class Living
{
protected:
	Entity* m_owner{ nullptr };

	Age m_current_age{ 18 };
	Age m_total_lifespan{ 80 };

	std::unordered_map<StatID, Stat> m_stats;

	std::vector<StatID> m_needs;
	std::vector<StatID> m_traits;
	std::vector<StatID> m_attributes;
	std::vector<StatID> m_skills;

	std::vector<StatTimer> m_timers;
	std::unordered_map<TimerID, size_t> m_timers_by_id;

	std::vector<Attraction> m_attractions;

	bool m_alive{ true };
	bool m_awake{ true };

	int m_required_important_parts{ 0 };

public:
	Living(Entity* owner = nullptr, Age current_age = { 18 }, Age total_lifespan = { 80 });

	void set_owner(Entity* owner) { m_owner = owner; }
	Entity* get_owner() const { return m_owner; }

	void set_age(Age age) { m_current_age = age; }
	Age get_age() const { return m_current_age; }

	void set_lifespan(Age lifespan) { m_total_lifespan = lifespan; }
	Age get_lifespan() const { return m_total_lifespan; }

	bool is_young() const;
	bool is_mature() const;

	bool is_baby() const;
	bool is_child() const;
	bool is_adult() const;

	bool is_immortal()
	{
		return m_total_lifespan < static_cast<Age_t>(INFINITY);
	}

public:

	// -- Stats -- //

	void add_defaults();

	struct StatInitializer
	{
		StatID ID;
		StatInfo stat_info;
		StatRate change_rate{ StatRate{ 0, TimeUnit::none() } };
		StatInt_t starting_value{ 0 };
	};

	void add_stat(const StatInitializer& init);
	void remove_stat(const StatID& ID);

	bool has_stat(const StatID& ID) const
	{
		return m_stats.contains(ID);
	}

	Stat& get_stat(const StatID& ID)
	{
		if (!has_stat(ID))
		{
			CERR("Stat not found: " << ID);
		}

		return m_stats.at(ID);
	}

	const Stat& get_stat(const StatID& ID) const
	{
		if (!has_stat(ID))
		{
			CERR("Stat not found: " << ID);
		}

		return m_stats.at(ID);
	}

	bool has_need(const StatID& ID) const
	{
		if (!has_stat(ID))
			return false;

		return get_stat(ID).info().type == StatType::Need;
	}

	bool has_skill(const StatID& ID) const
	{
		if (!has_stat(ID))
			return false;

		return get_stat(ID).info().type == StatType::Skill;
	}

	bool has_attribute(const StatID& ID) const
	{
		if (!has_stat(ID))
			return false;

		return get_stat(ID).info().type == StatType::Attribute;
	}

	bool has_trait(const StatID& ID) const
	{
		if (!has_stat(ID))
			return false;

		return get_stat(ID).info().type == StatType::Trait;
	}

	const std::vector<StatID>& get_needs() const { return m_needs; }
	const std::vector<StatID>& get_traits() const { return m_traits; }
	const std::vector<StatID>& get_attributes() const { return m_attributes; }
	const std::vector<StatID>& get_skills() const { return m_skills; }

public:
	// -- Timers -- //

	bool has_timer(const TimerID& ID) const
	{
		return m_timers_by_id.contains(ID);
	}

	void add_timer(const TimerID& ID, const StatID& modified_stat, const StatRate& rate);
	void remove_timer(const TimerID& ID);

	StatTimer& get_timer(TimerID ID) { return m_timers.at(m_timers_by_id.at(ID)); }
	const StatTimer& get_timer(TimerID ID) const { return m_timers.at(m_timers_by_id.at(ID)); }

public:
	// -- Attractions -- //

	void add_attraction(const Attraction& attraction)
	{
		m_attractions.push_back(attraction);
	}

	double get_attractiveness(const Entity* other) const;


public:

	bool is_conscious() const { return is_alive() && is_awake(); }
	bool is_unconscious() const { return !is_conscious(); }

	// Sleep //

	void set_asleep(bool asleep)
	{
		m_awake = !asleep;
	}

	bool is_awake() const { return m_awake; }
	bool is_asleep() const { return !m_awake; }

	double get_sleep_quality() const;

	// Alive //

	bool is_alive() const { return m_alive; }
	bool is_dead() const { return !m_alive; }

	void set_alive(bool alive)
	{
		m_alive = alive;
	}

	bool can_sleep() const;
	bool can_wake_up() const;

	bool should_wake_up() const;
	bool should_sleep() const;

	// Utilities //

	bool can_act() const { return is_conscious(); }
	bool take_consumable(const ConsumableEffect& effect);


	// Actions //

	void die();
	void generic_death();
	void starve_to_death();
	void die_of_dehydration();

	void fall_asleep();
	void wake_up();

public:

	void step(const TimeUnit& time_delta);
};

#endif // !LEMONADE_GAME_SRC_LIVING_H