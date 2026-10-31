#pragma once

#ifndef LEMONADE_GAME_SRC_PERSON_H
#define LEMONADE_GAME_SRC_PERSON_H

#include "color.h"
#include "snowy_grammar.h"
#include "stat_values.h"
#include "currency.h"
#include "time_units.h"

class Entity;

using MultiValue = StatValue<-100, 100>;
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
	StatType type;
	std::string positive_name;
	std::string negative_name;
	bool can_be_negative = false;

	std::vector<StatDescription> description;
};

class Stat
{
protected:
	StatInfo m_info;

	MultiValue m_value;

public:

	Stat(const StatInfo& info = {}) :
		m_info(info)
	{
	}

	const StatInfo& info() const { return m_info; }

	MultiValue& value() { return m_value; }
	const MultiValue& value() const { return m_value; }
	StatInt_t get_value() const;

	std::string name() const;
	std::string description() const;
	color_t description_color() const;

	std::string to_string() const;
};

namespace needs
{
	inline StatID AROUSAL = "AROUSAL";
	inline StatID HUNGER = "HUNGER";
	inline StatID THIRST = "THIRST";
	inline StatID HYGIENE = "HYGIENE";
	inline StatID FATIGUE = "FATIGUE";
	inline StatID MOOD = "MOOD";
}

using TimerID = std::string;
using StatTimer = std::tuple<StatRate, TimeUnit>;

class Person
{
protected:
	Entity* m_owner;
	Currency m_cash;

	std::unordered_map<StatID, Stat> m_stats;

	std::vector<StatID> m_needs;
	std::vector<StatID> m_traits;
	std::vector<StatID> m_attributes;
	std::vector<StatID> m_skills;

	std::unordered_map<TimerID, StatTimer> m_timers;

	bool m_asleep = false;
	bool m_alive = true;

public:
	Person(Entity* owner, const Currency& starting_cash = Currency::none()) :
		m_owner(owner), m_cash(starting_cash)
	{
	}

public:
	void add_defaults();

	void add_stat(const StatID& ID, const StatInfo& stat, const StatRate& change_rate = StatRate{ 0, TimeUnit::none() });
	void remove_stat(const StatID& ID);

	bool has_stat(const StatID& ID) const
	{
		return m_stats.find(ID) != m_stats.end();
	}

	Stat& stat(const StatID& ID)
	{
		if (!has_stat(ID))
		{
			CERR("Stat not found: " << ID);
		}

		return m_stats.at(ID);
	}

	const Stat& stat(const StatID& ID) const
	{
		if (!has_stat(ID))
		{
			CERR("Stat not found: " << ID);
		}

		return m_stats.at(ID);
	}

	std::vector<StatID> get_needs() const { return m_needs; }
	std::vector<StatID> get_traits() const { return m_traits; }
	std::vector<StatID> get_attributes() const { return m_attributes; }
	std::vector<StatID> get_skills() const { return m_skills; }

public:
	// -- Timers -- //

	bool has_timer(const TimerID& ID) const
	{
		return m_timers.find(ID) != m_timers.end();
	}

	void add_timer(const TimerID& ID, const StatRate& rate)
	{
		m_timers[ID] = StatTimer(rate, TimeUnit::none());
	}

	void remove_timer(const TimerID& ID)
	{
		m_timers.erase(ID);
	}

	StatTimer& get_timer(TimerID ID) { return m_timers.at(ID); }
	const StatTimer& get_timer(TimerID ID) const { return m_timers.at(ID); }

public:

	// -- Variables -- //

	void set_owner(Entity* owner) { m_owner = owner; }
	Entity* get_owner() const { return m_owner; }

	// Cash //

	void set_cash(const Currency& cash) { m_cash = cash; }
	Currency& cash() { return m_cash; }
	const Currency& cash() const { return m_cash; }

	// Sleep //

	bool is_asleep() const { return m_asleep; }
	bool is_awake() const { return !m_asleep; }

	void fall_asleep();
	void wake_up();

	double get_sleep_quality() const;

	// Alive //

	bool is_alive() const { return m_alive; }
	bool is_dead() const { return !m_alive; }
	void set_alive(bool alive) { m_alive = alive; }

	bool can_sleep() const;
	bool can_wake_up() const;

	bool should_wake_up() const;
	bool should_sleep() const;

public:

	void step(const TimeUnit& time_delta);
};

#endif // !LEMONADE_GAME_SRC_PERSON_H
