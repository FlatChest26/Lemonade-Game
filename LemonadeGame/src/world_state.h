#pragma once

#ifndef LEMONADE_GAME_SRC_WORLD_STATE_H
#define LEMONADE_GAME_SRC_WORLD_STATE_H

#include "game_states.h"
#include "world.h"
#include "time_units.h"
#include "camera.h"

#include "ai_handler.h"
#include "npc_generator.h"
#include "game_log.h"
#include "sidebar.h"

enum class TickSpeed : uint32_t
{
	TURN_BASED,
	REAL_TIME,
	REAL_TIME_2X,
	REAL_TIME_4X,
	REAL_TIME_8X,
};

/* The main game state. Simulates the world of the game. */
class WorldGameState : public GameState
{
private:
	std::unique_ptr<World> m_world;
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<AIManager> m_ai_manager;
	std::unique_ptr<GameLog> m_log;
	std::unique_ptr<Sidebar> m_sidebar;

	Entity* m_player{ nullptr };
	Position m_previous_player_pos;

	Position m_camera_focus{ 0, 0, 0 };
	bool m_has_camera_focus{ false };

	TickSpeed m_tick_speed = TickSpeed::TURN_BASED;

public:
	WorldGameState() = default;

public:
	// -- Getters & Setters -- //

	bool does_player_exist() const { return m_player; }

	const Entity* get_player() const { return m_player; }
	Entity* get_player()  { return m_player; }

	void set_player(Entity* player) { m_player = player; }

	World* get_world() const { return m_world.get(); }
	Camera* get_camera() const { return m_camera.get(); }
	AIManager* get_ai_manager() const { return m_ai_manager.get(); }

	GameLog* get_log() const { return m_log.get(); }
	Sidebar* get_sidebar() const { return m_sidebar.get(); }

	constexpr TickSpeed get_tick_speed() const { return m_tick_speed; }
	void set_tick_speed(TickSpeed tick_speed) { m_tick_speed = tick_speed; }

	// -- Checks -- //

	virtual constexpr bool is_world_game_state() const override { return true; }
	virtual constexpr WorldGameState* as_world_game_state() const override { return (WorldGameState*)this; }

public:

	// -- Utilities -- //

	virtual void new_game();
	virtual void update(double delta_time) override;
	virtual void render() const override;

	void step(const TimeUnit& time_delta);

	void set_camera_focus(const Position& pos)
	{
		m_camera_focus = pos;
		m_has_camera_focus = true;
	}

	void remove_camera_focus()
	{
		m_has_camera_focus = false;
	}

	void update_camera();

	virtual void handle_action(QueuedAction action);

	virtual void handle_input_result(InputResult result) override;

	void game_message(const GameMessage& msg);
};

#endif // !LEMONADE_GAME_SRC_WORLD_STATE_H