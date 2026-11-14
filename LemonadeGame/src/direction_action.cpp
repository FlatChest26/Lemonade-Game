#include "direction_action.h"
#include "location_actions.h"
#include "interactions.h"

#include "world.h"
#include "entity.h"
#include "snowy_macros.h"
#include "time_units.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>

bool ActionWithDirection::check_action() const
{
	if (!EntityAction::check_action())
		return false;

	return true;
}

bool BumpAction::check_action() const
{
	if (!ActionWithDirection::check_action())
		return false;

	if (!get_actor()->has_movable())
	{
		ACTION_CERR("Actor has no movable! " << m_actor->ID());
		return false;
	}

	if (!get_world())
	{
		ACTION_CERR("No world found!");
		return false;
	}

	return true;
}

ActionResult BumpAction::perform()
{
	auto actor = get_actor();
	auto world = get_world();

	auto dest_tile = world->get_tile(destination());

	if (dest_tile && dest_tile->can_open())
	{
		return ActionResult
		{
			.success = true,
			.next = new OpenAction(actor, destination())
		};
	}

	if (actor->can_collide())
	{
		auto blocking_entities = world->get_entities(
			EntityFilter
			{
				.ignore_held = true,
				.ignore_ids = { actor->ID() },
				.ignore_flags = EntityFlag::FIXED_IN_PLACE,

				.required_flags = EntityFlag::IS_BLOCKING,
				.required_positions = { destination() }
			}
		);

		if (blocking_entities.size() > 1)
		{
			return ActionResult(false); // Space is too packed
		}
		else if (blocking_entities.size() > 0 && blocking_entities.front() != actor)
		{
			return ActionResult
			{
				.success = true,
				.next = new SwapPlacesAction(actor, blocking_entities.front())
			};
		}
	}

	if (actor->is_being_grappled())
	{
		return ActionResult(false);
	}

	if (actor->is_grappling_other())
	{
		actor->get_grapple_target()->start_action(QueuedAction(new MovementAction(actor->get_grapple_target(), m_dx, m_dy, m_dz)));
	}

	return ActionResult
	{
		.success = true,
		.next = new MovementAction(actor, m_dx, m_dy, m_dz)
	};
}

TimeUnit MovementAction::get_action_length() const
{ 
	return TimeUnit::from_seconds(1.0) / m_actor->move_speed(); 
}

bool MovementAction::check_action() const
{
	if (!ActionWithDirection::check_action())
		return false;

	if (!get_actor()->has_movable())
	{
		ACTION_CERR("Actor has no movable! " << get_actor()->ID());
		return false;
	}

	if (get_actor()->fixed_in_place())
	{
		ACTION_CERR("Actor is fixed in place! " << get_actor()->ID());
		return false;
	}

	auto world = get_world();

	if (!world)
	{
		ACTION_CERR("No world found!");
		return false;
	}

	return true;
}

ActionResult MovementAction::perform()
{
	auto world = get_world();
	auto actor = get_actor();

	auto tile_map = actor->get_tile_map();

	Coord start_x = actor->movable()->posx();
	Coord start_y = actor->movable()->posy();
	Coord start_z = actor->movable()->posz();

	Coord final_x = start_x + m_dx;
	Coord final_y = start_y + m_dy;
	Coord final_z = start_z + m_dz;

	if (get_action_length() > TimeUnit::from_seconds(0.0f) && actor->can_fall() && !actor->is_on_floor())
	{
		return ActionResult(false);
	}

	if (tile_map && actor->is_map_bound())
	{
		final_x = std::clamp(final_x, 0, Coord(tile_map->get_width()) - 1);
		final_y = std::clamp(final_y, 0, Coord(tile_map->get_height()) - 1);
		final_z = std::clamp(final_z, 0, Coord(tile_map->get_depth()) - 1);
	}

	Coord delta_x = final_x - start_x;
	Coord delta_y = final_y - start_y;
	Coord delta_z = final_z - start_z;

	if (actor->can_collide())
	{
		bool has_collision = false;

		if (world->is_blocked(final_x, final_y, final_z, actor, delta_z))
		{
			has_collision = true;
		}

		// Attempt to slide along wall
		if (has_collision)
		{
			if (abs(delta_z) > 0 && world->is_blocked(final_x, start_y, start_z, actor, delta_z))
			{
				final_x = start_x;
			}

			if (abs(delta_y) > 0 && world->is_blocked(start_x, final_y, start_z, actor, delta_z))
			{
				final_y = start_y;
			}

			if (abs(delta_x) > 0 && world->is_blocked(start_x, start_y, final_z, actor, delta_z))
			{
				final_z = start_z;
			}
		}

		if (world->is_blocked(final_x, final_y, final_z, actor, delta_z))
		{
			return ActionResult(false);
		}
	}

	if (tile_map && delta_z > 0 && actor->can_fall())
	{
		if (!tile_map->get_tile(start_x, start_y, start_z)->can_ascend())
		{
			return ActionResult(false);
		}
	}

	actor->movable()->set_position(final_x, final_y, final_z);
	actor->movable()->face_direction(final_x - start_x, final_y - start_y, 0);

	return ActionResult(true);
}