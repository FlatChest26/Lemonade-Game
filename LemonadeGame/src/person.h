#pragma once

#ifndef LEMONADE_GAME_SRC_PERSON_H
#define LEMONADE_GAME_SRC_PERSON_H

#include "color.h"
#include "snowy_grammar.h"
#include "stat_values.h"
#include "currency.h"
#include "time_units.h"

class Entity;

class Person
{
protected:
	Entity* m_owner;
	Currency m_cash;

public:
	Person(Entity* owner, const Currency& starting_cash = Currency::none()) :
		m_owner(owner), m_cash(starting_cash)
	{
	}

public:
	// -- Variables -- //

	void set_owner(Entity* owner) { m_owner = owner; }
	Entity* get_owner() const { return m_owner; }

	// Cash //

	void set_cash(const Currency& cash) { m_cash = cash; }
	Currency& cash() { return m_cash; }
	const Currency& cash() const { return m_cash; }
};

#endif // !LEMONADE_GAME_SRC_PERSON_H
