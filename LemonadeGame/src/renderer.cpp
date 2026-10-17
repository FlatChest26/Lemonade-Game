#include "renderer.h"

#include "game_states.h"
#include "world_state.h"
#include "world.h"
#include "entity.h"
#include "camera.h"
#include "tile_map.h"

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

void WorldRenderer::render(RenderParams params) const
{
	if (!params.render_world)
	{
		return;
	}

	if (params.render_tile_map)
	{
		render_tile_map(params);
	}

	if (params.render_entities)
	{
		render_entities(params);
	}
}

void WorldRenderer::render_entities(RenderParams params) const
{
	for (const auto entity : get_world()->get_entities())
	{
		render_entity(params, entity);
	}
}

void WorldRenderer::render_entity(RenderParams params, const Entity* entity) const
{
	if (!entity->has_movable())
	{
		return;
	}

	if (entity->is_hidden() || entity->is_flashing())
	{
		return;
	}

	if (params.use_fov && !entity->is_visible_to_player())
	{
		return;
	}

	auto tile_map = get_tile_map();

	int world_x, world_y, world_z;
	std::tie(world_x, world_y, world_z) = entity->get_movable()->pos().as_tuple();

	int draw_x{ world_x }, draw_y{ world_y }, depth{ 0 };

	if (auto& camera = params.camera)
	{
		if (!camera->in_view(world_x, world_y, world_z))
		{
			return;
		}

		std::tie(draw_x, draw_y, depth) = camera->get_screen(world_x, world_y, world_z).as_tuple();
	}

	if (params.use_fov)
	{
		if (!tile_map->is_visible(world_x, world_y, world_z) && entity->has_been_seen_by_player())
		{
			std::tie(world_x, world_y, world_z) = entity->get_last_seen_location().as_tuple();
		}
	}

	glyph_t draw_glyph{};
	color_t draw_fg{};
	color_t draw_bg{};

	for (const auto& [offset_x, offset_y, offset_z] : entity->get_movable()->get_offsets()) // In the case of drawing an entity larger than 1x1x1
	{
		draw_glyph = entity->glyph();
		draw_fg = entity->fg();
		draw_bg = entity->bg();

		if (entity->is_player())
		{
			draw_glyph = PLAYER_CHAR;
			draw_fg = DEFAULT_FG_COLOR;
		}

		if (params.use_fov && tile_map && !tile_map->is_visible(world_x + offset_x, world_y + offset_y, world_z + offset_z))
		{
			if (params.show_explored_tiles && tile_map->is_explored(world_x + offset_x, world_y + offset_y, world_z + offset_z))
			{
				draw_fg = explored_fg;
			}
			else
			{
				continue;
			}
		}

		if (depth - offset_z < 0 || depth - offset_z > 1)
			continue;

		if (depth - offset_z == 1)
		{
			if (auto world = get_world())
			{
				if (auto tile = world->get_tile(entity->get_movable()->pos()))
				{
					if (tile->has_ceiling())
					{
						continue;
					}
				}
				if (auto tile_above = world->get_tile(entity->get_movable()->pos() + Position(0, 0, 1)))
				{
					if (tile_above->has_floor())
					{
						continue;
					}
				}
			}

			draw_glyph = DISTANT_DOT;
		}

		if (!entity->is_on_floor())
		{
			draw_bg = color::get("light_cyan");
		}

		output::put_rgb(draw_x + offset_x, draw_y + offset_y, draw_glyph, draw_fg, draw_bg);
	}
}

void WorldRenderer::render_tile_map(RenderParams params) const
{
	auto tile_map = get_tile_map();

	if (!tile_map)
	{
		CERR("No tile map found in renderer.");
		return;
	}

	int draw_x{ 0 }, draw_y{ 0 }, draw_z{ 0 };
	auto& camera = params.camera;

	int layer = 0;

	int view_left = 0;
	int view_right = tile_map->get_width();

	int view_top = 0;
	int view_bottom = tile_map->get_height();

	if (camera)
	{
		layer = camera->get_posz();

		view_left = std::max(view_left, camera->get_x1());
		view_right = std::min(view_right, camera->get_x2());

		view_top = std::max(view_top, camera->get_y1());
		view_bottom = std::min(view_bottom, camera->get_y2());
	}

	const Tile* draw_tile{};

	glyph_t draw_glyph{};
	color_t draw_fg{};
	color_t draw_bg{};

	for (int x = view_left; x < view_right; ++x) for (int y = view_top; y < view_bottom; ++y)
	{
		draw_tile = tile_map->get_tile(x, y, layer);

		draw_glyph = draw_tile->glyph();
		draw_fg = draw_tile->fg();
		draw_bg = draw_tile->bg();

		if (camera)
		{
			if (!camera->in_view(x, y, layer))
				continue;

			std::tie(draw_x, draw_y, draw_z) = camera->get_screen(x, y, layer).as_tuple();
		}

		if (draw_tile->is_air())
		{
			auto tile_below = tile_map->get_tile(x, y, layer - 1);

			if (tile_below->is_air())
			{
				draw_glyph = 177;
				draw_fg = color::get("light_cyan");
				draw_bg = color::get("black");
			}
			else
			{
				draw_glyph = DISTANT_DOT;
				draw_fg = tile_below->fg();
				draw_bg = tile_below->bg();
			}
		}

		if (params.use_fov && !tile_map->is_visible(x, y, layer))
		{
			if (params.show_explored_tiles && tile_map->is_explored(x, y, layer))
			{
				draw_fg = explored_fg;
			}
			else
			{
				draw_fg = unexplored_fg;
			}
		}

		output::put_rgb(draw_x, draw_y, draw_glyph, draw_fg, draw_bg);
	}
}