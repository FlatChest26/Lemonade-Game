#pragma once

#ifndef LEMONADE_GAME_SRC_PLAYER_INPUT_HANDLER_H
#define LEMONADE_GAME_SRC_PLAYER_INPUT_HANDLER_H

#include "input_handler.h"
#include "world.h"
#include "entity.h"
#include "camera.h"


class SidebarMenu_Inventory;
class WorldGameState;

class PlayerInputHandler : public InputHandler
{
protected:
	SidebarMenu_Inventory* m_inventory_menu;

public:
	virtual void init() override;
	virtual void step(const TimeUnit& delta_time) override;
	virtual void render(const RenderParams& params) const override;

protected:
	// -- Input Handling -- //

	OVERRIDE_HANDLER_EVENT(key_down);
};

class PlayerLookHandler : public PlayerInputHandler
{
protected:
	Position m_look_focus{ 0, 0, 0 };

public:
	virtual void init() override;
	virtual void remove() override;
	virtual void render(const RenderParams& params) const override;

protected:

	// -- Input Handling -- //

	OVERRIDE_HANDLER_EVENT(key_down);
};

class PlayerInventoryHandler : public PlayerInputHandler
{
public:
	virtual void init() override;
	virtual void remove() override;
	virtual void step(const TimeUnit& delta_time) override;

protected:
	// -- Input Handling -- //

	OVERRIDE_HANDLER_EVENT(key_down);
};

#endif // !LEMONADE_GAME_SRC_PLAYER_INPUT_HANDLER_H