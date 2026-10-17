#include "world_state.h"
#include "tile_map.h"
#include "tile_type.h"
#include "entity_builder.h"
#include "game.h"
#include "transforms.h"
#include "game_states.h"

#include "raw_master.h"

#include "player_input_handler.h"

#include "map_builder.h"

#include <utility>

WorldGameState::WorldGameState() :
	m_player(nullptr)
{
}

void WorldGameState::new_game()
{
	// Create World
	m_world = std::make_unique<World>(this);
	m_world->set_time(TimeDate{ .time_point = TimeUnit::from_hours(7.0), .day = 11, .month = 6, .year = 2025 });

	// Create Tile Map
	auto tile_map = std::make_unique<TileMap>(m_world.get());
	Length road_width = 10;
	Length sidewalk_width = 2;

	// Create Camera
	m_camera = std::make_unique<Camera>();

	// Create renderer

	m_renderer = std::make_unique<WorldRenderer>(this);

	// Create Creatures

	auto Player = EntityBuilder::create_creature
	({
		.soul = new CreatureSoul({
			.ID = "PLAYER",
			.name = {"Player", "Lemonade Person"},
			.gender = gender::NON_SPECIFIC,
			.description = "That is you."
		}),
		.species = RawMaster::get<Species>("HUMAN"),
		.lifespan = static_cast<Age::age_t>(INFINITY),
		.current_age = 18,
		.body_mods = { RawMaster::get<BodyMod>("GirlCock")}
	});

	m_player = m_world->add_entity_at(std::unique_ptr<Entity>(Player), tile_map->get_width() - (road_width + sidewalk_width) - 5, 0, 0);
	m_player->learn_name();

	// Build Map
	MapBuilder::set_floor(tile_map.get(), RawMaster::get<TileType>("TILE_GRASS"), 0);

	MapBuilder::make_building(
		tile_map.get(),
		Position(tile_map->get_width() - (road_width + sidewalk_width), 0, 0),
		Size(road_width + sidewalk_width, tile_map->get_height(), 1),
		RawMaster::get<TileType>("TILE_SIDEWALK")
	);

	MapBuilder::make_building(
		tile_map.get(),
		Position(tile_map->get_width() - (road_width), 0, 0),
		Size(road_width, tile_map->get_height(), 1),
		RawMaster::get<TileType>("TILE_ROAD")
	);

	// Add house
	MapBuilder::ASCIIMap house_layout_floor1{
		{
			"#######ooo/o#####",
			"#............#<<#",
			"#............#<<#",
			"#............#..#",
			"#............#..#",
			"##..##########..#",
			"#...............#",
			"#...............#",
			"#.......###+##..#",
			"#.......#....#..#",
			"#.......#....#..#",
			"#.......#....#..#",
			"#.......#######+#",
			"#.......#.......#",
			"##+#[[[##.......#",
			" .......#.......#",
			" .......#.......#",
			"        ##=====##",
		}
	};

	MapBuilder::ASCIIMap house_layout_floor2{
		{
			"VVVVVVVVVVVVV#[[#",
			"VVVVVVVVVVVVV[>>[",
			"##########[[[#>>#",
			"#.......#.......[",
			"#.......+.......[",
			"#.......#.......#",
			"#########.......[",
			"#....#..........[",
			"[....#..........#",
			"[....#..#####+###",
			"#....+..#.......#",
			"##+######.......#",
			"#..#....+.......#",
			"#..#....#.......#",
			"##########[[#[[##",
			"VVVVVVVVVVVVVVVVV",
			"VVVVVVVVVVVVVVVVV",
			"        VVVVVVVVV",
		}
	};

	MapBuilder::ASCIIMap house_layout_roof{
		{
			"             VVVV",
			"             VVVV",
			"VVVVVVVVVVVVVVVVV",
			"VVVVVVVVVVVVVVVVV",
			"VVVVVVVVVVVVVVVVV",
			"VVVVVVVVVVVVVVVVV",
			"VVVVVVVVVVVVVVVVV",
			"VVVVVVVVVVVVVVVVV",
			"VVVVVVVVVVVVVVVVV",
			"VVVVVVVVVVVVVVVVV",
			"VVVVVVVVVVVVVVVVV",
			"VVVVVVVVVVVVVVVVV",
			"VVVVVVVVVVVVVVVVV",
			"VVVVVVVVVVVVVVVVV",
			"VVVVVVVVVVVVVVVVV",
			"                 ",
			"                 ",
			"                 ",
		}
	};

	MapBuilder::make_from_ascii_map(
		tile_map.get(), house_layout_floor1, true, Position(10, 5, 0)
	);
	MapBuilder::make_from_ascii_map(
		tile_map.get(), house_layout_floor2, true, Position(10, 5, 1)
	);
	MapBuilder::make_from_ascii_map(
		tile_map.get(), house_layout_roof, true, Position(10, 5, 2)
	);

	// Create AI Manger
	m_ai_manager = std::make_unique<AIManager>(this);
	m_ai_manager->set_sidewalk_start(Position(tile_map->get_width() - (road_width + sidewalk_width), 0, 0));
	m_ai_manager->set_sidewalk_end(Position(tile_map->get_width() - (road_width + sidewalk_width), tile_map->get_height() - 2, 0));
	m_ai_manager->set_sidewalk_width(sidewalk_width);

	for (const auto& entity : m_world->get_entities())
	{
		if (!entity->is_animate())
			continue;

		if (entity == get_player())
			continue;

		m_ai_manager->add_brain_to_entity(entity);
	}

	m_world->set_tile_map(std::move(tile_map));

	// Update things
	m_camera_focus = Position(m_world->get_tile_map()->get_width() / 2, m_world->get_tile_map()->get_height() / 2, m_player->get_movable()->posz());

	// Create Input Handler
	handle_input_result(InputResult{ new PlayerInputHandler() });
}

void WorldGameState::update(double delta_time)
{
	TimeUnit base_speed = TimeUnit::from_seconds(delta_time * 6);
	int step_count = 0;

	switch (m_tick_speed)
	{
	case TickSpeed::REAL_TIME:
	{
		step_count = 1;
		break;
	}
	case TickSpeed::REAL_TIME_2X:
	{
		step_count = 2;
		break;
	}
	case TickSpeed::REAL_TIME_4X:
	{
		step_count = 4;
		break;
	}
	case TickSpeed::REAL_TIME_8X:
	{
		step_count = 8;
		break;
	}
	}

	while (step_count > 0)
	{
		step(base_speed);
		step_count--;
	}

	for (auto& entity : m_world->get_entities())
	{
		auto prev = entity->is_flashing();
		entity->set_flag_to(EntityFlag::IS_FLASHING, entity->should_flash());

		if (prev != entity->is_flashing())
		{
			if (auto game = get_game()) game->request_screen_update();
		}
	}
}

#include <ranges>

void WorldGameState::render() const
{
	if (auto renderer = get_renderer())
	{
		auto params = RenderParams{
			.camera = m_camera.get(),
			.use_fov = m_player != nullptr,
			.show_explored_tiles = get_game_settings().show_explored_tiles,
			.render_world = true,
			.render_entities = true,
			.render_tile_map = true,
		};

		renderer->render(params);

		for (auto& input_handler : std::views::reverse(m_input_handlers))
		{
			input_handler->render(params);
		}
	}
}

void WorldGameState::step(TimeUnit time_delta)
{
	ASSERT(m_world, "WorldGameState::step() called but world is null");

	m_world->step(time_delta);

	if (m_ai_manager)
	{
		m_ai_manager->step(time_delta);
	}

	if (m_player->should_remove())
	{
		m_player = nullptr;
	}

	auto game_settings = get_game_settings();

	if (game_settings.fixed_camera)
	{
		update_camera(Position(m_camera_focus.x, m_camera_focus.y, m_player ? m_player->get_movable()->posz() : m_camera_focus.z));
	}
	else
	{
		update_camera(m_player ? static_cast<Position>(m_player->get_movable()->pos()) : m_camera_focus);
	}

	if (m_player)
	{
		// Update FOV for player
		if (m_player->has_field_of_view())
		{
			if (auto tile_map = m_world->get_tile_map())
			{
				tile_map->compute_fov(m_player->get_movable()->posx(), m_player->get_movable()->posy(), m_player->get_movable()->posz(), m_player->view_range());
			}
		}
	}

	for (auto& input_handler : m_input_handlers)
	{
		input_handler->step(time_delta);
	}

	if (auto game = m_game)
	{
		game->request_screen_update();
	}

	//COUT("Simulation stepped by " << time_delta.total_seconds() << " seconds");
}

void WorldGameState::update_camera(const Position& pos)
{
	if (!m_camera)
	{
		return;
	}

	m_camera->set_position(pos);

	m_camera->set_size(
		Size
		{
			(uint32_t)floor(output::get_console_width() * 0.7),
			(uint32_t)floor(output::get_console_height() * 0.7),
			CAMERA_VIEW_DEPTH
		}
	);
}

#include "snowy_macros.h"

void WorldGameState::handle_action(std::unique_ptr<Action> action)
{
	if (action == nullptr)
	{
		return;
	}

	if (!action->check_action())
	{
		return;
	}

	auto actor = action->get_actor();
	auto action_result = action->perform();

	if (action_result.response != "")
	{
		game_message(action_result.response);
	}

	if (actor && action_result.success)
	{
		while (action_result.has_next_action())
		{
			if (actor->can_queue_action())
			{
				if (actor == get_player())
				{
					handle_input_result(InputResult(std::move(action_result.get_next_action())));
				}
				else
				{
					actor->queue_action(std::move(action_result.get_next_action()));
				}
			}
		}
	}
}

void WorldGameState::handle_input_result(InputResult result)
{
	if (result.type == InputResult::Type::Action /*&& get_run_speed() == runspeed_flag::TURN_BASED*/)
	{
		if (result.next_action && result.next_action->check_action())
		{
			std::unique_ptr<Action> new_action = std::move(result.next_action);

			auto actor = new_action->get_actor();
			auto action_length = new_action->get_action_length();
			actor->queue_action(std::move(new_action));

			if (actor == m_player)
			{
				step(action_length);
			}
		}
		else
		{
			step(TimeUnit::none());
		}
	}
	else
	{
		GameState::handle_input_result(std::move(result));
		step(TimeUnit::none());
	}
}

void WorldGameState::game_message(std::string msg)
{
	if (m_last_msg == msg)
	{
		return;
	}

	m_last_msg = msg;

	std::cout << msg << std::endl;

	if (auto game = m_game)
	{
		game->request_screen_update();
	}
}