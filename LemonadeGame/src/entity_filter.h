#pragma once

#ifndef LEMONADE_GAME_SRC_ENTITY_FILTER_H
#define LEMONADE_GAME_SRC_ENTITY_FILTER_H

#include "entity.h"

struct EntityFilter
{
	bool ignore_held = true;
	std::vector<EntityID> ignore_ids;
	EntityFlag ignore_flags = EntityFlag::NONE;
	std::vector<Position> ignore_positions;
	bool ignore_ai = false;

	bool require_held = false;
	std::vector<EntityID> required_ids;
	EntityFlag required_flags = EntityFlag::NONE;
	std::vector<Position> required_positions;
	bool must_be_ai = false;

	bool has_required() const
	{
		if (require_held)
			return true;

		if (required_flags != EntityFlag::NONE)
			return true;

		if (!required_ids.empty())
			return true;

		if (!required_positions.empty())
			return true;

		if (must_be_ai)
			return true;

		return false;
	}

	bool is_required(const Entity& entity) const
	{
		if (!has_required())
			return true;

		if (must_be_ai && !entity.has_brain())
			return false;

		if (require_held && !entity.is_being_held())
			return false;

		if (required_flags != EntityFlag::NONE && !entity.has_flags(required_flags))
			return false;

		if (!required_ids.empty() && !std::any_of(required_ids.begin(), required_ids.end(),
			[&entity](const EntityID& other)
			{
				return other == entity.ID();
			}
		))
		{
			return false;
		}

		if (!required_positions.empty() && !std::any_of(required_positions.begin(), required_positions.end(),
			[&entity](const Position& pos)
			{
				if (!entity.has_movable())
					return false;
				return entity.get_movable()->contains_point(pos);
			}
		))
		{
			return false;
		}

		return true;
	}

	bool is_required(const Entity* entity) const
	{
		return is_required(*entity);
	}

	bool has_ignore() const
	{
		if (ignore_held)
			return true;

		if (ignore_flags != EntityFlag::NONE)
			return true;

		if (!ignore_ids.empty())
			return true;

		if (!ignore_positions.empty())
			return true;

		if (ignore_ai)
			return true;

		return false;
	}

	bool should_ignore(const Entity& entity) const
	{
		if (!has_ignore())
			return false;

		if (ignore_ai && entity.has_brain())
			return true;

		if (ignore_held && entity.is_being_held())
			return true;

		if (ignore_flags != EntityFlag::NONE && entity.has_flags(ignore_flags))
			return true;

		if (
			!ignore_ids.empty() && std::any_of(ignore_ids.begin(), ignore_ids.end(),
				[&entity](const EntityID& other)
				{
					return other == entity.ID();
				}
			))
		{
			return true;
		}

		if (
			!ignore_positions.empty() && std::any_of(ignore_positions.begin(), ignore_positions.end(),
				[&entity](const Position& pos)
				{
					if (!entity.has_movable())
						return false;
					return entity.get_movable()->contains_point(pos);
				}
			))
		{
			return true;
		}

		return false;
	}

	bool should_ignore(const Entity* entity) const
	{
		return should_ignore(*entity);
	}


	bool check(const Entity& entity) const
	{
		if (!is_required(entity))
			return false;

		if (should_ignore(entity))
			return false;

		return true;
	}

	bool check(const Entity* entity) const
	{
		return check(*entity);
	}
};

#endif // !LEMONADE_GAME_SRC_ENTITY_FILTER_H
