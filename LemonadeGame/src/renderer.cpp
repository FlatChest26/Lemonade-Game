#include "renderer.h"

#include "game_states.h"
#include "world_state.h"
#include "world.h"
#include "entity.h"
#include "camera.h"
#include "tile_map.h"

#include "debug_info.h"

Renderer::Renderer(GameState* game_state) :
	m_game_state(game_state)
{
}

GameState* Renderer::get_game_state() const
{
	return m_game_state;
}

WorldGameState* WorldRenderer::get_world_game_state() const
{
	if (!m_game_state)
	{
		return nullptr;
	}

	return m_game_state->as_world_game_state();
}

World* WorldRenderer::get_world() const
{
	if (auto world_state = get_world_game_state())
	{
		return world_state->get_world();
	}

	return nullptr;
}

Camera* WorldRenderer::get_camera() const
{
	if (auto world_state = get_world_game_state())
	{
		return world_state->get_camera();
	}

	return nullptr;
}

Entity* WorldRenderer::get_player() const
{
	if (auto world_state = get_world_game_state())
	{
		return world_state->get_player();
	}

	return nullptr;
}

TileMap* WorldRenderer::get_tile_map() const
{
	if (auto world = get_world())
	{
		return world->get_tile_map();
	}

	return nullptr;
}

void WorldRenderer::render(const RenderParams& params) const
{
	if (!params.render_world)
	{
		return;
	}

	if (params.render_tile_map)
	{
		render_tile_map(params);
	}

#ifdef _DEBUG
	if (params.render_debug)
	{
		render_debug(params);
	}
#endif // _DEBUG

	if (params.render_entities)
	{
		render_entities(params);
	}
}

void WorldRenderer::render_entities(const RenderParams& params) const
{
	auto _render = [&](std::vector<Entity*> entities)
		{
			for (const auto entity : entities)
			{
				render_entity(params, entity);
			}
		};

	_render(get_world()->get_entities(
		EntityFilter
		{
			.ignore_held = true,
			.ignore_flags = EntityFlag::IS_HIDDEN | EntityFlag::IS_FLASHING,
			.required_flags = EntityFlag::IS_FURNITURE
		}
	));

	_render(get_world()->get_entities(
		EntityFilter
		{
			.ignore_held = true,
			.ignore_flags = EntityFlag::IS_HIDDEN | EntityFlag::IS_FLASHING,
			.required_flags = EntityFlag::IS_ITEM
		}
	));

	_render(get_world()->get_entities(
		EntityFilter
		{
			.ignore_held = true,
			.ignore_flags = EntityFlag::IS_HIDDEN | EntityFlag::IS_FLASHING,
			.required_flags = EntityFlag::IS_CREATURE
		}
	));
}

void WorldRenderer::render_entity(const RenderParams& params, const Entity* entity) const
{
	if (!entity)
		return;

	if (!entity->has_movable())
		return;

	if (params.use_fov && !entity->is_visible_to_player())
		return;

	if (params.camera && !params.camera->in_view(entity->get_movable()->get_transform()))
		return;

	auto world = get_world();

	if (!world)
	{
		CERR("No world found in renderer.");
		return;
	}

	Position camera_offset = Position(0, 0, 0);

	if (params.camera)
	{
		camera_offset = params.camera->get_screen(0, 0, 0);
	}

	for (const auto& offset : entity->get_movable()->get_offsets()) // In the case of drawing an entity larger than 1x1x1
	{
		Position world_position = entity->get_movable()->pos() + offset;

		int draw_x = world_position.x + camera_offset.x;
		int draw_y = world_position.y + camera_offset.y;
		int depth = world_position.z + camera_offset.z;

		if (!params.in_world_frame(draw_x, draw_y))
			continue;

		if (params.camera && !params.camera->in_view(world_position))
			continue;

		Renderable draw_renderable = entity->get_renderable_at(offset);

		if (depth < 0)
		{
			if (world->get_tile(world_position)->has_ceiling()) // Check if tile has a ceiling
				continue;

			if (world->get_tile(world_position + Position(0, 0, 1))->has_floor()) // Check if above tile has floor
				continue;

			draw_renderable.glyph = DISTANT_DOT;
		}

		if (!entity->is_on_floor())
		{
			draw_renderable.bg = color::get("col_falling");
		}

		output::put_rgb(
			draw_x + params.world_inner_frame_p1().x,
			draw_y + params.world_inner_frame_p1().y,
			draw_renderable, TCOD_BKGND_SET
		);
	}
}

void WorldRenderer::render_tile_map(const RenderParams& params) const
{
	auto tile_map = get_tile_map();

	if (!tile_map)
	{
		CERR("No tile map found in renderer.");
		return;
	}

	int layer = 0;

	int view_left = 0;
	int view_right = static_cast<int>(tile_map->get_width());

	int view_top = 0;
	int view_bottom = static_cast<int>(tile_map->get_height());

	if (params.camera)
	{
		layer = params.camera->get_posz();

		view_left = std::max(view_left, static_cast<int>(params.camera->get_x1()));
		view_right = std::min(view_right, static_cast<int>(params.camera->get_x2()));

		view_top = std::max(view_top, static_cast<int>(params.camera->get_y1()));
		view_bottom = std::min(view_bottom, static_cast<int>(params.camera->get_y2()));
	}

	if (view_right - view_left < 1 || view_bottom - view_top < 1)
		return;

	Position camera_offset = Position(0, 0, 0);

	if (params.camera)
	{
		camera_offset = params.camera->get_screen(0, 0, 0);
	}

	for (int x = view_left; x < view_right; x++) for (int y = view_top; y < view_bottom; y++)
	{
		int draw_x = x + camera_offset.x;
		int draw_y = y + camera_offset.y;
		int depth = layer + camera_offset.z;

		if (!params.in_world_frame(draw_x, draw_y))
			continue;

		if (params.camera && !params.camera->in_view(x, y, layer))
			continue;

		const Tile* draw_tile = tile_map->get_tile(x, y, layer);
		Renderable draw_renderable = draw_tile->renderable();

		if (draw_tile->is_air())
		{
			auto tile_below = tile_map->get_tile(x, y, layer - 1);

			if (!tile_below->is_air())
			{
				draw_renderable = Renderable{ DISTANT_DOT, tile_below->fg(), tile_below->bg() };
			}
		}

		if (params.use_fov && !tile_map->is_visible(x, y, layer))
		{
			if (params.show_explored_tiles && tile_map->is_explored(x, y, layer))
			{
				draw_renderable.fg = color::get("col_explored");
			}
			else
			{
				continue;
			}
		}

		output::put_rgb(
			draw_x + params.world_inner_frame_p1().x,
			draw_y + params.world_inner_frame_p1().y,
			draw_renderable, TCOD_BKGND_SET
		);
	}
}

void WorldRenderer::render_debug(const RenderParams& params) const
{
#ifdef _DEBUG

	for (auto& [pos, debug_tile] : DebugInfo::get_debug_tiles())
	{
		int draw_x{ pos.x }, draw_y{ pos.y }, depth{ pos.z };

		if (params.camera)
		{
			std::tie(draw_x, draw_y, depth) = params.camera->get_screen(pos).as_tuple();
		}

		if (depth != 0)
			continue;

		draw_x += params.world_inner_frame_p1().x;
		draw_y += params.world_inner_frame_p1().y;

		if (debug_tile.change_fg)
		{
			output::set_char(draw_x, draw_y, debug_tile.renderable.glyph);
		}

		if (debug_tile.change_fg)
		{
			output::set_fg(draw_x, draw_y, debug_tile.renderable.fg);
		}

		if (debug_tile.change_bg)
		{
			output::set_bg(draw_x, draw_y, debug_tile.renderable.bg);
		}

	}

#endif // _DEBUG

}