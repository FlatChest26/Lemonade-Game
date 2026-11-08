#include "world_state.h"
#include "tile_map.h"
#include "tile_type.h"
#include "entity_builder.h"
#include "game.h"
#include "transforms.h"
#include "game_states.h"

#include "debug_info.h"

#include "raw_master.h"

#include "player_input_handler.h"

#include "map_builder.h"

#include <utility>

void WorldGameState::new_game()
{
	// Create World
	m_world = std::make_unique<World>(this);
	m_world->set_time(TimeDate(TimeUnit::from_hours(7.0), 11, 6, 2025));

	// Create Tile Map
	auto tile_map = new TileMap(m_world.get());
	m_world->set_tile_map(tile_map);

	Length road_width = 10;
	Length sidewalk_width = 2;

	Position lemonade_stand_pos = Position(tile_map->get_width() - (road_width + sidewalk_width) - 8, 23, 0);

	// Create Camera
	m_camera = std::make_unique<Camera>();

	// Create renderer
	m_renderer = std::make_unique<WorldRenderer>(this);

	// Create message log
	m_log = std::make_unique<GameLog>(this);

	// Create sidebar
	m_sidebar = std::make_unique<Sidebar>(this);

	// Create Creatures

	m_player_id = m_world->add_entity_at(
		EntityBuilder::create_creature(
			"PLAYER",
			{
				.soul = new CreatureSoul
				{
					.name = { "Player", "Lemonade Person" },
					.gender = RawMaster::get<Gender>("NEUTRAL"),
					.description = "That is you."
				},
				.species = RawMaster::get<Species>("HUMAN"),
				.lifespan = static_cast<Age::age_t>(INFINITY),
				.current_age = 20,
				.body_mods = { RawMaster::get<BodyMod>("GirlCock") },
				.starting_cash = Currency::from_dollars(100),
				.initial_stats = {
					{ needs::AROUSAL, 3 },
					{ needs::HUNGER, 13 },
					{ needs::HYGIENE, 18 },
					{ needs::FATIGUE, 12 },
					{ needs::MOOD, 7 },
				}
			}
		), 
		lemonade_stand_pos + Position(-1, 1, 0)
	)->ID();

	get_player()->learn_name();

	m_world->add_entity_at(
		EntityBuilder::create_item(
			"DILDO_1", { .item_definition = RawMaster::get<ItemDefinition>("DILDO") }
		),
		lemonade_stand_pos + Position(-1, 2, 0)
	);

	m_world->add_entity_at(
		EntityBuilder::create_item(
			"LEMONADE_GLASS_1", { .item_definition = RawMaster::get<ItemDefinition>("LEMONADE_GLASS") }
		),
		lemonade_stand_pos + Position(-1, 0, 0)
	);

	m_world->add_entity_at(
		EntityBuilder::create_item(
			"KNIFE_1", { .item_definition = RawMaster::get<ItemDefinition>("KNIFE") }
		),
		lemonade_stand_pos + Position(-2, 1, 0)
	);

	m_world->add_entity_at(
		EntityBuilder::create_furniture(
			"LEMONADE_STAND_1", { .furniture_definition = RawMaster::get<FurnitureDefinition>("LEMONADE_STAND") }
		),
		lemonade_stand_pos, 
		EAST
	);

	auto Snow = m_world->add_entity_at(
		EntityBuilder::create_creature(
			"SNOW",
			{
				.soul = new CreatureSoul
				{
					.name = { "Snow", "Snow Oaks" },
					.gender = RawMaster::get<Gender>("MASCULINE"),
					.description = "The cutest boy."
				},
				.species = RawMaster::get<Species>("HUMAN"),
				.lifespan = static_cast<Age::age_t>(INFINITY),
				.current_age = 20,
				.body_mods = { RawMaster::get<BodyMod>("MaleParts") },
				.starting_cash = Currency::from_dollars(250),
				.initial_stats = {
					{ needs::AROUSAL, 3 },
					{ needs::HUNGER, 73 },
					{ needs::HYGIENE, 18 },
					{ needs::FATIGUE, 12 },
					{ needs::MOOD, 7 },
				}
			}
		), 
		lemonade_stand_pos + Position(4, 1, 0)
	);

	Snow->learn_name();

	// Build Map
	MapBuilder::set_floor(tile_map, RawMaster::get<TileType>("TILE_GRASS"), 0);

	MapBuilder::make_building(
		tile_map,
		Position(tile_map->get_width() - (road_width + sidewalk_width), 0, 0),
		Size(road_width + sidewalk_width, tile_map->get_height(), 1),
		RawMaster::get<TileType>("TILE_SIDEWALK")
	);

	MapBuilder::make_building(
		tile_map,
		Position(tile_map->get_width() - (road_width), 0, 0),
		Size(road_width, tile_map->get_height(), 1),
		RawMaster::get<TileType>("TILE_ROAD")
	);


	TileMapping tile_mapping
	{
		{ ' ',{ "TILE_AIR" } },
		{ ',',{ "TILE_GRASS" } },
		{ 'u',{ "TILE_GRAVEL" } },
		{ '.',{ "TILE_PLAIN_FLOOR" } },
		{ '#',{ "TILE_PLAIN_WALL" } },
		{ '+',{ "TILE_DOOR" } },
		{ '[',{ "TILE_WINDOW" } },
		{ 'o',{ "TILE_GLASS" } },
		{ '<',{ "TILE_UPWARD_STAIRCASE" } },
		{ '>',{ "TILE_DOWNWARD_STAIRCASE" } },
		{ 'X',{ "TILE_UP_AND_DOWN_STAIRCASE" } },
		{ '=',{ "TILE_GARAGE_DOOR" } },
		{ '^',{ "TILE_FENCE" } },
		{ '/',{ "TILE_GLASS_DOOR" } },
		{ 'V',{ "TILE_SLANTED_ROOF" } },
		{ '_',{ "TILE_SIDEWALK" } },
		{ 's',{ "TILE_ROAD" } },
		{ 'B',{ "TILE_PLAIN_FLOOR", "BED" } },
		{ 'L',{ "TILE_GRASS", "LEMONADE_STAND" } },

	};

	// Add house
	MapBuilder::LayeredASCIIMap house_layout
	{
		tile_mapping,
		{
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
				"^^..^^^^##=====##",
				"  u u     _____             ",
				"  uu uu uu_____             ",
				"   uuu uuu_____             ",
				"          _____             ",
				"          __________________",
				"          __________________",
				"          __________________",
				"          __________________",
			},
			{
				"VVVVVVVVVVVVV#[[#",
				"VVVVVVVVVVVVV[>>[",
				"##########[[[#>>#",
				"#.......#.......[",
				"#.......+.......[",
				"#.......#.......#",
				"#########.......[",
				"#..B.#..........[",
				"[....#..........#",
				"[....#..#####+###",
				"#....+..#.......#",
				"##+######......B#",
				"#..#....+......B#",
				"#..#....#.......#",
				"##########[[#[[##",
				"VVVVVVVVVVVVVVVVV",
				"VVVVVVVVVVVVVVVVV",
				"        VVVVVVVVV",
			},
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
			}
		}
	};

	MapBuilder::make_from_layered_ascii_map(
		m_world.get(), house_layout, true, Position(10, 4, 0)
	);

	// Create AI Manger
	m_ai_manager = std::make_unique<AIManager>(this);

	m_ai_manager->set_sidewalk_start(Position(tile_map->get_width() - (road_width + sidewalk_width), 0, 0));
	m_ai_manager->set_sidewalk_end(Position(tile_map->get_width() - (road_width + sidewalk_width), tile_map->get_height() - 1, 0));
	m_ai_manager->set_sidewalk_width(sidewalk_width);

	m_ai_manager->set_spawn_npcs(true);

	// Material

	//tile_map->add_material(lemonade_stand_pos - Position(4, 0, 0), RawMaster::get<MaterialType>("lemonade"), 4);
	tile_map->add_material(lemonade_stand_pos - Position(4, -3, 0), RawMaster::get<MaterialType>("blood"), 7);

	// Add AI Brains
	for (auto& entity : m_world->get_entities(
		EntityFilter
		{
			.ignore_ids = { m_player_id },
			.required_flags = EntityFlag::IS_CREATURE
		}
	))
	{
		entity->set_brain(m_ai_manager->create_brain());
	}

	m_previous_player_pos = Position(-1, -1, -1);

	// Create Input Handler
	handle_input_result(InputResult(new InputHandler_Player()));

	// Update camera size

	m_camera->set_size(
		Size
		{
			m_world->get_tile_map()->get_width(),
			m_world->get_tile_map()->get_height(),
			CAMERA_VIEW_DEPTH
		}
	);
}

void WorldGameState::update(double delta_time)
{
	TimeUnit time_progress = TimeUnit::from_seconds(delta_time * 6.0f);

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

	if ((m_tick_speed == TickSpeed::TURN_BASED || GameSettings::INSTANT_ACTIONS) && does_player_exist() && get_player()->has_current_action())
	{
		auto current_action = get_player()->get_current_action();
		step(current_action->remaining_action_length());
	}
	else
	{
		while (step_count > 0)
		{
			step(time_progress);
			step_count--;
		}
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
		.world_render_frame = { Vec2D<Coord>(0, 0), Vec2D<Length>(tile_map_width + 2, tile_map_height + 2), 1 },
		.use_fov = m_world->has_entity(m_player_id),
		.show_explored_tiles = GameSettings::SHOW_EXPLORED_TILES,
		.render_world = true,
		.render_entities = true,
		.render_tile_map = true,
		.render_debug = true,
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

void WorldGameState::step(const TimeUnit& time_delta)
{
	ASSERT(m_world, "WorldGameState::step() called but world is null");

	if (time_delta > TimeUnit::none())
	{
		TimeUnit elloted_time = time_delta;

		while (elloted_time > TimeUnit::none())
		{
			TimeUnit step_time = std::min(elloted_time, TimeUnit::from_seconds(1.0));

			m_world->step(step_time);

			if (m_ai_manager)
			{
				m_ai_manager->step(step_time);
			}

			elloted_time -= step_time;
		}
	}
	else
	{
		m_world->step(time_delta);

		if (m_ai_manager)
		{
			m_ai_manager->step(time_delta);
		}
	}

	auto tile_map = m_world->get_tile_map();

	if (tile_map && tile_map->should_update_path())
	{
		tile_map->set_update_path(false);
	}

	update_camera();

	if (does_player_exist() && tile_map)
	{
		auto player = get_player();
		auto player_pos = player->get_movable()->pos();

		if (m_previous_player_pos != player_pos)
		{
			tile_map->request_fov_update();
			m_previous_player_pos = player_pos;
		}

		// Update FOV for player
		if (player->has_field_of_view() && tile_map->should_update_fov())
		{
			tile_map->compute_fov(
				player_pos.x, player_pos.y, player_pos.z,
				player->view_range()
			);
		}
	}

	for (auto& input_handler : m_input_handlers)
	{
		input_handler->step(time_delta);
	}

	if (auto game = m_game)
	{
		m_game->request_screen_update();
	}

	//COUT("Simulation stepped by " << time_delta.total_seconds() << " seconds");
}

void WorldGameState::update_camera()
{
	if (!m_camera)
	{
		return;
	}
	
	Position focus{ 0, 0, 0 };

	if (m_has_camera_focus)
	{
		focus = m_camera_focus;
	}
	else if (does_player_exist())
	{
		focus = get_player()->get_movable()->pos();
	}

	if (GameSettings::FIXED_CAMERA)
	{
		focus.x = 0;
		focus.y = 0;
	}

	m_camera->set_position(focus);
}

#include "snowy_macros.h"

void WorldGameState::handle_action(QueuedAction action)
{
	if (!action || !action->check_action())
	{
		return;
	}

	auto action_result = action->perform();

	if (!action_result.responses.empty())
	{
		for (const auto& response : action_result.responses)
		{
			game_message(response);
		}
	}

	if (!action_result.success)
	{
		return;
	}

	if (action_result.next)
	{
		if (action->has_actor())
		{
			auto actor = action->get_actor();
			actor->start_action(action_result.next);
		}
		else
		{
			handle_input_result(InputResult(action_result.next));
		}
	}
}

void WorldGameState::handle_input_result(InputResult result)
{
	if (result.type == InputResult::Type::Action && result.next_action && result.next_action->check_action())
	{
		if (result.next_action->has_actor())
		{
			auto actor = result.next_action->get_actor();
			auto action_length = result.next_action->get_action_length();
			actor->start_action(std::move(result.next_action));
		}
		else
		{
			auto action_length = result.next_action->get_action_length();
			step(action_length);

			handle_action(std::move(result.next_action));
		}
	}
	else
	{
		if (result.type != InputResult::Type::Action)
		{
			GameState::handle_input_result(std::move(result));
		}

		step(TimeUnit::none());
	}
}

void WorldGameState::game_message(const GameMessage& msg)
{
	if (msg.text.empty())
		return;

	if (!m_log)
	{
		CERR("No game log attatched to world state!");
		return;
	}

	m_log->add_message(msg, get_world()->get_time().total_time());
	std::cout << msg.text << std::endl;

	if (auto game = m_game)
	{
		game->request_screen_update();
	}
}