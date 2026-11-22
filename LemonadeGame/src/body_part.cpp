#include "body_part.h"
#include "anatomy.h"
#include "entity.h"
#include "snowy_rng.h"

#include "tile_map.h"

BodyPart::BodyPart(Entity* owner, BodyPartID ID, RawMaster::Ref<BodyPartType> type, double rel_size, BodyPartPosition position, std::string prefix) :
	m_owner(owner), m_ID(ID), m_type(type), m_rel_size(rel_size), m_position(position), m_prefix(prefix)
{
	if (m_type)
	{
		m_health.max = m_type->max_health;
		m_health.current = m_type->max_health;

		m_noun = m_type->noun;
	}
}

void BodyPart::set_owner(Entity* entity)
{
	m_owner = entity;
}

Entity* BodyPart::get_owner() const
{
	return m_owner;
}

Anatomy* BodyPart::get_anatomy() const
{
	if (m_owner && m_owner->anatomy())
	{
		return m_owner->anatomy();
	}
	return nullptr;
}

std::string BodyPart::prefix() const
{
	return m_prefix;
}

GrammarNoun BodyPart::display_noun() const
{ 
	return GrammarNoun::from_noun(m_prefix + m_noun, false);
}

void BodyPart::take_damage(const Damage& damage)
{
	if (damage.amount > m_health.current)
	{
		m_health.current = 0;
	}
	else
	{
		m_health.current -= damage.amount;
	}

	

	if (auto anatomy = get_anatomy())
	{
		anatomy->add_dirty_part(ID());
	}
}