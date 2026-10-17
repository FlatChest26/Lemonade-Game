#include "living.h"
#include "entity.h"

void Living::set_owner(Entity* owner)
{
	m_owner = owner;
}

Entity* Living::get_owner() const
{
	return m_owner;
}

bool Living::is_young() const
{
	if (auto species = get_owner()->get_species())
	{
		return get_age() < species->mature_age;
	}

	return false;
}

bool Living::is_mature() const
{
	if (auto species = get_owner()->get_species())
	{
		return get_age() >= species->mature_age;
	}

	return false;
}

bool Living::is_baby() const
{
	if (auto species = get_owner()->get_species())
	{
		return get_age() < species->child_age;
	}

	return false;
}

bool Living::is_child() const
{
	if (auto species = get_owner()->get_species())
	{
		return get_age() >= species->child_age && get_age() < species->adult_age;
	}

	return false;
}

bool Living::is_adult() const
{
	if (auto species = get_owner()->get_species())
	{
		return get_age() >= species->adult_age;
	}

	return false;
}
