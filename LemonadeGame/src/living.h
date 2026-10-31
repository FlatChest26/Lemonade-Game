#pragma once

#ifndef LEMONADE_GAME_SRC_LIVING_H
#define LEMONADE_GAME_SRC_LIVING_H

#include "species.h"

class Entity;

class Living
{
protected:
	Entity* m_owner{ nullptr };

	Age m_current_age{ 18 };
	Age m_total_lifespan{ 80 };

public:
	Living(Entity* owner = nullptr, Age current_age = { 18 }, Age total_lifespan = { 80 }) :
		m_owner(owner), m_current_age(current_age), m_total_lifespan(total_lifespan)
	{
	}

	void set_owner(Entity* owner);
	Entity* get_owner() const;

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
		return m_total_lifespan < static_cast<Age::age_t>(INFINITY);
	}
};

#endif // !LEMONADE_GAME_SRC_LIVING_H