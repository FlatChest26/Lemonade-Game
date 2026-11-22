#include "entity_filter.h"

bool EntityFilter::has_required() const
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

	if (require_movable)
		return true;

	if (require_anatomy)
		return true;

	return false;
}

bool EntityFilter::is_required(const Entity& entity) const
{
	if (!has_required())
		return true;

	if (must_be_ai && !entity.has_brain())
		return false;

	if (require_movable && !entity.has_movable())
		return false;

	if (require_anatomy && !entity.has_anatomy())
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
			return entity.movable()->contains_point(pos);
		}
	))
	{
		return false;
	}

	return true;
}

bool EntityFilter::has_ignore() const
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

	if (ignore_movable)
		return true;

	if (ignore_anatomy)
		return true;

	return false;
}

bool EntityFilter::should_ignore(const Entity& entity) const
{
	if (!has_ignore())
		return false;

	if (ignore_ai && entity.has_brain())
		return true;

	if (ignore_movable && entity.has_movable())
		return true;

	if (ignore_anatomy && entity.has_anatomy())
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
				return entity.movable()->contains_point(pos);
			}
		))
	{
		return true;
	}

	return false;
}