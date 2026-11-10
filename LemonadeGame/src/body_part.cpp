#include "body_part.h"
#include "anatomy.h"
#include "entity.h"
#include "snowy_rng.h"

#include "tile_map.h"

BodyPart::BodyPart(Entity* owner, ThingID ID, RawMaster::Ref<BodyPartType> type, double rel_size, BodyPartPosition position, std::string prefix) :
	m_owner(owner), m_ID(ID), m_type(type), m_rel_size(rel_size), m_position(position), m_prefix(prefix)
{
	if (m_type)
	{
		m_health.max = type->max_health;
		m_health.current = type->max_health;
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
	if (m_owner && m_owner->get_anatomy())
	{
		return m_owner->get_anatomy();
	}
	return nullptr;
}

std::string BodyPart::prefix() const
{
	return m_prefix;
}

GrammarNoun BodyPart::display_noun() const
{ 
	return GrammarNoun::from_noun(m_prefix + m_type->noun, false);
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

	switch (damage.type)
	{
	case DamageType::EDGE:
	case DamageType::PIERCE:
	{
		if (get_owner()->has_movable())
		{
			Position pos = get_owner()->get_movable()->pos();

			auto splatter_positions = transforms::get_surrounding_locations(pos, false, false, true);
			std::shuffle(splatter_positions.begin(), splatter_positions.end(), snowy::rng::generator);

			get_owner()->get_tile_map()->add_material(splatter_positions.front(), RawMaster::get<MaterialType>("blood"), 1);
		}
		break;
	}
	}

	if (auto anatomy = get_anatomy())
	{
		anatomy->add_dirty_part(ID());
	}
}