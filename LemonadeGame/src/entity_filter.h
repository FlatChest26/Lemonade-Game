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
	bool ignore_movable = false;
	bool ignore_anatomy = false;

	bool require_held = false;
	std::vector<EntityID> required_ids;
	EntityFlag required_flags = EntityFlag::NONE;
	std::vector<Position> required_positions;
	bool must_be_ai = false;
	bool require_movable{ !required_positions.empty() };
	bool require_anatomy = false;

	bool has_required() const;
	bool is_required(const Entity& entity) const;

	bool is_required(const Entity* entity) const
	{
		return is_required(*entity);
	}

	bool has_ignore() const;
	bool should_ignore(const Entity& entity) const;

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
