#include "location_actions.h"
#include "tile_map.h"
#include "world.h"
#include "raw_master.h"

using namespace std;

bool ActionWithLocation::check_action()
{
	return get_world() != nullptr;
}

Tile* ActionWithLocation::get_tile_at_dest()
{
	if (auto tm = get_tile_map())
		return tm->get_tile(m_loc_x, m_loc_y, m_loc_z);
	return &::tile_map::VOID_TILE;
}

static bool transform_tile(World* world, int loc_x, int loc_y, int loc_z)
{
	if (!world)
	{
		return false;
	}

	auto tile_map = world->get_tile_map();
	const auto tile = tile_map->get_tile(loc_x, loc_y, loc_z);

	auto transformed_tile = RawMaster::get_tile_type(tile->get_type()->get_transform_ID());

	if (world->has_blocking_entities_at(loc_x, loc_y, loc_z))
	{
		return false;
	}

	tile_map->set_tile(loc_x, loc_y, loc_z, transformed_tile);

	return true;
}

static bool recursively_open_or_close_tiles(World* world, int loc_x, int loc_y, int loc_z, bool open)
{
	if (!world)
	{
		return false;
	}

	auto tile_map = world->get_tile_map();
	auto tile = tile_map->get_tile(loc_x, loc_y, loc_z);

	if (!transform_tile(world, loc_x, loc_y, loc_z))
	{
		return false;
	}

	if (!(tile->opens_neighbors() || tile->closes_neighbors()))
	{
		return true;
	}

	for (auto& check_location : transforms::get_surrounding_locations(Position(loc_x, loc_y, loc_z)))
	{
		auto other_tile = tile_map->get_tile(check_location);

		if (tile->can_connect_with(other_tile) || other_tile->can_connect_with(tile))
		{
			if (open && other_tile->can_open())
			{
				if (!recursively_open_or_close_tiles(world, check_location.x, check_location.y, check_location.z, true))
				{
					transform_tile(world, loc_x, loc_y, loc_z);
					return false;
				}
			}

			if (!open && other_tile->can_close())
			{
				if (!recursively_open_or_close_tiles(world, check_location.x, check_location.y, check_location.z, false))
				{
					transform_tile(world, loc_x, loc_y, loc_z);
					return false;
				}
			}
		}
	}

	return true;
}

bool OpenAction::check_action()
{
	if (get_tile_map() == nullptr)
		return false;

	if (!get_tile_at_dest()->can_open())
		return false;

	return true;
}

ActionResult OpenAction::perform()
{
	return ActionResult(recursively_open_or_close_tiles(get_world(), m_loc_x, m_loc_y, m_loc_z, true));
}

bool CloseAction::check_action()
{
	if (get_tile_map() == nullptr)
		return false;

	if (!get_tile_at_dest()->can_close())
		return false;

	return true;
}

ActionResult CloseAction::perform()
{
	return ActionResult(recursively_open_or_close_tiles(get_world(), m_loc_x, m_loc_y, m_loc_z, false));
}