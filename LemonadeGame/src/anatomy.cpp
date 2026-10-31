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

void Anatomy::set_owner(Entity* entity)
{
	m_owner = entity;
}

BodyPart* Anatomy::add_body_part(const BodyPartPlan& bp_plan)
{
	auto bp_type = RawMaster::get<BodyPartType>(bp_plan.type);

	if (!bp_type)
	{
		return nullptr;
	}

	m_body_parts.push_back(std::move(std::make_unique<BodyPart>(m_owner, bp_plan.ID, bp_type, bp_plan.rel_size, bp_plan.position, bp_plan.prefix)));

	if (!root)
	{
		root = m_body_parts.back().get();
	}

	return m_body_parts.back().get();
}

bool Anatomy::add_body_connection(const BodyConnectionPlan& bc_plan)
{
	BodyPart* parent = get_first_body_part_with_id(bc_plan.parent);
	BodyPart* child = get_first_body_part_with_id(bc_plan.child);

	if (!parent || !child)
	{
		CERR("One or both body parts not found: " << bc_plan.parent << ", " << bc_plan.child);
		return false;
	}

	m_connections.push_back(BodyConnection(parent, child));

	return true;
}

bool Anatomy::add_body_relation(const BodyConnectionPlan& bc_plan)
{
	BodyPart* parent = get_first_body_part_with_id(bc_plan.parent);
	BodyPart* child = get_first_body_part_with_id(bc_plan.child);

	if (!parent || !child)
	{
		CERR(" One or both body parts not found: " << bc_plan.parent << ", " << bc_plan.child);
		return false;
	}

	m_relations.push_back(BodyConnection(parent, child));

	return true;
}

bool Anatomy::remove_body_part(const BodyPartID& bp_ID)
{
	if (!has_body_part_with_id(bp_ID))
	{
		return false;
	}

	std::vector<size_t> remove;

	size_t idx = 0;

	for (auto& bp : m_body_parts)
	{
		if (bp->ID() == bp_ID)
		{
			remove.push_back(idx);
		}
		idx++;
	}

	for (auto remove_index : remove)
	{
		m_body_parts.erase(m_body_parts.begin() + remove_index);
	}

	return true;
}