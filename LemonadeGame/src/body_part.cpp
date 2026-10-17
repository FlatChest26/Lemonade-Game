#include "body_part.h"
#include "anatomy.h"
#include "entity.h"

BodyPart::BodyPart(Entity* owner, ThingID ID, RawMaster::Ref<BodyPartType> type, double rel_size, BodyPartPosition position, std::string prefix) :
	m_owner(owner), m_ID(ID), m_type(type), m_rel_size(rel_size), m_position(position), m_prefix(prefix)
{
	if (is_lewd() || is_suggestive() || is_kinky() || is_pretty())
	{
		m_allure = std::make_unique<Allure>();
	}
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