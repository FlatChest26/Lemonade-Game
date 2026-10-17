#include "pathfinding.h"
#include "tile_map.h"
#include "entity.h"
#include "world.h"

namespace Pathfinding
{
	//entity can be null!
	static double get_move_cost(TileMap* tile_map, Position pos, Entity* entity)
	{
		if (!tile_map)
		{
			CERR("tile_map is null!");
			return -1.0f;
		}

		auto tile_at = tile_map->get_tile(pos);
		double cost = tile_at->get_move_cost();

		if (entity && entity->is_blocking())
		{
			auto world = tile_map->get_world();
			auto blocking = world->get_blocking_entities_at(pos);
			if (std::count_if(blocking.begin(), blocking.end(), [&](Entity* other) { return other != entity; }) > 0)
			{
				cost *= 50.0f;
			}
		}

		if (entity && entity->can_fall() && !tile_at->has_floor() || tile_at->is_air())
		{
			auto tile_below = tile_map->get_tile(pos + Position(0, 0, -1));
			if (!tile_below || !tile_below->has_ceiling())
			{
				cost *= 50.0f;
			}
		}

		return cost;
	}

	//entity can be null!
	static double get_hueristic(TileMap* tile_map, Position from_pos, Position to_pos, Entity* entity)
	{
		if (!tile_map)
		{
			CERR("tile_map is null!");
			return 0.0f;
		}

		return tile_map->euclidian_distance_between(from_pos, to_pos);
	}

	//entity can be null!
	std::vector<Position> Pathfinding::compute_path(TileMap* tile_map, Position start_pos, Position end_pos, Entity* entity )
	{
		if (!tile_map)
		{
			CERR("tile_map is null!");
			return {};
		}

		std::vector<Position> path;

		start_pos.x = std::clamp(start_pos.x, (Coord)0, (Coord)tile_map->get_width());
		start_pos.y = std::clamp(start_pos.y, (Coord)0, (Coord)tile_map->get_height());
		start_pos.z = std::clamp(start_pos.z, (Coord)0, (Coord)tile_map->get_depth());

		end_pos.x = std::clamp(end_pos.x, (Coord)0, (Coord)tile_map->get_width());
		end_pos.y = std::clamp(end_pos.y, (Coord)0, (Coord)tile_map->get_height());
		end_pos.z = std::clamp(end_pos.z, (Coord)0, (Coord)tile_map->get_depth());

		tile_map->reset_nodes();

		Node* start_node = tile_map->get_node(start_pos);
		Node* end_node = tile_map->get_node(end_pos);

		if (start_node == nullptr)
		{
			CERR("start_node is null!");
			return {};
		}

		if (end_node == nullptr)
		{
			CERR("end_node is null!");
			return {};
		}

		Node* current_node = start_node;
		current_node->local_goal = 0.0f;
		current_node->global_goal = get_hueristic(tile_map, start_pos, end_pos, entity);

		std::list<Node*> nodes_to_check;
		nodes_to_check.push_back(current_node);

		while (!nodes_to_check.empty())
		{
			// Sort the list of nodes by how close they are to the end point
			nodes_to_check.sort([](Node* n1, Node* n2) { return n1->global_goal < n2->global_goal; });

			// Get rid of all the nodes which we've already visited
			while (!nodes_to_check.empty() && nodes_to_check.front()->visited)
				nodes_to_check.pop_front();

			if (nodes_to_check.empty())
				break;

			current_node = nodes_to_check.front();
			current_node->visited = true;

			if (current_node->pos == end_pos) break; // Break out early if ANY path to the target is found

			Tile* tile = tile_map->get_tile(current_node->pos);
			bool can_fall = false;

			if (entity && entity->can_fall() && !tile->has_floor() || tile->is_air())
			{
				auto tile_below = tile_map->get_tile(current_node->pos + Position(0, 0, -1));
				can_fall = !tile_below || !tile_below->has_ceiling();
			}

			// Check for unvisited nodes
			for (const Position& check_location : transforms::get_surrounding_locations_above_or_below(current_node->pos, tile->can_ascend(), can_fall || tile->can_descend()))
			{
				if (!tile_map->in_bounds(check_location))
					continue;

				Node* neighbor_node = tile_map->get_node(check_location);
				if (!neighbor_node)
					continue;

				double move_cost = get_move_cost(tile_map, check_location, entity);
				
				if (!neighbor_node->visited && move_cost > 0.0f)
					nodes_to_check.push_back(neighbor_node);

				double possible_lower_goal = current_node->local_goal + (tile_map->euclidian_distance_between(current_node->pos, neighbor_node->pos) + move_cost);

				if (possible_lower_goal < neighbor_node->local_goal)
				{
					neighbor_node->parent = current_node;
					neighbor_node->local_goal = possible_lower_goal;

					neighbor_node->global_goal = neighbor_node->local_goal + get_hueristic(tile_map, neighbor_node->pos, end_pos, entity);
				}
			}
		}

		Node* checking_node = end_node;

		while (checking_node->parent != nullptr)
		{
			path.push_back(checking_node->pos);
			checking_node = checking_node->parent;
		}

		return path;
	}
}