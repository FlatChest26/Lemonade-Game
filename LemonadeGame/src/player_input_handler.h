#pragma once

#ifndef LEMONADE_GAME_SRC_PLAYER_INPUT_HANDLER_H
#define LEMONADE_GAME_SRC_PLAYER_INPUT_HANDLER_H

#include "input_handler.h"
#include "world.h"
#include "entity.h"
#include "camera.h"
#include "sidebar.h"

class WorldGameState;

namespace sidebar_menus
{
	inline SidebarMenuID GAME_INFO = "GAME_INFO";
	inline SidebarMenuID PERSON_INFO = "PERSON_INFO";
	inline SidebarMenuID LOOK = "LOOK";
	inline SidebarMenuID NEEDS = "NEEDS";
	inline SidebarMenuID INVENTORY = "INVENTORY";
	inline SidebarMenuID EXAMINE = "EXAMINE";
	inline SidebarMenuID INTERACTION = "INTERACTION";
}

class InputHandler_Player : public InputHandler
{
protected:
	SidebarMenu_GameInfo* m_game_info_menu;
	SidebarMenu_PersonInfo* m_person_info_menu;
	SidebarMenu_Look* m_look_menu;
	SidebarMenu_Needs* m_needs_menu;
	SidebarMenu_Inventory* m_inventory_menu;
	SidebarMenu_Examine* m_examine_menu;
	SidebarMenu_Interaction* m_interaction_menu;

public:
	template<typename T = SidebarMenu>
	T* register_sidebar(const SidebarMenuID& ID);

	virtual void init() override;
	virtual void step(const TimeUnit& delta_time) override;
	virtual void render(const RenderParams& params) const override;

protected:
	// -- Input Handling -- //

	OVERRIDE_HANDLER_EVENT(key_down);
};

class InputHandler_Look : public InputHandler_Player
{
public:
	virtual void init() override;
	virtual void remove() override;
	virtual void render(const RenderParams& params) const override;

protected:

	// -- Input Handling -- //

	OVERRIDE_HANDLER_EVENT(key_down);
};

class InputHandler_Inventory : public InputHandler_Player
{
public:
	virtual void init() override;
	virtual void remove() override;
	virtual void step(const TimeUnit& delta_time) override;

protected:
	// -- Input Handling -- //

	OVERRIDE_HANDLER_EVENT(key_down);
};

class InputHandler_Interaction : public InputHandler_Player
{
public:
	virtual void init() override;
	virtual void remove() override;

protected:
	// -- Input Handling -- //

	OVERRIDE_HANDLER_EVENT(key_down);
};

#include <functional>

class InputHandler_PositionSelector : public InputHandler_Player
{
protected:
	Position m_center;
	std::function<InputResult(InputHandler_Player*, const Position&)> m_callback;
	std::set<Position> m_valid_positions;
	std::string m_title;
	Keybind m_input_keybind;

public:
	struct initializer
	{
		Position center{ 0, 0, 0 };
		std::function<InputResult(InputHandler_Player*, const Position&)> callback;
		std::set<Position> valid_positions{};
		std::string title;
		Keybind input_keybind;
	};

	InputHandler_PositionSelector(const initializer& init) :
		m_center(init.center), 
		m_callback(init.callback), 
		m_valid_positions(init.valid_positions), 
		m_title(init.title),
		m_input_keybind(init.input_keybind)
	{}

	virtual void render(const RenderParams& params) const override;

protected:
	// -- Input Handling -- //

	OVERRIDE_HANDLER_EVENT(key_down);
};

#endif // !LEMONADE_GAME_SRC_PLAYER_INPUT_HANDLER_H
