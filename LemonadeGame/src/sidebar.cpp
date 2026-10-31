#include "sidebar.h"
#include "world_state.h"

Sidebar::Sidebar(WorldGameState* world_state) :
	m_world_state(world_state)
{
}

WorldGameState* Sidebar::get_world_state() const
{
	return m_world_state;
}

World* Sidebar::get_world() const
{
	if (m_world_state)
	{
		return m_world_state->get_world();
	}

	return nullptr;
}

Entity* Sidebar::get_player() const
{
	if (m_world_state)
	{
		return m_world_state->get_player();
	}

	return nullptr;
}

TileMap* Sidebar::get_tile_map() const
{
	if (auto world = get_world())
	{
		return world->get_tile_map();
	}

	return nullptr;
}

void Sidebar::render(const RenderParams& params) const
{
	int panel_width = output::get_console_width() - params.world_render_frame.size.x;
	int panel_height = output::get_console_height();

	panel_width = std::clamp(panel_width, 2, int(output::get_console_width() * 0.5));
	panel_height = std::clamp(panel_height, 2, output::get_console_height());

	int panel_x = params.world_outer_frame_p2().x;
	int panel_y = 0;

	color_t fg = color::get("white");
	color_t bg = color::get("black");

	//output::draw_frame_rgb(panel_x, panel_y, panel_width, panel_height, fg, bg, DEFAULT_RECT_FRAME, true, TCOD_BKGND_SET);

	int menu_x = panel_x;
	int menu_y = panel_y;

	for (const auto& [ID, sidebar_menu] : m_menus)
	{
		if (!sidebar_menu->is_visible())
			continue;

		menu_y += sidebar_menu->render(
			params,
			SidebarParams
			{
				.sidebar = this,
				.sidebar_rect = Rect2D{ .pos = Vec2D<Coord>(panel_x, panel_y), .size = Vec2D<Length>(panel_width, panel_height), .padding = 0 },
				.menu_position = Vec2D<Coord>(menu_x, menu_y)
			}
		);
	}
}

int SidebarMenu_GameInfo::render(const RenderParams& params, const SidebarParams& sb_params) const
{
	int panel_x = sb_params.menu_position.x;
	int starting_y = sb_params.menu_position.y + 1;
	int panel_y = starting_y;

	int panel_width = (int)(sb_params.sidebar_rect.size.x);

	auto print_text = [&](std::string text)
		{
			panel_y += output::console_printf_rect_ex(panel_x, panel_y, panel_width - 1, 0, DEFAULT_BKGND_FLAG, TCOD_LEFT, text);
		};

	if (auto world_state = sb_params.sidebar->get_world_state())
	{
		switch (world_state->get_tick_speed())
		{
		case TickSpeed::TURN_BASED:
			print_text("Game Speed | Turn-based");
			break;
		case TickSpeed::REAL_TIME:
			print_text("Game Speed | x1");
			break;
		case TickSpeed::REAL_TIME_2X:
			print_text("Game Speed | x2");
			break;
		case TickSpeed::REAL_TIME_4X:
			print_text("Game Speed | x4");
			break;
		case TickSpeed::REAL_TIME_8X:
			print_text("Game Speed | x8");
			break;
		}
	}

	if (auto world = sb_params.sidebar->get_world())
	{
		print_text("     Time  | " + world->get_time().clock_time().str_12_hr());
		print_text("     Date  | " + world->get_time().str_fancy(true));
	}

	return (panel_y - starting_y) + 2;
}

int SidebarMenu_PersonInfo::render(const RenderParams& params, const SidebarParams& sb_params) const
{
	int panel_x = sb_params.menu_position.x;
	int panel_y = sb_params.menu_position.y;

	int draw_x = panel_x + 1;
	int draw_y = panel_y + 1;

	int panel_width = (int)(sb_params.sidebar_rect.size.x);
	int panel_height = 2;

	color_t fg = color::get("white");
	color_t bg = color::get("black");

	auto print_text = [&](std::string text, color_t new_fg)
		{
			draw_y += output::console_printf_rgb_rect_ex(draw_x, draw_y, panel_width - 1, 0, new_fg, bg, DEFAULT_BKGND_FLAG, TCOD_LEFT, text);
		};

	print_text(fmt::format("{} ({})", entity->full_name(), entity->pronouns()->shortened), entity->fg());
	draw_y++;
	print_text(fmt::format("${:.2f}", entity->get_person()->cash().total_dollars()), color::get("yellow"));

	panel_height = std::max(panel_height, (draw_y - panel_y) + 1);

	output::draw_frame_rgb(panel_x, panel_y, panel_width, panel_height, color::get("gray"), bg, DEFAULT_RECT_FRAME, false, TCOD_BKGND_SET);
	output::console_printf_rgb_rect_ex(panel_x, panel_y, panel_width, 1, fg, bg, TCOD_BKGND_SET, TCOD_CENTER, "Identity");

	return panel_height;
}

int SidebarMenu_Look::render(const RenderParams& params, const SidebarParams& sb_params) const
{
	int panel_x = sb_params.menu_position.x;
	int panel_y = sb_params.menu_position.y;

	int draw_x = panel_x + 1;
	int draw_y = panel_y + 1;

	int panel_width = (int)(sb_params.sidebar_rect.size.x);
	int panel_height = 4;

	color_t fg = color::get("white");
	color_t bg = color::get("black");

	auto print_text = [&](std::string text, color_t new_fg)
		{
			draw_y += output::console_printf_rgb_rect_ex(draw_x, draw_y, panel_width - 1, 0, new_fg, bg, DEFAULT_BKGND_FLAG, TCOD_LEFT, text);
		};

	auto world = sb_params.sidebar->get_world();
	auto tile_map = sb_params.sidebar->get_tile_map();

	auto tile = tile_map->get_tile(look_position);
	auto entities_at = world->get_entities_at(look_position);

	// Write info at look focus

	if (tile->get_type())
	{
		if (!params.use_fov || tile->is_visible())
		{
			print_text(tile->display_name(), tile->fg());
		}
		else if (tile->is_explored())
		{
			print_text(fmt::format("{} (remembered)", tile->display_name()), color::get("gray"));
		}
		else
		{
			print_text("unexplored", color::get("gray"));
		}
	}

	for (const auto& entity : entities_at)
	{
		print_text(fmt::format("{}", entity->display_name()), entity->fg());
	}

	panel_height = std::max(panel_height, (draw_y - panel_y) + 1);

	std::string title = fmt::format("Look ({})", InputMap::actions_to_string(Keybind::Look));

	if (is_focused())
	{
		output::draw_frame_rgb(panel_x, panel_y, panel_width, panel_height, fg, bg, DEFAULT_RECT_FRAME, false, TCOD_BKGND_SET);
		output::console_printf_rgb_rect_ex(panel_x, panel_y, panel_width, 1, bg, fg, TCOD_BKGND_SET, TCOD_CENTER, title);
	}
	else
	{
		output::draw_frame_rgb(panel_x, panel_y, panel_width, panel_height, color::get("gray"), bg, DEFAULT_RECT_FRAME, false, TCOD_BKGND_SET);
		output::console_printf_rgb_rect_ex(panel_x, panel_y, panel_width, 1, fg, bg, TCOD_BKGND_SET, TCOD_CENTER, title);
	}

	return panel_height;
}

int SidebarMenu_Needs::render(const RenderParams& params, const SidebarParams& sb_params) const
{
	int panel_x = sb_params.menu_position.x;
	int panel_y = sb_params.menu_position.y;

	int draw_x = panel_x + 1;
	int draw_y = panel_y + 1;

	int panel_width = (int)(sb_params.sidebar_rect.size.x);
	int panel_height = 2;

	color_t fg = color::get("white");
	color_t bg = color::get("black");

	auto print_text = [&](std::string text, color_t new_fg)
		{
			draw_y += output::console_printf_rgb_rect_ex(draw_x, draw_y, panel_width - 1, 0, new_fg, bg, DEFAULT_BKGND_FLAG, TCOD_LEFT, text);
		};

	for (const auto& need_id : entity->get_person()->get_needs())
	{
		const auto& need = entity->get_person()->stat(need_id);
		print_text(fmt::format("{}", capitalize(need.name()), need.get_value()), fg);
		print_text(fmt::format(" You are {}.", need.description()), need.description_color());
	}

	panel_height = std::max(panel_height, (draw_y - panel_y) + 1);

	output::draw_frame_rgb(panel_x, panel_y, panel_width, panel_height, color::get("gray"), bg, DEFAULT_RECT_FRAME, false, TCOD_BKGND_SET);
	output::console_printf_rgb_rect_ex(panel_x, panel_y, panel_width, 1, fg, bg, TCOD_BKGND_SET, TCOD_CENTER, "Needs");

	return panel_height;
}

using InventoryListTuple = std::tuple<std::string, color_t, color_t, bool>;

int SidebarMenu_Inventory::render(const RenderParams& params, const SidebarParams& sb_params) const
{
	int panel_x = sb_params.menu_position.x;
	int panel_y = sb_params.menu_position.y;

	int panel_width = (int)(sb_params.sidebar_rect.size.x);
	int panel_height = 2;

	color_t fg = color::get("white");
	color_t bg = color::get("black");

	if (is_pinned() || !is_collapsed())
	{
		auto inventory = entity->get_inventory();

		panel_height = (int)inventory->get_max_slots();

		std::vector<InventoryListTuple> inventory_list;

		size_t slot_number = 0;
		for (const auto& item_slot : inventory->get_item_slots())
		{
			if (item_slot.item)
			{
				inventory_list.push_back(std::tuple(
					grammar::a(item_slot.item->display_noun()), item_slot.item->fg(), item_slot.item->bg(),
					m_focused && selected_slot == slot_number)
				);
			}
			else
			{
				inventory_list.push_back(std::tuple(
					"-", color::get("gray"), bg, m_focused && selected_slot == slot_number)
				);
			}

			slot_number++;
		}

		int draw_x = panel_x + 1;
		int draw_y = panel_y + 2;

		for (const auto& inv_list_item : inventory_list)
		{
			bool selected = std::get<3>(inv_list_item);
			color_t fg = selected ? std::get<2>(inv_list_item) : std::get<1>(inv_list_item);
			color_t bg = selected ? std::get<1>(inv_list_item) : std::get<2>(inv_list_item);

			draw_y += output::console_printf_rgb_rect_ex(
				draw_x, draw_y,
				panel_width - 2, 0,
				fg, bg, TCOD_BKGND_SET, TCOD_CENTER, std::get<0>(inv_list_item)
			);
		}

		panel_height = std::min((draw_y - panel_y) + 1, (int)sb_params.sidebar_rect.size.y - panel_y);
	}

	std::string title = fmt::format("Inventory ({})", InputMap::actions_to_string(Keybind::Inventory));

	if (is_focused())
	{
		output::draw_frame_rgb(panel_x, panel_y, panel_width, panel_height, fg, bg, DEFAULT_RECT_FRAME, false, TCOD_BKGND_SET);
		output::console_printf_rgb_rect_ex(panel_x, panel_y, panel_width, 1, fg, bg, TCOD_BKGND_SET, TCOD_CENTER, title);
	}
	else
	{
		output::draw_frame_rgb(panel_x, panel_y, panel_width, panel_height, color::get("gray"), bg, DEFAULT_RECT_FRAME, false, TCOD_BKGND_SET);
		output::console_printf_rgb_rect_ex(panel_x, panel_y, panel_width, 1, fg, bg, TCOD_BKGND_SET, TCOD_CENTER, title);
	}

	if (is_pinned())
	{
		output::put_rgb(panel_x + panel_width - 1, panel_y, Renderable('+'));
	}

	return panel_height;
}

int SidebarMenu_Examine::render(const RenderParams& params, const SidebarParams& sb_params) const
{
	if (examined_entities.empty() && !examined_tile)
	{
		return 0;
	}

	int panel_x = sb_params.menu_position.x;
	int panel_y = sb_params.menu_position.y;

	int draw_x = panel_x + 1;
	int draw_y = panel_y + 1;

	int panel_width = (int)(sb_params.sidebar_rect.size.x);
	int panel_height = 2;

	color_t fg = color::get("white");
	color_t bg = color::get("black");

	if (!is_focused())
	{
		fg = color::get("gray");
	}

	auto print_text = [&](std::string text, color_t new_fg = color::get("white"))
		{
			draw_y += output::console_printf_rgb_rect_ex(draw_x, draw_y, panel_width - 1, 0, new_fg, bg, DEFAULT_BKGND_FLAG, TCOD_LEFT, text);
		};

	if (examined_tile)
	{
		print_text(fmt::format("{}", examined_tile->display_name()), examined_tile->fg());
		print_text(fmt::format("\"{}\"", examined_tile->description()), fg);

		if (!examined_entities.empty())
		{
			output::draw_horizontal_line(draw_x, draw_y, panel_width - 1, fg, bg);
			draw_y++;
		}
	}

	if (!examined_entities.empty())
	{
		for (const auto& entity : examined_entities)
		{
			print_text(fmt::format("{}", (entity->is_name_known() ? entity->full_name() : entity->display_name())), entity->fg());
			print_text(fmt::format("\"{}\"", entity->description()));

			if (entity != examined_entities.back())
			{
				output::draw_horizontal_line(draw_x, draw_y, panel_width - 1, fg, bg);
				draw_y++;
			}
		}
	}

	panel_height = std::max(panel_height, (draw_y - panel_y) + 1);

	std::string title = fmt::format("Examine ({})", InputMap::actions_to_string(Keybind::Examine));

	output::draw_frame_rgb(panel_x, panel_y, panel_width, panel_height, fg, bg, DEFAULT_RECT_FRAME, false, TCOD_BKGND_SET);
	output::console_printf_rgb_rect_ex(panel_x, panel_y, panel_width, 1, is_focused() ? bg : color::get("white"), is_focused() ? fg : bg, TCOD_BKGND_SET, TCOD_CENTER, title);

	if (is_pinned())
	{
		output::put_rgb(panel_x + panel_width - 1, panel_y, Renderable('+'));
	}

	return panel_height;
}

int SidebarMenu_Interaction::render(const RenderParams& params, const SidebarParams& sb_params) const
{
	if (!actor || !subject)
	{
		return 0;
	}

	int panel_x = sb_params.menu_position.x;
	int panel_y = sb_params.menu_position.y;

	int draw_x = panel_x + 1;
	int draw_y = panel_y + 1;

	int panel_width = (int)(sb_params.sidebar_rect.size.x);
	int panel_height = 2;

	color_t fg = color::get("white");
	color_t bg = color::get("black");

	if (!is_focused())
	{
		fg = color::get("gray");
	}

	auto print_text = [&](std::string text, color_t new_fg = color::get("white"))
		{
			draw_y += output::console_printf_rgb_rect_ex(draw_x, draw_y, panel_width - 1, 0, new_fg, bg, DEFAULT_BKGND_FLAG, TCOD_LEFT, text);
		};


	panel_height = std::max(panel_height, (draw_y - panel_y) + 1);

	std::string title = fmt::format("{}", InputMap::actions_to_string(Keybind::Examine));

	output::draw_frame_rgb(panel_x, panel_y, panel_width, panel_height, fg, bg, DEFAULT_RECT_FRAME, false, TCOD_BKGND_SET);
	output::console_printf_rgb_rect_ex(panel_x, panel_y, panel_width, 1, is_focused() ? bg : color::get("white"), is_focused() ? fg : bg, TCOD_BKGND_SET, TCOD_CENTER, title);

	if (is_pinned())
	{
		output::put_rgb(panel_x + panel_width - 1, panel_y, Renderable('+'));
	}

	return panel_height;
}