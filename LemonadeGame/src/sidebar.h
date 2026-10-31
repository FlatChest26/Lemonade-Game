#pragma once

#ifndef LEMONADE_GAME_SRC_SIDEBAR_H
#define LEMONADE_GAME_SRC_SIDEBAR_H

#include "render_params.h"
#include <vector>
#include <memory>

class WorldGameState;
class World;
class Entity;
class Tile;
class TileMap;
class Sidebar;

struct SidebarParams
{
	const Sidebar* sidebar;
	Rect2D sidebar_rect;
	Vec2D<Coord> menu_position;
};

using SidebarMenuID = std::string;

class SidebarMenu
{
protected:
	SidebarMenuID m_ID;
	bool m_visible{ true };
	bool m_focused{ false };
	bool m_collapsed{ false };
	bool m_pinned{ false };

public:
	SidebarMenuID ID() const { return m_ID; }
	void set_ID(SidebarMenuID ID) { m_ID = ID; }

	bool is_focused() const { return m_focused; }
	void set_focused(bool focused) { m_focused = focused; }

	void focus() { set_focused(true); }
	void unfocus() { set_focused(false); }

	bool is_collapsed() const { return m_collapsed; }
	void set_collapsed(bool collapsed) { m_collapsed = collapsed; }

	void collapse() { set_collapsed(true); }
	void uncollapse() { set_collapsed(false); }

	bool is_pinned() const { return m_pinned; }
	void set_pinned(bool pinned) { m_pinned = pinned; }

	void pin() { set_pinned(true); }
	void unpin() { set_pinned(false); }

	bool is_visible() const { return m_visible; }
	void set_visible(bool visible) { m_visible = visible; }

	void show() { set_visible(true); }
	void hide() { set_visible(false); }

public:
	virtual int render(const RenderParams& params, const SidebarParams& sb_params) const = 0;
};

class Sidebar
{
protected:
	WorldGameState* m_world_state;
	std::unordered_map<SidebarMenuID, std::unique_ptr<SidebarMenu>> m_menus;

public:
	Sidebar(WorldGameState* world_state);

	WorldGameState* get_world_state() const;
	World* get_world() const;
	Entity* get_player() const;
	TileMap* get_tile_map() const;

public:
	SidebarMenu* add_menu(const SidebarMenuID& ID, std::unique_ptr<SidebarMenu> menu)
	{
		menu->set_ID(ID);
		m_menus[ID] = std::move(menu);

		return m_menus[ID].get();
	}

	void remove_menu(const SidebarMenuID& ID)
	{
		m_menus.erase(ID);
	}

	SidebarMenu* get_menu(const SidebarMenuID& ID)
	{
		if (has_menu(ID))
		{
			return m_menus[ID].get();
		}

		return nullptr;
	}

	bool has_menu(const SidebarMenuID& ID) const
	{
		return m_menus.find(ID) != m_menus.end();
	}

public:
	void render(const RenderParams& params) const;
};

class SidebarMenu_GameInfo : public SidebarMenu
{
public:
	virtual int render(const RenderParams& params, const SidebarParams& sb_params) const override;
};

class SidebarMenu_Look : public SidebarMenu
{
public:
	Position look_position;
	bool follow_player;
	virtual int render(const RenderParams& params, const SidebarParams& sb_params) const override;
};

class SidebarMenu_PersonInfo : public SidebarMenu
{
public:
	Entity* entity;
	virtual int render(const RenderParams& params, const SidebarParams& sb_params) const override;
};

class SidebarMenu_Needs : public SidebarMenu
{
public:
	Entity* entity;
	virtual int render(const RenderParams& params, const SidebarParams& sb_params) const override;
};

class SidebarMenu_Inventory : public SidebarMenu
{
public:
	Entity* entity;
	int selected_slot{ -1 };
	virtual int render(const RenderParams& params, const SidebarParams& sb_params) const override;
};

class SidebarMenu_Examine : public SidebarMenu
{
public:
	std::vector<Entity*> examined_entities;
	Tile* examined_tile;
	virtual int render(const RenderParams& params, const SidebarParams& sb_params) const override;
};

class SidebarMenu_Interaction : public SidebarMenu
{
public:
	Entity* actor;
	Entity* subject;


	virtual int render(const RenderParams& params, const SidebarParams& sb_params) const override;
};

#endif // !LEMONADE_GAME_SRC_SIDEBAR_H