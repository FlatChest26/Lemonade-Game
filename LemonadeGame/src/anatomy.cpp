#include "anatomy.h"
#include "units.h"
#include "entity.h"

#include <ranges>

Anatomy::Anatomy(RawMaster::Ref<BodyPlan> bd_plan, const std::vector<RawMaster::Ref<BodyMod>>& bd_mods)
{
	from_plan(bd_plan);
	from_mods(bd_mods);
}

Anatomy::Anatomy(const Anatomy& other)
{
	m_owner = other.m_owner;

	m_height = other.m_height;
	m_weight = other.m_weight;

	m_body_parts = other.m_body_parts;
	m_body_parts_by_id = other.m_body_parts_by_id;

	m_connections = other.m_connections;
	m_relations = other.m_relations;

	m_root_id = other.m_root_id;

	m_posture = other.m_posture;

	set_owner(other.m_owner);
}

Anatomy::Anatomy(Anatomy&& other)
{
	m_height = other.m_height;
	m_weight = other.m_weight;

	m_body_parts = std::move(other.m_body_parts);
	m_body_parts_by_id = std::move(other.m_body_parts_by_id);

	m_connections = std::move(other.m_connections);
	m_relations = std::move(other.m_relations);

	m_root_id = other.m_root_id;

	m_posture = other.m_posture;

	set_owner(other.m_owner);
}

void Anatomy::from_plan(RawMaster::Ref<BodyPlan> bd_plan)
{
	if (!bd_plan)
		return;

	m_height = bd_plan->height;
	m_weight = bd_plan->weight;

	for (auto& bp : bd_plan->body_parts)
	{
		add_body_part(bp);
	}

	m_posture = Posture(bd_plan->postures);

	for (auto& bc : bd_plan->body_connections)
	{
		add_body_connection(bc);
	}

	for (auto& bc : bd_plan->body_relations)
	{
		add_body_relation(bc);
	}
}

void Anatomy::from_mods(const std::vector<RawMaster::Ref<BodyMod>>& bd_mods)
{
	std::vector<BodyGloss> body_gloss;

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

		for (auto& bdg : bdm->add_body_gloss)
		{
			body_gloss.push_back(*bdg);
		}

		for (auto& bdg_id : bdm->remove_body_gloss)
		{
			std::vector<size_t> remove_indexes;

			size_t idx = 0;
			for (auto& bdg : body_gloss)
			{
				if (bdg.ID == bdg_id)
				{
					remove_indexes.push_back(idx);
				}
				idx++;
			}

			for (size_t remove_idx : std::views::reverse(remove_indexes))
			{
				if (remove_idx > body_gloss.size())
				{
					continue;
				}

				if (body_gloss[remove_idx] != body_gloss.back())
				{
					std::swap(body_gloss[remove_idx], body_gloss.back());
				}

				body_gloss.pop_back();
			}
		}
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

	for (auto& bdg : body_gloss)
	{
		apply_body_gloss(bdg);
	}
}

void Anatomy::set_owner(Entity* entity)
{
	m_owner = entity;

	for (auto& bp : m_body_parts)
	{
		bp.set_owner(entity);
	}
}

void Anatomy::add_dirty_part(const BodyPartID& bp_ID)
{
	if (!is_dirty_part(bp_ID))
	{
		m_already_added_dirty_parts.insert(bp_ID);
		m_dirty_parts.push_back(bp_ID);
	}
}

bool Anatomy::is_dirty_part(const BodyPartID& bp_ID) const
{
	return m_already_added_dirty_parts.contains(bp_ID);
}

BodyPart* Anatomy::add_body_part(const BodyPartPlan& bp_plan)
{
	auto bp_type = RawMaster::get<BodyPartType>(bp_plan.type);
	
	if (!bp_type)
	{
		CERR("Body part has no type! " << bp_plan.ID);
		return nullptr;
	}

	BodyPartID ID = bp_plan.ID;
	double rel_size = bp_plan.rel_size;

	if (rel_size < 0)
	{
		rel_size = bp_type->default_rel_size;
	}

	size_t idx = m_body_parts.size();

	m_body_parts.emplace_back(m_owner, ID, bp_type, rel_size, bp_plan.position, bp_plan.prefix);
	m_body_parts_by_id[ID] = idx;

	if (bp_plan.is_root)
	{
		m_root_id = ID;
	}

	add_dirty_part(ID);

	return &m_body_parts.back();
}

bool Anatomy::add_body_connection(const BodyConnectionPlan& bc_plan)
{
	if (has_connection(bc_plan.parent, bc_plan.child))
		return false;

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

void Anatomy::apply_body_gloss(const BodyGloss& bdg)
{
	for (const auto& replacement : bdg.replacements)
	{
		for (auto& bp : m_body_parts)
		{
			bp.set_noun(grammar::noun_replace(bp.noun(), replacement));
		}
	}
}

bool Anatomy::remove_body_part(const BodyPartID& bp_ID)
{
	if (!has_body_part(bp_ID))
	{
		return false;
	}

	if (is_root_part(bp_ID))
	{
		m_connections.clear();
		m_relations.clear();
		m_body_parts.clear();
		m_body_parts_by_id.clear();
	}
	else
	{
		auto body_part = get_body_part(bp_ID);

		for (auto& child_id : get_child_part_ids(bp_ID, false))
		{
			remove_body_part(child_id);
		}

		size_t idx = m_body_parts_by_id.at(bp_ID);
		m_body_parts_by_id.erase(bp_ID);

		if (m_body_parts[idx] != m_body_parts.back()) // Swap with back
		{
			BodyPartID swap_id = m_body_parts.back().ID();

			std::swap(m_body_parts[idx], m_body_parts.back());

			m_body_parts_by_id[swap_id] = idx;
		}

		m_body_parts.pop_back();
		
		fix_relations_and_connections();
	}

	return true;
}

void Anatomy::fix_relations_and_connections()
{
	std::vector<size_t> remove_connections;
	std::vector<size_t> remove_relations;

	size_t remove_idx = 0;
	for (auto& connection : m_connections)
	{
		if (!has_body_part(connection.parent) || !has_body_part(connection.child))
		{
			remove_connections.push_back(remove_idx);
		}

		remove_idx++;
	}

	remove_idx = 0;
	for (auto& relation : m_relations)
	{
		if (!has_body_part(relation.parent) || !has_body_part(relation.child))
		{
			remove_connections.push_back(remove_idx);
		}

		remove_idx++;
	}

	for (size_t idx : std::views::reverse(remove_connections))
	{
		if (idx >= m_connections.size())
			continue;

		m_connections.erase(m_connections.begin() + idx);
	}

	for (size_t idx : std::views::reverse(remove_relations))
	{
		if (idx >= m_connections.size())
			continue;

		m_relations.erase(m_relations.begin() + idx);
	}
}

bool Anatomy::can_stand() const
{
	return count_body_parts(BodyPartFilter{ .required_flags = BodyPartFlag::STANCE }) >= 2;
}

void Anatomy::step(const TimeUnit& time_delta)
{
	m_already_added_dirty_parts.clear();
	std::vector<BodyPartID> current_dirty_parts = std::move(m_dirty_parts);

	for (auto& dirty_id : current_dirty_parts)
	{
		bool still_dirty = false;
		auto bp = get_body_part(dirty_id);

		if (bp->health().current == 0)
		{
			auto n1 = get_owner()->display_noun();
			auto n2 = bp->display_noun();

			get_owner()->game_message(
				GameMessage(
					fmt::format(
						"{} {} {} destroyed!",
						grammar::Poss(std::string_view(grammar::the(n1))),
						grammar::to_string(n2),
						grammar::say_regarding(Verb{ "is", "are" }, n2)
					),
					color::get("red")
				)
			);

			remove_body_part(bp->ID());
		}

		if (still_dirty)
		{
			add_dirty_part(dirty_id);
		}
	}

	if (m_posture.is_standing() && !can_stand())
	{
		auto n1 = get_owner()->display_noun();

		get_owner()->game_message(
			GameMessage(
				fmt::format(
					"{} {} over.",
					grammar::The(n1),
					grammar::say_regarding(Verb{ "falls", "fall" }, n1)
				),
				color::get("light_red")
			)
		);

		m_posture.set_current_posture(m_posture.get_fallen_over_id());
		if (get_owner()->has_gait())
		{
			for (auto& gait : get_owner()->gait()->get_gaits())
			{
				if (!gait->is_default)
					continue;

				if (gait->posture_type == PostureType::CRAWL)
				{
					get_owner()->gait()->set_current_gait(gait->name);
					break;
				}
			}
		}
	}
}
