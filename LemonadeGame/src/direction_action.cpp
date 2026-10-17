#include "direction_action.h"
#include "location_actions.h"
#include "misc_actions.h"

#include "world.h"
#include "entity.h"
#include "snowy_macros.h"
#include "time_units.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>

ActionWithDirection::ActionWithDirection(const TimeUnit& action_length, Entity* actor, const Coord& dx, const Coord& dy, const Coord& dz) :
	Action(action_length, actor), m_dx(dx), m_dy(dy), m_dz(dz)
{
}

BumpAction::BumpAction(Entity* actor, const Coord& dx, const Coord& dy, const Coord& dz) :
	ActionWithDirection(TimeUnit::none(), actor, dx, dy, dz)
{
}

MovementAction::MovementAction(const TimeUnit& action_length, Entity* entity, const Coord& dx, const Coord& dy, const Coord& dz) :
	ActionWithDirection(action_length, entity, dx, dy, dz)
{
}

ActionResult BumpAction::perform()
{
	auto world = get_world();

	if (!world)
	{
		CERR("No world found");
		return ActionResult(false);
	}

	if (world->in_bounds(destination()))
	{
		auto dest = destination();

		if (world->get_tile(dest)->can_open())
		{
			auto open_action = std::make_unique<OpenAction>(m_actor->speed(), get_actor(), dest);
			auto result = ActionResult{ true, "" };
			result.add_next_action(std::move(open_action));
			return result;
		}
	}

	if (auto world = get_world())
	{
		auto blocking = world->get_blocking_entities_at(destination());
		for (auto& entity : blocking)
		{
			if (entity != get_actor() && entity->is_animate() && !entity->fixed_in_place())
			{
				auto swap_action = std::make_unique<SwapPlacesAction>(m_actor->speed(), get_actor(), entity);
				auto result = ActionResult{ true, "" };
				result.add_next_action(std::move(swap_action));
				return result;
			}
		}
	}

	auto movement_action = std::make_unique<MovementAction>(m_actor->speed(), m_actor, m_dx, m_dy, m_dz);
	auto result = ActionResult{ true, "" };
	result.add_next_action(std::move(movement_action));

	return result;
}

ActionResult MovementAction::perform()
{
	if (!m_actor->has_movable())
	{
		CERR("Entity has no movable!");
		return ActionResult(false);
	}

	auto world = get_world();

	if (!world)
	{
		CERR("No world found!");
		return ActionResult(false);
	}

	if (get_action_length() > TimeUnit::from_seconds(0.0f) && m_actor->can_fall() && !m_actor->is_on_floor())
	{
		return ActionResult(false);
	}

	Coord start_x = m_actor->get_movable()->posx();
	Coord start_y = m_actor->get_movable()->posy();
	Coord start_z = m_actor->get_movable()->posz();

	Coord final_x = start_x + m_dx;
	Coord final_y = start_y + m_dy;
	Coord final_z = start_z + m_dz;

	if (m_actor->is_map_bound())
	{
		if (auto tile_map = m_actor->get_tile_map())
		{
			Coord bounds_x = tile_map->get_width();
			Coord bounds_y = tile_map->get_height();
			Coord bounds_z = tile_map->get_depth();

			final_x = std::max(0, std::min(final_x, bounds_x - 1));
			final_y = std::max(0, std::min(final_y, bounds_y - 1));
			final_z = std::max(0, std::min(final_z, bounds_z - 1));
		}
	}

	if (m_actor->can_collide())
	{
		bool has_collision = false;

		if (world->is_blocked(final_x, final_y, final_z, m_actor, m_dz))
		{
			has_collision = true;
		}

		// Attempt to slide along wall
		if (has_collision)
		{
			if (abs(m_dx) > 0 && world->is_blocked(final_x, start_y, start_z, m_actor, m_dz))
			{
				final_x = start_x;
			}

			if (abs(m_dy) > 0 && world->is_blocked(start_x, final_y, start_z, m_actor, m_dz))
			{
				final_y = start_y;
			}

			if (abs(m_dz) > 0 && world->is_blocked(start_x, start_y, final_z, m_actor, m_dz))
			{
				final_z = start_z;
			}
		}

		if (world->is_blocked(final_x, final_y, final_z, m_actor, m_dz))
		{
			return ActionResult(false);
		}
	}

	if (m_dz > 0 && m_actor->can_fall())
	{
		if (!get_tile_map()->get_tile(start_x, start_y, start_z)->can_ascend())
		{
			return ActionResult(false);
		}
	}

	m_actor->get_movable()->set_position(Position(final_x, final_y, final_z));

	if (start_x == final_x && start_y == final_y && start_z == final_z)
	{
		return ActionResult(false);
	}

	return ActionResult(true);
}