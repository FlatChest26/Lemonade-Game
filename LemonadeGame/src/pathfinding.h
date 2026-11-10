#pragma once

#ifndef LEMONADE_GAME_SRC_PATHFINDING_H
#define LEMONADE_GAME_SRC_PATHFINDING_H

#include "transforms.h"
#include <vector>
#include <unordered_map>

class TileMap;
class Entity;
class Action;

namespace Pathfinding
{
	struct Node
	{
		Position pos{ 0, 0, 0 };

		bool visited{ false };
		Node* parent{ nullptr };

		double path_cost{ 0.0 };
		double total_cost{ 0.0 };
	};

	struct AStarResult
	{
		std::unordered_map<Position, Node> nodes;
		bool found_end = false;

		Node* get_node(const Position& pos)
		{
			if (!nodes.contains(pos))
			{
				nodes[pos] = Node
				{
					.pos = pos,

					.visited = false,
					.parent = nullptr,

					.path_cost = INFINITY,
					.total_cost = INFINITY,
				};
			}

			return &nodes.at(pos);
		};

		const Node* get_node(const Position& pos) const
		{
			if (!nodes.contains(pos))
			{
				return nullptr;
			}

			return &nodes.at(pos);
		}
	};

	AStarResult astar(
		TileMap* tile_map,
		const Position& start_pos,
		const Position& end_pos,
		Entity* entity = nullptr,
		bool show_debug = false
	);

	std::vector<Position> compute_path(
		TileMap* tile_map, 
		const Position& start_pos,
		const Position& end_pos, 
		Entity* entity = nullptr, 
		bool show_debug = false
	);

}

#endif // !LEMONADE_GAME_SRC_PATHFINDING_H
