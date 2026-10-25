#include "sidebar.h"
#include "world_state.h"

Sidebar::Sidebar(WorldGameState* world_state):
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

void Sidebar::render(RenderParams params) const
{
	int panel_width = output::get_console_width() - params.world_render_frame.size.x;
	int panel_height = params.world_render_frame.size.y;

	panel_width = std::clamp(panel_width, 2, output::get_console_width());
	panel_height = std::clamp(panel_height, 2, output::get_console_height());

	int panel_x = output::get_console_width() - panel_width;
	int panel_y = 0;

	color_t fg = color::get("white");
	color_t bg = color::get("black");

	output::draw_frame_rgb(panel_x, panel_y, panel_width, panel_height, fg, bg, DEFAULT_RECT_FRAME, true, TCOD_BKGND_SET);
}
