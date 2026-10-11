#include "npc_generator.h"
#include "world_state.h"
#include "world.h"

NPCGenerator::NPCGenerator(WorldGameState* owner) :
	m_owner(owner)
{
}

WorldGameState* NPCGenerator::get_world_game_state() const
{
	return m_owner;
}

World* NPCGenerator::get_world() const
{
	if (m_owner)
		return m_owner->get_world();
	return nullptr;
}