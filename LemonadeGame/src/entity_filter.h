#pragma once

#ifndef LEMONADE_GAME_SRC_ENTITY_FILTER_H
#define LEMONADE_GAME_SRC_ENTITY_FILTER_H

#include "entity.h"

struct EntityFilter
{
	bool ignore_held = true;
	std::vector<const Entity*> ignore_entities;
	EntityFlag ignore_flags = EntityFlag::NONE;
	std::vector<Position> ignore_positions;

	bool require_held = false;
	std::vector<const Entity*> required_entities;
	EntityFlag required_flags = EntityFlag::NONE;
	std::vector<Position> required_positions;

	bool has_required() const
	{
		if (require_held)
			return true;

		if (required_flags != EntityFlag::NONE)
			return true;

		if (!required_entities.empty())
			return true;

		if (!required_positions.empty())
			return true;

		return false;
	}

	bool is_required(const Entity* entity) const
	{
		if (!has_required())
			return true;

		if (require_held && !entity->is_being_held())
			return false;

		if (required_flags != EntityFlag::NONE && !entity->has_flags(required_flags))
			return false;

		if (!required_entities.empty() && !std::any_of(required_entities.begin(), required_entities.end(),
			[&entity](const Entity* other)
			{
				return other == entity;
			}
		))
		{
			return false;
		}

		if (!required_positions.empty() && !std::any_of(required_positions.begin(), required_positions.end(),
			[&entity](const Position& pos)
			{
				if (!entity->has_movable())
					return false;
				return entity->get_movable()->contains_point(pos);
			}
		))
		{
			return false;
		}

		return true;
	}

	bool has_ignore() const
	{
		if (ignore_held)
			return true;
			
		if (ignore_flags != EntityFlag::NONE)
			return true;

		if (!ignore_entities.empty())
			return true;

		if (!ignore_positions.empty())
			return true;

		return false;
	}

	bool should_ignore(const Entity* entity) const
	{
		if (!has_ignore())
			return false;

		if (ignore_held && entity->is_being_held())
			return true;

		if (ignore_flags != EntityFlag::NONE && entity->has_flags(ignore_flags))
			return true;

		if (
			!ignore_entities.empty() && std::any_of(ignore_entities.begin(), ignore_entities.end(), 
				[&entity](const Entity* other)
				{ 
					return other == entity; 
				}
			))
		{
			return true;
		}

		if (
			!ignore_positions.empty() && std::any_of(ignore_positions.begin(), ignore_positions.end(), 
				[&entity](const Position& pos) 
				{ 
					if (!entity->has_movable())
						return false;
					return entity->get_movable()->contains_point(pos);
				}
			))
		{
			return true;
		}

		return false;
	}
};

#endif // !LEMONADE_GAME_SRC_ENTITY_FILTER_H
