#pragma once

#ifndef LEMONADE_GAME_SRC_AI_HANDLER_H
#define LEMONADE_GAME_SRC_AI_HANDLER_H


#include "time_units.h"
#include "npc_generator.h"

#include "ai_behavior.h"

#include <memory>

#include "ai_brain.h"

class World;
class GameState;

class AIManager
{
protected:
	GameState* m_game_state{ nullptr };
	std::unique_ptr<NPCGenerator> m_npc_generator{ nullptr };

	TimeUnit m_time_until_next_NPC{};

	Position m_sidewalk_start{ 0, 0, 0 };
	Position m_sidewalk_end{ 0, 0, 0 };
	Length m_sidewalk_width{ 0 };

public:
	AIManager(GameState* game_state = {});
public:

	GameState* get_game_state() const;
	WorldGameState* get_world_state() const;
	World* get_world() const;

	void set_sidewalk_start(Position pos) { m_sidewalk_start = pos; }
	void set_sidewalk_end(Position pos) { m_sidewalk_end = pos; }
	void set_sidewalk_width(Length size) { m_sidewalk_width = size; }

	Position get_sidewalk_start() const { return m_sidewalk_start; }
	Position get_sidewalk_end() const { return m_sidewalk_end; }
	Length get_sidewalk_width() const { return m_sidewalk_width; }

	AIBrain* create_brain();

public:
	void step(TimeUnit time_delta);
};

#endif // !LEMONADE_GAME_SRC_AI_HANDLER_H