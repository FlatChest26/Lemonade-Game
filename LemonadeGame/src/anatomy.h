#pragma once

#ifndef LEMONADE_GAME_SRC_ANATOMY_H
#define LEMONADE_GAME_SRC_ANATOMY_H

#include <algorithm>
#include <vector>

#include "body_part_type.h"
#include "body_part.h"
#include "body_plan.h"
#include "posture.h"

#include "raw_master.h"

struct BodyConnection
{
	BodyPartID parent;
	BodyPartID child;
	BodyPartRelativePosition relative_position;
};

struct BodyPartFilter
{
	std::vector<BodyPartID> required_ids{};
	std::vector<BodyPartCategory> required_categories{ };
	std::vector<RawMaster::Ref<BodyPartType>> required_types{ };
	BodyPartFlag required_flags{ BodyPartFlag::NONE };

	std::vector<BodyPartID> ignore_ids{};
	std::vector<BodyPartCategory> ignore_categories{};
	std::vector<RawMaster::Ref<BodyPartType>> ignore_types{};
	BodyPartFlag ignore_flags{ BodyPartFlag::NONE };

	bool check(const BodyPart& bp) const
	{
		auto id_check = [&](const BodyPartID& bp_id) { return bp.ID() == bp_id; };

		if (!required_ids.empty() && !std::any_of(required_ids.begin(), required_ids.end(), id_check))
			return false;

		if (!ignore_ids.empty() && std::any_of(ignore_ids.begin(), ignore_ids.end(), id_check))
			return false;


		auto category_check = [&](const BodyPartCategory& bp_cat) { return bp.category() == bp_cat; };

		if (!required_categories.empty() && !std::any_of(required_categories.begin(), required_categories.end(), category_check))
			return false;

		if (!ignore_categories.empty() && std::any_of(ignore_categories.begin(), ignore_categories.end(), category_check))
			return false;

		auto type_check = [&](RawMaster::Ref<BodyPartType> bp_type) { return bp.get_type()->ID == bp_type->ID; };

		if (!required_types.empty() && !std::any_of(required_types.begin(), required_types.end(), type_check))
			return false;

		if (!ignore_types.empty() && std::any_of(ignore_types.begin(), ignore_types.end(), type_check))
			return false;

		if (required_flags != BodyPartFlag::NONE && !(bp.flags() & required_flags))
			return false;

		if (ignore_flags != BodyPartFlag::NONE && (bp.flags() & ignore_flags))
			return false;

		return true;
	}
};

class Entity;

class Anatomy
{
protected:
	Entity* m_owner{ nullptr };

	units::Length m_height{ AVERAGE_HEIGHT };
	units::Mass m_weight{ AVERAGE_WEIGHT };

	std::vector<BodyPart> m_body_parts{};
	std::unordered_map<BodyPartID, size_t> m_body_parts_by_id{};

	std::vector<BodyConnection> m_connections{};
	std::vector<BodyConnection> m_relations{};

	BodyPartID m_root_id;

	std::set<BodyPartID> m_already_added_dirty_parts;
	std::vector<BodyPartID> m_dirty_parts;

	Posture m_posture;

public:

	Anatomy(RawMaster::Ref<BodyPlan> bd_plan, const std::vector<RawMaster::Ref<BodyMod>>& bd_mods = {});

	Anatomy(const Anatomy& other);
	Anatomy(Anatomy&& other);

	void from_plan(RawMaster::Ref<BodyPlan> bd_plan);
	void from_plan(const BodyPlan* bd_plan) { from_plan(RawMaster::Ref<BodyPlan>(bd_plan)); }

	void from_mods(const std::vector<RawMaster::Ref<BodyMod>>& bd_mods);

	void set_owner(Entity* owner);
	Entity* get_owner() const { return m_owner; }

	const BodyPartID& get_root_id() const { return m_root_id; }
	BodyPart* get_root_part() { return get_body_part(m_root_id); }
	const BodyPart* get_root_part() const { return get_body_part(m_root_id); }
	bool is_root_part(const BodyPartID& bp_ID) const { return m_root_id == bp_ID; }

	Posture& posture() { return m_posture; }
	const Posture& posture() const { return m_posture; }

	BodyPart* add_body_part(const BodyPartPlan& bp_plan);
	bool add_body_connection(const BodyConnectionPlan& bc_plan);
	bool add_body_relation(const BodyConnectionPlan& bc_plan);

	bool remove_body_part(const BodyPartID& bp_ID);
	void fix_relations_and_connections();

	void add_dirty_part(const BodyPartID& bp_ID);
	bool is_dirty_part(const BodyPartID& bp_ID) const;

	void apply_body_gloss(const BodyGloss& bdg);
	void apply_body_gloss(BodyGloss* bdg) { apply_body_gloss(*bdg); }

	bool can_stand() const;

	// Body Part Utilities //

	bool has_body_part(const BodyPartID& bp_ID) const
	{
		return m_body_parts_by_id.contains(bp_ID);
	}

	BodyPart* get_body_part(const BodyPartID& bp_ID)
	{
		if (!has_body_part(bp_ID))
			return nullptr;

		return &m_body_parts.at(m_body_parts_by_id.at(bp_ID));
	}

	const BodyPart* get_body_part(const BodyPartID& bp_ID) const
	{
		if (!has_body_part(bp_ID))
			return nullptr;

		return &m_body_parts.at(m_body_parts_by_id.at(bp_ID));
	}

	std::vector<BodyPart*> get_body_parts()
	{
		std::vector<BodyPart*> body_parts;

		for (auto& bp : m_body_parts)
		{
			body_parts.push_back(&bp);
		}

		return body_parts;
	}

	std::vector<BodyPart*> get_body_parts(const BodyPartFilter& bp_filter)
	{
		std::vector<BodyPart*> body_parts;

		for (auto& bp : m_body_parts)
		{
			if (!bp_filter.check(bp))
				continue;

			body_parts.push_back(&bp);
		}

		return body_parts;
	}

	std::vector<const BodyPart*> get_body_parts() const
	{
		std::vector<const BodyPart*> body_parts;

		for (auto& bp : m_body_parts)
		{
			body_parts.push_back(&bp);
		}

		return body_parts;
	}

	std::vector<const BodyPart*> get_body_parts(const BodyPartFilter& bp_filter = {}) const
	{
		std::vector<const BodyPart*> body_parts;

		for (auto& bp : m_body_parts)
		{
			if (!bp_filter.check(bp))
				continue;

			body_parts.push_back(&bp);
		}

		return body_parts;
	}

	std::vector<BodyPartID> get_body_part_ids() const
	{
		std::vector<BodyPartID> body_parts;

		for (auto& bp : m_body_parts)
		{
			body_parts.push_back(bp.ID());
		}

		return body_parts;
	}

	std::vector<BodyPartID> get_body_part_ids(const BodyPartFilter& bp_filter) const
	{
		std::vector<BodyPartID> body_parts;

		for (auto& bp : m_body_parts)
		{
			if (!bp_filter.check(bp))
				continue;

			body_parts.push_back(bp.ID());
		}

		return body_parts;
	}

	bool has_body_parts() const
	{
		return !m_body_parts.empty();
	}

	bool has_body_parts(const BodyPartFilter& bp_filter) const
	{
		return std::any_of(
			m_body_parts.begin(), m_body_parts.end(),
			[&bp_filter](const auto& bp)
			{
				return bp_filter.check(bp);
			}
		);
	}

	int count_body_parts() const
	{
		return static_cast<int>(m_body_parts.size());
	}

	int count_body_parts(const BodyPartFilter& bp_filter) const
	{
		return static_cast<int>(std::count_if(
			m_body_parts.begin(), m_body_parts.end(),
			[&bp_filter](const auto& bp)
			{
				return bp_filter.check(bp);
			}
		));
	}

	bool has_connection(const BodyPartID& parent, const BodyPartID& child) const
	{
		if (!has_body_part(parent) || !has_body_part(child))
			return false;

		return std::any_of(
			m_connections.begin(), m_connections.end(),
			[&](const auto& connection)
			{
				if (!is_valid_connection(connection))
					return false;

				if (connection.parent == parent && connection.child == child)
					return true;

				return false;
			}
		);;
	}

	bool is_valid_connection(const BodyConnection& connection) const
	{
		return has_body_part(connection.parent) && has_body_part(connection.child);
	}

	BodyConnection get_connection(const BodyPartID& parent, const BodyPartID& child) const
	{
		for (const auto& connection : m_connections)
		{
			if (!is_valid_connection(connection))
				continue;

			if (connection.parent == parent && connection.child == child)
				return connection;
		}

		return BodyConnection();
	}

	bool has_parent_part(const BodyPartID& bp_ID) const
	{
		if (!has_body_part(bp_ID))
			return false;

		for (const auto& connection : m_connections)
		{
			if (!is_valid_connection(connection))
				continue;

			if (connection.child == bp_ID)
				return true;
		}

		return false;
	}

	bool has_child_parts(const BodyPartID& bp_ID) const
	{
		if (!has_body_part(bp_ID))
			return false;

		for (const auto& connection : m_connections)
		{
			if (!is_valid_connection(connection))
				continue;

			if (connection.parent == bp_ID)
				return true;
		}

		return false;
	}

	BodyPartID get_parent_part_id(const BodyPartID& bp_ID) const
	{
		if (!has_body_part(bp_ID))
			return {};

		for (const auto& connection : m_connections)
		{
			if (!is_valid_connection(connection))
				continue;

			if (connection.child == bp_ID)
				return connection.parent;
		}

		return {};
	}

	std::vector<BodyPartID> get_child_part_ids(const BodyPartID& bp_ID, bool recursive = false) const
	{
		if (!has_body_part(bp_ID))
			return {};

		std::vector<BodyPartID> child_parts;

		for (const auto& connection : m_connections)
		{
			if (!is_valid_connection(connection))
				continue;

			if (connection.parent == bp_ID)
			{
				child_parts.push_back(connection.child);
			}
		}

		if (recursive && !child_parts.empty())
		{
			for (const auto& child_id : child_parts)
			{
				auto child_children = get_child_part_ids(child_id, recursive);

				if (!child_children.empty())
				{
					child_parts.insert(child_parts.end(), child_children.begin(), child_children.end());
				}
			}
		}

		return child_parts;
	}

	BodyPart* get_parent_part(const BodyPartID& bp_ID)
	{
		if (!has_body_part(bp_ID))
			return nullptr;

		BodyPartID parent_id = get_parent_part_id(bp_ID);

		if (parent_id.empty())
			return nullptr;

		return get_body_part(parent_id);
	}

	const BodyPart* get_parent_part(const BodyPartID& bp_ID) const
	{
		if (!has_body_part(bp_ID))
			return nullptr;

		BodyPartID parent_id = get_parent_part_id(bp_ID);

		if (parent_id.empty())
			return nullptr;

		return get_body_part(parent_id);
	}

	std::vector<BodyPart*> get_child_parts(const BodyPartID& bp_ID, bool recursive = false)
	{
		if (!has_body_part(bp_ID))
			return {};

		std::vector<BodyPartID> child_ids = get_child_part_ids(bp_ID, recursive);

		if (child_ids.empty())
			return {};

		return get_body_parts(BodyPartFilter{ .required_ids = child_ids });
	}

	std::vector<const BodyPart*> get_child_parts(const BodyPartID& bp_ID, bool recursive = false) const
	{
		if (!has_body_part(bp_ID))
			return {};

		std::vector<BodyPartID> child_ids = get_child_part_ids(bp_ID, recursive);

		if (child_ids.empty())
			return {};

		return get_body_parts(BodyPartFilter{ .required_ids = child_ids });
	}

	
public:
	void step(const TimeUnit& time_delta);

};

#endif // !LEMONADE_GAME_SRC_ANATOMY_H