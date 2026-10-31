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
	BodyPart* parent;
	BodyPart* child;
	BodyPartRelativePosition relative_position;
};

class Entity;

class Anatomy
{
protected:
	Entity* m_owner{ nullptr };

	units::Length m_height{ AVERAGE_HEIGHT };
	units::Mass m_weight{ AVERAGE_WEIGHT };

	std::vector<std::unique_ptr<BodyPart>> m_body_parts{};
	std::vector<BodyConnection> m_connections{};
	std::vector<BodyConnection> m_relations{};

	BodyPart* root;

public:

	Anatomy(RawMaster::Ref<BodyPlan> bd_plan, std::vector<RawMaster::Ref<BodyMod>> bd_mods = {});

	void set_owner(Entity* entity);

	BodyPart* add_body_part(const BodyPartPlan& bp_plan);
	bool add_body_connection(const BodyConnectionPlan& bc_plan);
	bool add_body_relation(const BodyConnectionPlan& bc_plan);

	bool remove_body_part(const BodyPartID& bp_ID);

	int count_body_parts_with_category(const BodyPartCategory& category) const
	{
		return static_cast<int>(std::count_if(
			m_body_parts.begin(), m_body_parts.end(),
			[&](auto& bp)
			{
				return bp->category() == category;
			}
		));
	}

	int count_body_parts_with_id(const BodyPartID& bp_ID) const
	{
		return static_cast<int>(std::count_if(
			m_body_parts.begin(), m_body_parts.end(),
			[&](auto& bp)
			{
				return bp->ID() == bp_ID;
			}
		));
	}

	int count_body_parts_with_type(const BodyPartType* bp_type) const
	{
		return static_cast<int>(std::count_if(
			m_body_parts.begin(), m_body_parts.end(),
			[&](auto& bp)
			{
				return bp->get_type() == bp_type;
			}
		));
	}

	int count_body_parts_with_flag(const BodyPartFlag& flag) const
	{
		return static_cast<int>(std::count_if(
			m_body_parts.begin(), m_body_parts.end(),
			[&](auto& bp)
			{
				return bp->has_flag(flag);
			}
		));
	}

	bool has_body_part_with_category(const BodyPartCategory& category) const
	{
		return std::any_of(
			m_body_parts.begin(), m_body_parts.end(),
			[&](const auto& bp)
			{
				return bp->category() == category;
			}
		);
	}

	bool has_body_part_with_id(const BodyPartID& bp_ID) const
	{
		return std::any_of(
			m_body_parts.begin(), m_body_parts.end(),
			[&](const auto& bp)
			{
				return bp->ID() == bp_ID;
			}
		);
	}

	bool has_body_part_with_type(const BodyPartType* bp_type) const
	{
		return std::any_of(
			m_body_parts.begin(), m_body_parts.end(),
			[&](const auto& bp)
			{
				return bp->get_type() == bp_type;
			}
		);
	}

	bool has_body_part_with_flag(const BodyPartFlag& flag) const
	{
		return std::any_of(
			m_body_parts.begin(), m_body_parts.end(),
			[&](const auto& bp)
			{
				return bp->has_flag(flag);
			}
		);
	}

	BodyPart* get_first_body_part_with_category(const BodyPartCategory& category)
	{
		for (auto& bp : m_body_parts)
			if (bp->category() == category)
				return bp.get();

		return nullptr;
	}

	BodyPart* get_first_body_part_with_id(const BodyPartID& bp_ID)
	{
		for (auto& bp : m_body_parts)
			if (bp->ID() == bp_ID)
				return bp.get();

		return nullptr;
	}

	BodyPart* get_first_body_part_with_type(const BodyPartType* bp_type)
	{
		for (auto& bp : m_body_parts)
			if (bp->get_type() == bp_type)
				return bp.get();

		return nullptr;
	}

	BodyPart* get_first_body_part_with_flag(const BodyPartFlag& flag)
	{
		std::vector<BodyPart*> bps;
		for (auto& bp : m_body_parts)
			if (bp->has_flag(flag))
				return bp.get();

		return nullptr;
	}

	std::vector<BodyPart*> get_body_parts_with_id(const BodyPartID& bp_ID)
	{
		std::vector<BodyPart*> bps;

		for (auto& bp : m_body_parts)
			if (bp->ID() == bp_ID)
				bps.push_back(bp.get());

		return bps;
	}

	std::vector<BodyPart*> get_body_parts_with_type(const BodyPartType* bp_type)
	{
		std::vector<BodyPart*> bps;
		for (auto& bp : m_body_parts)
			if (bp->get_type() == bp_type)
				bps.push_back(bp.get());

		return bps;
	}

	std::vector<BodyPart*> get_body_parts_with_flag(const BodyPartFlag& flag)
	{
		std::vector<BodyPart*> bps;
		for (auto& bp : m_body_parts)
			if (bp->has_flag(flag))
				bps.push_back(bp.get());

		return bps;
	}
};

#endif // !LEMONADE_GAME_SRC_ANATOMY_H