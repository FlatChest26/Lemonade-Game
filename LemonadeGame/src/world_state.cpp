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
	m_world->set_time(TimeDate(TimeUnit::from_hours(7.0), 11, 6, 2025));

	// Create Tile Map
	auto tile_map = std::make_unique<TileMap>(m_world.get());
	Length road_width = 10;
	Length sidewalk_width = 2;

	// Create Camera
	m_camera = std::make_unique<Camera>();

	// Create renderer
	m_renderer = std::make_unique<WorldRenderer>(this);

	// Create message log
	m_log = std::make_unique<GameLog>(this);

	// Create sidebar
	m_sidebar = std::make_unique<Sidebar>(this);

	// Create Creatures

	auto Player = EntityBuilder::create_creature(
		"ENTITY_PLAYER",
		{
			.soul = new CreatureSoul{
				.ID = "PLAYER",
				.name = {"Player", "Lemonade Person"},
				.gender = gender::NON_SPECIFIC,
				.description = "That is you."
			},
			.species = RawMaster::get<Species>("HUMAN"),
			.lifespan = static_cast<Age::age_t>(INFINITY),
			.current_age = 18,
			.body_mods = { RawMaster::get<BodyMod>("GirlCock")}
		}
	);

	m_player = m_world->add_entity_at(std::unique_ptr<Entity>(Player), tile_map->get_width() - (road_width + sidewalk_width) - 5, 0, 0);
	m_player->learn_name();


	auto Dildo = EntityBuilder::create_item(
		"ENTITY_DILDO", 
		{
			.item_definition = RawMaster::get<ItemDefinition>("DILDO")
		}
	);

	m_world->add_entity_at(std::unique_ptr<Entity>(Dildo), tile_map->get_width() - (road_width + sidewalk_width) - 5, 1, 0);

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
			"^.......#.......#",
			"^.......#.......#",
			"^..^^^^^##=====##",
			" u u      _____  ",
			" uuu uu uu_____  ",
			"   uuu uuu_____  ",
			"          _____  ",
			"          __________________",
			"          __________________",
			"          __________________",
			"          __________________",
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
		tile_map.get(), house_layout_floor1, true, Position(10, 4, 0)
	);
	MapBuilder::make_from_ascii_map(
		tile_map.get(), house_layout_floor2, true, Position(10, 4, 1)
	);
	MapBuilder::make_from_ascii_map(
		tile_map.get(), house_layout_roof, true, Position(10, 4, 2)
	);

	// Create AI Manger
	m_ai_manager = std::make_unique<AIManager>(this);

	m_ai_manager->set_sidewalk_start(Position(tile_map->get_width() - (road_width + sidewalk_width), 0, 0));
	m_ai_manager->set_sidewalk_end(Position(tile_map->get_width() - (road_width + sidewalk_width), tile_map->get_height() - 1, 0));
	m_ai_manager->set_sidewalk_width(sidewalk_width);

	// Add AI Brains
	for (const auto& entity : m_world->get_entities(
		EntityFilter
		{ 
			.ignore_entities = { m_player }, 
			.required_flags = EntityFlag::ANIMATE
		}
	))
	{
		entity->add_brain(m_ai_manager->create_brain());
	}

	m_world->set_tile_map(std::move(tile_map));

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
	Coord console_width = output::get_console_width();
	Coord tile_map_width = get_world()->get_tile_map()->get_width();

	Coord console_height = output::get_console_height();
	Coord tile_map_height = get_world()->get_tile_map()->get_height();

	auto params = RenderParams
	{
		.camera = m_camera.get(),
		.world_render_frame = { Vec2D<Coord>(0, 0), Vec2D<Length>(tile_map_width + 2, tile_map_height + 2), 1},
		.use_fov = m_player != nullptr,
		.show_explored_tiles = get_game_settings().show_explored_tiles,
		.render_world = true,
		.render_entities = true,
		.render_tile_map = true,
		.sidebar_position = UIAttachPosition::RIGHT,
		.gamelog_position = UIAttachPosition::BOTTOM,
	};

	if (m_renderer)
	{
		m_renderer->render(params);
	}
	else
	{
		CERR("No renderer attatched to world state!");
	}

	if (m_log)
	{
		m_log->render(params);
	}
	else
	{
		CERR("No game log attatched to world state!");
	}

	if (m_sidebar)
	{
		m_sidebar->render(params);
	}
	else
	{
		CERR("No side bar attatched to world state!");
	}

	for (const auto& input_handler : std::views::reverse(m_input_handlers))
	{
		input_handler->render(params);
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

	update_camera();

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

void WorldGameState::update_camera()
{
	if (!m_camera)
	{
		return;
	}

	auto game_settings = get_game_settings();

	if (game_settings.fixed_camera || !m_player)
	{
		Position focus = Position(0, 0, 0);

		if (m_player)
		{
			focus.z = m_player->get_movable()->posz();
		}

		if (m_camera_focus)
		{
			focus.z = m_camera_focus->z;
		}

		m_camera->set_position(focus);
		m_camera->set_size(
			Size
			{
				m_world->get_tile_map()->get_width(),
				m_world->get_tile_map()->get_height(),
				CAMERA_VIEW_DEPTH
			}
		);
	}
	else
	{
		Position focus = Position(m_player->get_movable()->pos());

		if (m_camera_focus)
		{
			focus = *m_camera_focus;
		}

		m_camera->set_center(focus);
		m_camera->set_size(
			Size
			{
				(Length)floor(output::get_console_width()),
				(Length)floor(output::get_console_height()),
				CAMERA_VIEW_DEPTH
			}
		);

	}
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

	auto action_result = action->perform();

	if (action_result.response != "")
	{
		game_message(GameMessage(action_result.response));
	}

	if (!action_result.success)
		return;

	while (action_result.has_next_action())
	{
		auto next_action = action_result.get_next_action();

		if (action->has_actor())
		{
			auto actor = action->get_actor();

			if (actor->can_queue_action())
			{
				if (actor == get_player())
				{
					handle_input_result(InputResult(std::move(next_action)));
				}
				else
				{
					actor->queue_action(std::move(next_action));
				}
			}
		}
		else
		{
			handle_input_result(InputResult(std::move(next_action)));
		}
	}
}

void WorldGameState::handle_input_result(InputResult result)
{
	bool can_instant_act = get_game_settings().instant_actions || get_tick_speed() == TickSpeed::TURN_BASED;

	if (result.type == InputResult::Type::Action)
	{
		if (result.next_action && result.next_action->check_action())
		{
			std::unique_ptr<Action> new_action = std::move(result.next_action);

			if (new_action->has_actor())
			{
				auto actor = new_action->get_actor();
				auto action_length = new_action->get_action_length();

				if (actor == m_player && !can_instant_act)
				{
					actor->start_action(std::move(new_action));
				}
				else
				{
					actor->queue_action(std::move(new_action));
				}

				if (actor == m_player && can_instant_act)
				{
					step(action_length);
				}
			}
			else
			{
				auto action_length = new_action->get_action_length();
				step(action_length);
				handle_action(std::move(new_action));
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

void WorldGameState::game_message(GameMessage msg)
{
	if (msg.text.empty())
		return;

	if (!m_log)
	{
		CERR("No game log attatched to world state!");
		return;
	}

	GameMessage new_message(msg.text, msg.fg, msg.bg, get_world()->get_time().total_time());

	m_log->add_message(new_message);
	std::cout << new_message.text << std::endl;

	if (auto game = m_game)
	{
		game->request_screen_update();
	}
}