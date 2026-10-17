#pragma once

#ifndef LEMONADE_GAME_SRC_PLAYER_INPUT_HANDLER_H
#define LEMONADE_GAME_SRC_PLAYER_INPUT_HANDLER_H

#include "input_handler.h"
#include "world.h"
#include "entity.h"
#include "camera.h"

class WorldGameState;

class PlayerInputHandler : public InputHandler
{
public:
	virtual void render(RenderParams params) const override;

protected:
	// -- Input Handling -- //

	OVERRIDE_HANDLER_EVENT(key_down);
};

class PlayerLookHandler : public PlayerInputHandler
{
protected:
	Position m_look_focus{ 0, 0, 0 };

public:
	virtual void render(RenderParams params) const override;
	virtual void init() override;
	virtual void step(TimeUnit delta_time) override;

protected:

	// -- Input Handling -- //

	OVERRIDE_HANDLER_EVENT(key_down);
};

#endif // !LEMONADE_GAME_SRC_PLAYER_INPUT_HANDLER_H