#pragma once

#ifndef LEMONADE_GAME_SRC_NEED_STATES_H
#define LEMONADE_GAME_SRC_NEED_STATES_H

#include "ai_state.h"
#include "entity.h"

class AIState_Horny : public AIState
{
protected:
	Entity* m_horny_target;
	std::function<bool()> m_still_valid_callback;
	int m_search_attempts{ 20 };

public:
	struct initializer
	{
		Entity* horny_target;
		UrgencyLevel urgency{ UrgencyLevel::NEUTRAL };
		std::function<bool()> still_valid_callback;

	};

	AIState_Horny(initializer init) :
		m_horny_target(init.horny_target), 
		m_still_valid_callback(init.still_valid_callback)
	{
		set_urgency(init.urgency);
	}

public:
	void set_horny_target(Entity* target) { m_horny_target = target; }

	virtual void init() override;
	virtual bool check() const override;
	virtual StateResult step(const TimeUnit& time_delta) override;
};

class AIState_FindConsumableEffect : public AIState
{
protected:
	Entity* m_consumable_target;
	StatID m_stat_id;
	std::function<bool()> m_still_valid_callback;

	int m_search_attempts{ 20 };

public:
	struct initializer
	{
		Entity* consumable_target;
		StatID stat_id;
		UrgencyLevel urgency{ UrgencyLevel::TENSE };
		std::function<bool()> still_valid_callback;
	};

	AIState_FindConsumableEffect(initializer init) :
		m_consumable_target(init.consumable_target), 
		m_stat_id(init.stat_id), 
		m_still_valid_callback(init.still_valid_callback)
	{
		set_urgency(init.urgency);
	}

public:
	void set_consumable_target(Entity* target) { m_consumable_target = target; }

	virtual void init() override;
	virtual bool check() const override;
	virtual StateResult step(const TimeUnit& time_delta) override;
};


class AIState_Kill : public AIState
{
protected:
	Entity* m_kill_target;
	Entity* m_weapon_target{ nullptr };
	int m_search_attempts{ 20 };
public:
	struct initializer
	{
		Entity* kill_target;
		UrgencyLevel urgency{ UrgencyLevel::LIFE_OR_DEATH };
	};

	AIState_Kill(initializer init) :
		m_kill_target(init.kill_target)
	{
		set_urgency(init.urgency);
	}

public:
	void set_kill_target(Entity* target) { m_kill_target = target; }
	void set_weapon_target(Entity* target) { m_weapon_target = target; }

	virtual void init() override;
	virtual bool check() const override;
	virtual StateResult step(const TimeUnit& time_delta) override;
};

#endif // !LEMONADE_GAME_SRC_NEED_STATES_H
