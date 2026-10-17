#include "location_actions.h"
#include "tile_map.h"
#include "world.h"
#include "raw_master.h"

using namespace std;

bool ActionWithLocation::check_action()
{
	return get_tile_map() != nullptr;
}

Tile* ActionWithLocation::get_tile_at_dest()
{
	if (auto tm = get_tile_map())
		return tm->get_tile(m_loc_x, m_loc_y, m_loc_z);
	return nullptr;
}

ActionResult TransformAction::perform()
{
	auto tile_map = get_tile_map();
	if (!tile_map)
	{
		CERR("tile_map is null!");
		return ActionResult(false);
	}

	if (!tile_map->in_bounds(location()))
	{
		CERR("no tile at destination!");
		return ActionResult(false);
	}

	auto transformed_tile = RawMaster::get<TileType>(get_tile_at_dest()->get_type()->get_transform_ID());
	bool will_be_blocked = !(transformed_tile->tile_flags() & TileType::Flag::IS_WALKABLE);

	std::vector<Position> tiles_to_transform;

	std::vector<Position> locations_to_check;
	locations_to_check.push_back(location());

	std::vector<Position> locations_already_checked;

	while (!locations_to_check.empty())
	{
		Position loc = locations_to_check.back();
		locations_to_check.pop_back();

		locations_already_checked.push_back(loc);

		auto tile = tile_map->get_tile(loc);
		if (!tile || (m_open && !tile->can_open()) || (!m_open && !tile->can_close()))
			continue;

		if (will_be_blocked)
		{
			auto entities = tile_map->get_world()->get_entities_at(loc);

			if (!entities.empty())
			{
				auto is_blocked = std::find_if(
					entities.begin(), entities.end(),
					[&](const auto& entity) { return entity->can_collide(); }
				);

				if (is_blocked != entities.end())
				{
					return ActionResult(false);
				}
			}
		}

		tiles_to_transform.push_back(loc);

		if ((m_open && tile->opens_neighbors()) || (!m_open && tile->closes_neighbors()))
		{
			std::vector<Position> tiles_to_check
			{
				loc + Position(1, 0, 0),
				loc + Position(-1, 0, 0),
				loc + Position(0, 1, 0),
				loc + Position(0, -1, 0),
			};

			for (auto& check_location : tiles_to_check)
			{
				if (!tile_map->in_bounds(check_location))
					continue;

				auto other_tile = tile_map->get_tile(check_location);

				if (!(tile->can_connect_with(other_tile) || other_tile->can_connect_with(tile)))
					continue;

				auto already_checked = std::find_if(
					locations_already_checked.begin(), locations_already_checked.end(),
					[&check_location](const auto& pos) { return pos == check_location; }
				);

				if (already_checked != locations_already_checked.end())
					continue;

				locations_to_check.push_back(check_location);
			}
		}
	}

	for (auto& loc_to_trans : tiles_to_transform)
	{
		tile_map->set_tile(loc_to_trans, transformed_tile);
	}

	return ActionResult(true);
}

bool OpenAction::check_action()
{
	if (!ActionWithLocation::check_action())
		return false;

	if (!get_tile_at_dest()->can_open())
		return false;

	return true;
}

bool CloseAction::check_action()
{
	if (!ActionWithLocation::check_action())
		return false;

	if (!get_tile_at_dest()->can_close())
		return false;

	return true;
}
