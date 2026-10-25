#pragma once

#ifndef LEMONADE_GAME_SRC_NPC_GENERATOR_H
#define LEMONADE_GAME_SRC_NPC_GENERATOR_H

#include "entity.h"
#include "time_units.h"
#include "transforms.h"
#include "snowy_rng.h"

#include <memory>

class AIManager;
class WorldGameState;
class World;

class NPCGenerator
{
protected:
	AIManager* m_owner{ nullptr };
	uint32_t m_npc_count{ 0 };
	

public:
	NPCGenerator(AIManager* owner);

	AIManager* get_ai_manager() const;
	WorldGameState* get_world_state() const;
	World* get_world() const;


public:
	// -- Utilities -- //

	Entity* generate_npc();
};

#endif // !LEMONADE_GAME_SRC_NPC_GENERATOR_H
