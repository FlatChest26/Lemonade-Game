#include "creature_soul.h"
#include "entity.h"

CreatureSoul::CreatureSoul(const initializer& init) :
	m_ID(std::move(init.ID)),
	m_description(std::move(init.description)),
	m_name(std::move(init.name)),
	m_gender(std::move(init.gender)),
	m_color(std::move(init.color))
{
}

void CreatureSoul::set_physical_form(Entity* form)
{
	m_physical_form = form;
}

Entity* CreatureSoul::get_physical_form() const
{
	return m_physical_form;
}

bool CreatureSoul::has_physical_form() const
{
	return get_physical_form() != nullptr;
}


/*
bool CreatureSoul::can_be_attracted_to(CreatureSoul* other) const
{
	auto has_attraction = [&](SexualAttraction attraction)
		{
			return m_sexuality.attraction & attraction;
		};

	if (has_attraction(SexualAttraction::ANY))
	{
		return true;
	}
	if (has_attraction(SexualAttraction::NONE))
	{
		return false;
	}

	
	if( has_attraction( SexualAttraction::OPPOSITE ) )
	{
		return ( is_male() && other->is_female() ) || ( is_female() && other->is_male() );
	}
	if( has_attraction( SexualAttraction::SAME ) )
	{
		return ( is_male() && other->is_male() ) || ( is_female() && other->is_female() );
	}
	if( has_attraction( SexualAttraction::MALE ) )
	{
		return other->is_male();
	}
	if( has_attraction( SexualAttraction::FEMALE ) )
	{
		return other->is_female();
	}
	

	if (has_attraction(SexualAttraction::NEUTER))
	{
		return other->is_neuter();
	}
	if (has_attraction(SexualAttraction::EPICENE))
	{
		return other->is_epicene();
	}
	if (has_attraction(SexualAttraction::MASCULINE))
	{
		return other->is_masculine();
	}
	if (has_attraction(SexualAttraction::FEMININE))
	{
		return other->is_feminine();
	}

	return false;
}
*/

const Species* CreatureSoul::get_species() const
{
	if (auto entity = get_physical_form())
		return entity->get_species();

	return nullptr;
}

Noun CreatureSoul::species_noun() const
{
	if (get_species())
		return get_species()->noun;

	return Noun();
}