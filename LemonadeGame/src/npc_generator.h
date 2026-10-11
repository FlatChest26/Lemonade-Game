#pragma once

#ifndef LEMONADE_GAME_SRC_NPC_GENERATOR_H
#define LEMONADE_GAME_SRC_NPC_GENERATOR_H

#include "creature.h"
#include "time_units.h"
#include "transforms.h"

#include <memory>

class WorldGameState;
class World;

class NPCGenerator
{
protected:
	WorldGameState* m_owner;

	TimeUnit m_time_since_spawned_NPC{};
	TimeUnit m_time_until_next_NPC{};
public:
	NPCGenerator(WorldGameState* owner);

	WorldGameState* get_world_game_state() const;
	World* get_world() const;

public:
	// -- Utilities -- //

	std::unique_ptr<Creature> generate_npc_at(Position pos) const;
};

#endif // !LEMONADE_GAME_SRC_NPC_GENERATOR_H
