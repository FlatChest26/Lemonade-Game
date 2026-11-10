#include "pathfinding.h"
#include "tile_map.h"
#include "entity.h"
#include "world.h"
#include "transforms.h"
#include "debug_info.h"

#include <unordered_map>

#include "direction_action.h"
#include "location_actions.h"

namespace Pathfinding
{
	//entity can be null!
	static double get_move_cost(TileMap* tile_map, Position pos, Entity* entity = nullptr)
	{
		auto tile_at = tile_map->get_tile(pos);
		double cost = tile_at->get_move_cost();

		if (entity)
		{
			if (entity->can_collide())
			{
				auto world = tile_map->get_world();

				if (world->has_blocking_entities_at(pos, { .ignore_ids = { entity->ID() }, .ignore_flags = EntityFlag::FREE_TO_MOVE }))
				{
					return -1.0;
				}
			}

			if (entity->can_fall() && (!tile_at->has_floor() || tile_at->is_air()))
			{
				auto tile_below = tile_map->get_tile(pos + Position(0, 0, -1));
				if (!tile_below->has_ceiling())
				{
					cost *= 50.0;
				}
			}
		}

		return cost;
	}

	static double get_hueristic(TileMap* tile_map, Position from_pos, Position to_pos, Entity* entity)
	{
		return (to_pos - from_pos).length_squared() * (2.0 * 2.0);
	}

	AStarResult astar(TileMap* tile_map, const Position& start_pos, const Position& end_pos, Entity* entity, bool show_debug)
	{
		AStarResult result;

		if (show_debug)
			DebugInfo::clear_debug_tiles();

		if (!tile_map)
		{
			CERR("tile_map is null!");
			return result;
		}

		if (!tile_map->in_bounds(start_pos))
		{
			CERR("start_pos is out of bounds!");
			return result;
		}

		if (!tile_map->in_bounds(end_pos))
		{
			CERR("end_pos is out of bounds!");
			return result;
		}

		Node* current_node = result.get_node(start_pos); // Start node

		current_node->path_cost = 0.0f;
		current_node->total_cost = get_hueristic(tile_map, start_pos, end_pos, entity);

		std::vector<Node*> nodes_to_check;
		nodes_to_check.push_back(current_node);

		while (!nodes_to_check.empty())
		{
			// Sort the list of nodes by how close they are to the end point
			std::sort(nodes_to_check.begin(), nodes_to_check.end(), [](Node* n1, Node* n2) { return n1->total_cost > n2->total_cost; });

			// Get rid of all the nodes which we've already visited
			while (!nodes_to_check.empty() && nodes_to_check.back()->visited)
				nodes_to_check.pop_back();

			if (nodes_to_check.empty())
				break;

			current_node = nodes_to_check.back();
			current_node->visited = true;

			if (current_node->pos == end_pos) // Break out early if ANY path to the target is found
			{
				result.found_end = true;
				break;
			}

			Tile* tile = tile_map->get_tile(current_node->pos);
			bool can_fall = false;

			if (entity && entity->can_fall() && (!tile->has_floor() || tile->is_air()))
			{
				auto tile_below = tile_map->get_tile(current_node->pos + Position(0, 0, -1));
				can_fall = !(tile_below->is_blocked() || tile_below->has_ceiling());
			}

			// Check for unvisited nodes
			for (const Position& check_location : transforms::get_surrounding_locations(current_node->pos, tile->can_ascend(), can_fall || tile->can_descend()))
			{
				if (!tile_map->in_bounds(check_location))
					continue;

				Node* neighbor_node = result.get_node(check_location);

				if (!neighbor_node)
					continue;

				double move_cost = get_move_cost(tile_map, check_location, entity);

				if (move_cost < DBL_EPSILON)
					continue;

				if (!neighbor_node->visited)
				{
					nodes_to_check.push_back(neighbor_node);

					if (show_debug)
						DebugInfo::set_debug_tile(neighbor_node->pos, DebugInfo::DebugTile{ .renderable = { .bg = color::get("red") }, .change_bg = true });
				}

				double new_path_cost = (neighbor_node->pos - current_node->pos).length_squared() + (move_cost * move_cost);
				double possible_lower_path_cost = current_node->path_cost + new_path_cost;

				if (possible_lower_path_cost < neighbor_node->path_cost)
				{
					neighbor_node->parent = current_node;
					neighbor_node->path_cost = possible_lower_path_cost;

					neighbor_node->total_cost = neighbor_node->path_cost + get_hueristic(tile_map, neighbor_node->pos, end_pos, entity);
				}
			}
		}

		return result;
	}

	std::vector<Position> compute_path(TileMap* tile_map, const Position& start_pos, const Position& end_pos, Entity* entity, bool show_debug)
	{
		AStarResult result = astar(tile_map, start_pos, end_pos, entity, show_debug);

		std::vector<Position> path;

		Node* checking_node = result.get_node(end_pos);

		if (!checking_node)
		{
			return {};
		}

		while (checking_node != nullptr)
		{
			path.push_back(checking_node->pos);

			auto tile_at = tile_map->get_tile(checking_node->pos);

			if (tile_at->is_blocked() && tile_at->can_open())
			{
				path.push_back(checking_node->pos);
			}

			if (show_debug)
			{
				DebugInfo::set_debug_tile(checking_node->pos, DebugInfo::DebugTile{ .renderable = { .bg = color::get("yellow") }, .change_bg = true });
			}
			
			checking_node = checking_node->parent;
		}

		if (path.back() == start_pos)
		{
			path.pop_back();
		}

		return path;
	}
}