#pragma once

#ifndef LEMONADE_GAME_SRC_SIDEBAR_H
#define LEMONADE_GAME_SRC_SIDEBAR_H

#include "render_params.h"
#include <vector>
#include <memory>

class WorldGameState;
class World;

struct SidebarParams
{

};

class SidebarMenu
{
public:
	virtual void render(RenderParams params, SidebarParams sb_params) const = 0;
};

class Sidebar
{
protected:
	WorldGameState* m_world_state;
	std::vector<std::unique_ptr<SidebarMenu>> m_menus;

public:
	Sidebar(WorldGameState* world_state);
	
	WorldGameState* get_world_state() const;
	World* get_world() const;

	void add_menu(std::unique_ptr<SidebarMenu> menu)
	{
		m_menus.push_back(std::move(menu));
	}

public:
	void render(RenderParams params) const;
};


#endif // !LEMONADE_GAME_SRC_SIDEBAR_H