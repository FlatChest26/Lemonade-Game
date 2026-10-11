#include "world_state.h"
#include "tile_map.h"
#include "tile_type.h"
#include "creature.h"
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

	// Create Camera
	m_camera = std::make_unique<Camera>();

	// Create renderer

	m_renderer = std::make_unique<WorldRenderer>(this);

	// Create Creatures

	/*
	auto SnowSoul = new CreatureSoul(CreatureSoul::initializer{
		.ID = "SNOW",
		.name = {"Snow", "Snow Oaks"},
		.age = 21,
		.gender = gender::FEMBOY,
		.sexuality = sexuality::PANSEXUAL,
		.sexual_position = SexualPosition::SUB_SWITCH,
		.description = "He is the cutest boy."
	});

	auto AdelineSoul = new CreatureSoul(CreatureSoul::initializer{
		.ID = "ADELINE",
		.name = {"Adeline", "Adeline Baudelaire"},
		.age = 22,
		.gender = gender::FEMININE,
		.sexuality = sexuality::PANSEXUAL,
		.sexual_position = SexualPosition::DOM_SWITCH,
		.description = "She is the most devious succubus."
	});

	auto Snow = new Creature(Creature::initializer{
		.soul = SnowSoul,
		.species = human,
		.body_mods = { body_mod::MaleParts },
		.transform = { Position(7, 4, 0), DEFAULT_SIZE, NORTH },
	});

	auto Adeline = new Creature(Creature::initializer{
		.soul = AdelineSoul,
		.species = demon,
		.body_mods = { body_mod::FemaleParts },
		.transform = { Position(8, 4, 0), DEFAULT_SIZE, NORTH },
	});

	m_world->add_entity(std::unique_ptr<Creature>(Adeline));
	m_world->add_entity(std::unique_ptr<Creature>(Snow));

	*/

	auto PlayerSoul = new CreatureSoul(CreatureSoul::initializer{
		.ID = "PLAYER",
		.name = {"Player", "Lemonade Person"},
		.age = 24,
		.gender = gender::NON_SPECIFIC,
		.sexuality = sexuality::PANSEXUAL,
		.sexual_position = SexualPosition::VERSE_SWITCH,
		.description = "That is you."
		});

	auto Player = new Creature(Creature::initializer{
		.soul = PlayerSoul,
		.species = RawMaster::get_species("HUMAN"),
		.body_mods = { body_mod::GirlCock },
		.transform = { Position(0, 0, 0), DEFAULT_SIZE, NORTH },
		});

	m_player = m_world->add_entity(std::unique_ptr<Creature>(Player));

	// Create Tile Map
	auto tile_map = std::make_unique<TileMap>(m_world.get());

	// Build Map
	MapBuilder::set_floor(tile_map.get(), RawMaster::get_tile_type("TILE_GRASS"), 0);

	// Add road
	Length road_width = 6;
	Length sidewalk_width = 2;

	MapBuilder::make_building(
		tile_map.get(),
		Position(0, tile_map->get_height() - (road_width + sidewalk_width), 0),
		Size(tile_map->get_width(), road_width + sidewalk_width, 1),
		RawMaster::get_tile_type("TILE_SIDEWALK")
	);

	MapBuilder::make_building(
		tile_map.get(),
		Position(0, tile_map->get_height() - (road_width), 0),
		Size(tile_map->get_width(), road_width, 1),
		RawMaster::get_tile_type("TILE_ROAD")
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
			"##+#ooo##.......#",
			" .......#.......#",
			" .......#.......#",
			"        ##=====##",
		}
	};

	MapBuilder::ASCIIMap house_layout_floor2{
		{
			"VVVVVVVVVVVVV#oo#",
			"VVVVVVVVVVVVVo>>#",
			"##########ooo#>>#",
			"#.......#.......#",
			"#.......+.......#",
			"#.......#.......o",
			"#########.......o",
			"#....#..........o",
			"#....#..........#",
			"#....#..#####+###",
			"#....+..#.......#",
			"##+######.......#",
			"#..#....+.......#",
			"#..#....#.......#",
			"##########oo#oo##",
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
	m_camera_focus = Position(m_world->get_tile_map()->get_width() / 2, m_world->get_tile_map()->get_height() / 2, m_player->posz());

	// Create Input Handler
	handle_input_result(InputResult{ new PlayerInputHandler() });
}

void WorldGameState::update(double delta_time)
{
	if (m_tick_speed == TickSpeed::TURN_BASED)
		return;

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

	auto game_settings = get_game_settings();

	if (game_settings.fixed_camera)
	{
		update_camera(Position(m_camera_focus.x, m_camera_focus.y, m_player ? m_player->posz() : m_camera_focus.z));
	}
	else
	{
		update_camera(m_player ? m_player->pos() : m_camera_focus);
	}

	if (m_player)
	{
		// Update FOV for player
		if (m_player->has_field_of_view())
		{
			if (auto tile_map = m_world->get_tile_map())
			{
				tile_map->compute_fov(m_player->posx(), m_player->posy(), m_player->posz(), m_player->view_range());
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
		if (result.next_action)
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