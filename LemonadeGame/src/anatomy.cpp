#include "anatomy.h"
#include "units.h"

Anatomy::Anatomy(RawMaster::Ref<BodyPlan> bd_plan, std::vector<RawMaster::Ref<BodyMod>> bd_mods) :
	m_height(bd_plan->height), m_weight(bd_plan->weight)
{
	for (auto& bp : bd_plan->body_parts)
	{
		add_body_part(bp);
	}

	for (auto& bdm : bd_mods)
	{
		m_height *= bdm->multiply_height;
		m_height *= bdm->modify_size;
		m_height += bdm->add_height;

		if (bdm->set_height > 0_ft)
		{
			m_height = bdm->set_height;
		}

		m_weight *= bdm->multiply_weight;
		m_weight *= bdm->modify_size;
		m_weight += bdm->add_weight;

		if (bdm->set_weight > 0_lb)
		{
			m_weight = bdm->set_weight;
		}

		for (auto& bp : bdm->add_parts)
		{
			add_body_part(bp);
		}

		for (auto& bp_id : bdm->remove_parts)
		{
			remove_body_part(bp_id);
		}

		for (auto& [bp_id, bp] : bdm->replace_parts)
		{
			remove_body_part(bp_id);
			add_body_part(bp);
		}
	}

	for (auto& bc : bd_plan->body_connections)
	{
		add_body_connection(bc);
	}

	for (auto& bc : bd_plan->body_relations)
	{
		add_body_relation(bc);
	}

	for (auto& bdm : bd_mods)
	{
		for (auto& bc : bdm->add_connections)
		{
			add_body_connection(bc);
		}

		for (auto& bc : bdm->add_relations)
		{
			add_body_relation(bc);
		}
	}
}

Anatomy::Anatomy(const Anatomy& other)
{
	m_owner = other.m_owner;

	m_height = other.m_height;
	m_weight = other.m_weight;

	m_body_parts = other.m_body_parts;

	m_connections = other.m_connections;
	m_relations = other.m_relations;

	m_root_id = other.m_root_id;
}

Anatomy::Anatomy(Anatomy&& other)
{

}

void Anatomy::set_owner(Entity* entity)
{
	m_owner = entity;

	for (auto& [bp_id, bp] : m_body_parts)
	{
		bp.set_owner(entity);
	}
}

BodyPart& Anatomy::add_body_part(const BodyPartPlan& bp_plan)
{
	auto bp_type = RawMaster::get<BodyPartType>(bp_plan.type);
	
	if (!bp_type)
	{
		CERR("Body part has no type! " << bp_plan.ID);
	}

	BodyPartID ID = bp_plan.ID;
	double rel_size = bp_plan.rel_size;

	if (rel_size < 0)
	{
		rel_size = bp_type->default_rel_size;
	}

	m_body_parts.insert(std::pair(ID, BodyPart(m_owner, ID, bp_type, rel_size, bp_plan.position, bp_plan.prefix)));

	if (m_root_id.empty())
	{
		m_root_id = ID;
	}

	//COUT("added body part: " << ID);

	return m_body_parts[ID];
}

bool Anatomy::add_body_connection(const BodyConnectionPlan& bc_plan)
{
	if (!has_body_part(bc_plan.parent) || !has_body_part(bc_plan.child))
	{
		CERR(" One or both body parts not found: " << bc_plan.parent << ", " << bc_plan.child);
		return false;
	}

	m_connections.push_back(BodyConnection(bc_plan.parent, bc_plan.child));

	return true;
}

bool Anatomy::add_body_relation(const BodyConnectionPlan& bc_plan)
{
	if (!has_body_part(bc_plan.parent) || !has_body_part(bc_plan.child))
	{
		CERR(" One or both body parts not found: " << bc_plan.parent << ", " << bc_plan.child);
		return false;
	}

	m_relations.push_back(BodyConnection(bc_plan.parent, bc_plan.child));

	return true;
}

bool Anatomy::remove_body_part(const BodyPartID& bp_ID)
{
	if (!has_body_part(bp_ID))
	{
		return false;
	}

	m_body_parts.erase(bp_ID);

	return true;
}