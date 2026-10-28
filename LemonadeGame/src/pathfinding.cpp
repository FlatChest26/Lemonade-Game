#include "pathfinding.h"
#include "tile_map.h"
#include "entity.h"
#include "world.h"
#include <unordered_map>
#include "transforms.h"

namespace Pathfinding
{
	struct Node
	{
		Position pos{ 0, 0, 0 };

		bool visited{ false };
		Node* parent{ nullptr };

		double global_goal{ 0.0f };
		double local_goal{ 0.0f };
	};

	//entity can be null!
	static double get_move_cost(TileMap* tile_map, Position pos, Entity* entity = nullptr)
	{
		if (!tile_map)
		{
			CERR("tile_map is null!");
			return -1.0f;
		}

		auto tile_at = tile_map->get_tile(pos);
		double cost = tile_at->get_move_cost();

		if (entity && entity->can_collide())
		{
			if (tile_at->is_blocked())
			{
				return -1.0f;
			}

			auto world = tile_map->get_world();

			if (!world)
			{
				CERR("world on tile_map is null!");
			}
			else if (world->has_blocking_entities_at(pos, { .ignore_entities = { entity }, .ignore_flags = EntityFlag::FREE_TO_MOVE }))
			{
				cost *= 1000.0f;
			}
		}

		if (entity && entity->can_fall() && (!tile_at->has_floor() || tile_at->is_air()))
		{
			auto tile_below = tile_map->get_tile(pos + Position(0, 0, -1));
			if (!tile_below->has_ceiling())
			{
				//cost *= 50.0f;
				cost = -1.0f;
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
			return -1.0f;
		}

		return tile_map->euclidian_distance_between_squared(from_pos, to_pos);
	}

	static Node* create_node_at(const Coord& x, const Coord& y, const Coord& z)
	{
		return new Node{
			.pos = Position(x, y, z),

			.visited = false,
			.parent = nullptr,

			.global_goal = INFINITY,
			.local_goal = INFINITY,
		};
	}

	static Node* create_node_at(const Position& pos)
	{
		return new Node{
			.pos = pos,

			.visited = false,
			.parent = nullptr,

			.global_goal = INFINITY,
			.local_goal = INFINITY,
		};
	}

	//entity can be null!
	std::vector<Position> Pathfinding::compute_path(TileMap* tile_map, Position start_pos, Position end_pos, Entity* entity)
	{
		if (!tile_map)
		{
			CERR("tile_map is null!");
			return {};
		}

		std::unordered_map<Position, std::unique_ptr<Node>> created_nodes;

		auto get_node = [&created_nodes, &tile_map](const Position& pos) mutable
			{
				if (!tile_map->in_bounds(pos))
				{
					return (Node*)nullptr;
				}

				if (!created_nodes.contains(pos))
				{
					created_nodes[pos] = std::unique_ptr<Node>(create_node_at(pos));
				}

				return created_nodes[pos].get();
			};

		start_pos.x = std::clamp(start_pos.x, 0, (Coord)tile_map->get_width());
		start_pos.y = std::clamp(start_pos.y, 0, (Coord)tile_map->get_height());
		start_pos.z = std::clamp(start_pos.z, 0, (Coord)tile_map->get_depth());

		end_pos.x = std::clamp(end_pos.x, 0, (Coord)tile_map->get_width());
		end_pos.y = std::clamp(end_pos.y, 0, (Coord)tile_map->get_height());
		end_pos.z = std::clamp(end_pos.z, 0, (Coord)tile_map->get_depth());

		Node* current_node = get_node(start_pos); // Start node

		current_node->local_goal = 0.0f;
		current_node->global_goal = get_hueristic(tile_map, start_pos, end_pos, entity);

		std::vector<Node*> nodes_to_check;
		nodes_to_check.push_back(current_node);

		while (!nodes_to_check.empty())
		{
			// Sort the list of nodes by how close they are to the end point
			std::sort(nodes_to_check.begin(), nodes_to_check.end(), [](Node* n1, Node* n2) { return n1->global_goal > n2->global_goal; });

			// Get rid of all the nodes which we've already visited
			while (!nodes_to_check.empty() && nodes_to_check.back()->visited)
				nodes_to_check.pop_back();

			if (nodes_to_check.empty())
				break;

			current_node = nodes_to_check.back();
			current_node->visited = true;

			if (current_node->pos == end_pos) break; // Break out early if ANY path to the target is found

			Tile* tile = tile_map->get_tile(current_node->pos);
			bool can_fall = false;

			if (entity && entity->can_fall() && (!tile->has_floor() || tile->is_air()))
			{
				auto tile_below = tile_map->get_tile(current_node->pos + Position(0, 0, -1));
				can_fall = !tile_below->has_ceiling();
			}

			// Check for unvisited nodes
			for (const Position& check_location : transforms::get_surrounding_locations(current_node->pos, tile->can_ascend(), can_fall || tile->can_descend()))
			{
				if (!tile_map->in_bounds(check_location))
					continue;

				Node* neighbor_node = get_node(check_location);
				
				if (!neighbor_node)
					continue;

				double move_cost = get_move_cost(tile_map, check_location, entity);
				
				if (!neighbor_node->visited && move_cost > 0.0f)
					nodes_to_check.push_back(neighbor_node);

				double possible_lower_goal = current_node->local_goal + (tile_map->euclidian_distance_between_squared(current_node->pos, neighbor_node->pos) + move_cost);

				if (possible_lower_goal < neighbor_node->local_goal)
				{
					neighbor_node->parent = current_node;
					neighbor_node->local_goal = possible_lower_goal;

					neighbor_node->global_goal = neighbor_node->local_goal + get_hueristic(tile_map, neighbor_node->pos, end_pos, entity);
				}
			}
		}

		std::vector<Position> path;

		Node* checking_node = get_node(end_pos);

		while (checking_node != nullptr)
		{
			path.push_back(checking_node->pos);
			checking_node = checking_node->parent;
		}

		if (path.back() == start_pos)
		{
			path.pop_back();
		}

		return path;
	}
}