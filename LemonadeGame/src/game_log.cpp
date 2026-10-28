#include "game_log.h"
#include "output.h"
#include "world_state.h"

GameLog::GameLog(WorldGameState* world_state):
	m_world_state(world_state)
{
}

WorldGameState* GameLog::get_world_state() const
{
	return m_world_state;
}

World* GameLog::get_world() const
{
	if (m_world_state)
	{
		return m_world_state->get_world();
	}

	return nullptr;
}

void GameLog::render(RenderParams params) const
{
	int panel_width = params.world_render_frame.size.x;
	int panel_height = output::get_console_height() - params.world_render_frame.size.y;

	int panel_x = 0;
	int panel_y = params.world_render_frame.size.y;

	panel_width = std::clamp(panel_width, 24, output::get_console_width() - panel_x);
	panel_height = std::clamp(panel_height, 2, output::get_console_height() - panel_y);

	color_t fg = color::get("white");
	color_t bg = color::get("black");

	//output::draw_horizontal_line(panel_x, panel_y + panel_height - 1, panel_width, fg, bg, TCOD_BKGND_SET);
	output::draw_frame_rgb(panel_x, panel_y, panel_width, panel_height, fg, bg, DEFAULT_RECT_FRAME, true, TCOD_BKGND_SET);
	output::console_printf_rgb_rect_ex(panel_x, panel_y, panel_width, 1, bg, fg, TCOD_BKGND_SET, TCOD_alignment_t::TCOD_CENTER, "Log");

	int draw_x = panel_x + 1;
	int draw_y = panel_y + panel_height - 2;

	for (const auto& msg : std::views::reverse(m_messages))
	{
		if (draw_y - panel_y < 1)
		{
			break;
		}

		color_t draw_fg = msg.fg;
		color_t draw_bg = msg.bg;

		if (auto world = get_world())
		{
			auto elotted_time = world->get_time().total_time() - msg.time_point;
			if (elotted_time >= TimeUnit::from_seconds(20.0) * world->get_time_progress_rate())
			{
				draw_fg.r /= 2;
				draw_fg.g /= 2;
				draw_fg.b /= 2;
			}
		}

		draw_y -= output::console_printf_rgb_rect_ex(
			draw_x, draw_y, panel_width - 2, panel_height - 2, draw_fg, draw_bg, TCOD_BKGND_SET, TCOD_LEFT, 
			msg.text + std::string(msg.repeat_count > 1 ? (" (x" + std::to_string(msg.repeat_count) + ")") : "")
		);
	}
}
