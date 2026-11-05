#pragma once

#ifndef LEMONADE_GAME_SRC_ANATOMY_H
#define LEMONADE_GAME_SRC_ANATOMY_H

#include <algorithm>
#include <vector>

#include "body_part_type.h"
#include "body_part.h"
#include "body_plan.h"

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

	std::unordered_map<BodyPartID, BodyPart> m_body_parts{};

	std::vector<BodyConnection> m_connections{};
	std::vector<BodyConnection> m_relations{};

	BodyPartID m_root_id;
public:

	Anatomy(RawMaster::Ref<BodyPlan> bd_plan, std::vector<RawMaster::Ref<BodyMod>> bd_mods = {});

	Anatomy(const Anatomy& other);
	Anatomy(Anatomy&& other);

	void set_owner(Entity* entity);

	const BodyPartID& get_root_id() const { return m_root_id; }

	BodyPart* get_root_part()
	{
		return &m_body_parts.at(m_root_id);
	}

	const BodyPart* get_root_part() const
	{
		return &m_body_parts.at(m_root_id);
	}

	BodyPart& add_body_part(const BodyPartPlan& bp_plan);
	bool add_body_connection(const BodyConnectionPlan& bc_plan);
	bool add_body_relation(const BodyConnectionPlan& bc_plan);

	bool remove_body_part(const BodyPartID& bp_ID);


	// Body Part Utilities //

	bool has_body_part(const BodyPartID& bp_ID) const
	{
		return m_body_parts.contains(bp_ID);
	}

	BodyPart* get_body_part(const BodyPartID& bp_ID)
	{
		if (!has_body_part(bp_ID))
			return nullptr;

		return &m_body_parts.at(bp_ID);
	}

	std::vector<BodyPart*> get_body_parts(const BodyPartFilter& bp_filter = {})
	{
		std::vector<BodyPart*> body_parts;

		for (auto& [bp_id, bp] : m_body_parts)
		{
			if (!bp_filter.check(bp))
				continue;

			body_parts.push_back(&bp);
		}

		return body_parts;
	}

	std::vector<const BodyPart*> get_body_parts(const BodyPartFilter& bp_filter = {}) const
	{
		std::vector<const BodyPart*> body_parts;

		for (auto& [bp_id, bp] : m_body_parts)
		{
			if (!bp_filter.check(bp))
				continue;

			body_parts.push_back(&bp);
		}

		return body_parts;
	}

	bool has_body_parts(const BodyPartFilter& bp_filter = {}) const
	{
		for (auto& [bp_id, bp] : m_body_parts)
		{
			if (bp_filter.check(bp))
				return true;
		}

		return false;
	}

	int count_body_parts(const BodyPartFilter& bp_filter = {}) const
	{
		int count = 0;

		for (auto& [bp_id, bp] : m_body_parts)
		{
			if (bp_filter.check(bp))
				count++;
		}

		return count;
	}

};

#endif // !LEMONADE_GAME_SRC_ANATOMY_H