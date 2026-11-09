#include "player_input_handler.h"

#include "action.h"
#include "direction_action.h"
#include "location_actions.h"
#include "interactions.h"
#include "special_actions.h"

#include "game.h"

#include "world_state.h"

#include "tile_map.h"

#include "render_params.h"

#include <SDL_keycode.h>
#include <map>

#include "transforms.h"

template<typename T>
T* InputHandler_Player::register_sidebar(const SidebarMenuID& ID)
{
	auto sidebar = get_world_state()->get_sidebar();

	if (!sidebar->has_menu(ID))
	{
		return (T*)sidebar->add_menu(ID, std::make_unique<T>());
	}
	else
	{
		return (T*)sidebar->get_menu(ID);
	}

	return nullptr;
}

bool InputHandler_Player::init()
{
	auto sidebar = get_world_state()->get_sidebar();

	if (!sidebar)
	{
		CERR("No sidebar on world state!");
		return false;
	}


	if (!does_player_exist())
	{
		CERR("No player found!");
		return false;
	}

	auto player = get_player();

	m_game_info_menu = register_sidebar<SidebarMenu_GameInfo>(sidebar_menus::GAME_INFO);

	m_person_info_menu = register_sidebar<SidebarMenu_PersonInfo>(sidebar_menus::PERSON_INFO);
	m_person_info_menu->entity = player;
	
	m_needs_menu = register_sidebar<SidebarMenu_Needs>(sidebar_menus::NEEDS);
	m_needs_menu->entity = player;
	

	if (!sidebar->has_menu(sidebar_menus::INVENTORY))
	{
		m_inventory_menu = register_sidebar<SidebarMenu_Inventory>(sidebar_menus::INVENTORY);
		m_inventory_menu->pin();
	}

	m_inventory_menu = register_sidebar<SidebarMenu_Inventory>(sidebar_menus::INVENTORY);
	m_inventory_menu->entity = player;

	m_inventory_menu->collapse();
	

	if (!sidebar->has_menu(sidebar_menus::LOOK))
	{
		m_look_menu = register_sidebar<SidebarMenu_Look>(sidebar_menus::LOOK);
		m_look_menu->hide();
	}

	m_look_menu = register_sidebar<SidebarMenu_Look>(sidebar_menus::LOOK);

	m_look_menu->look_position = player->get_movable()->pos();
	m_look_menu->follow_player = true;
	

	m_examine_menu = register_sidebar<SidebarMenu_Examine>(sidebar_menus::EXAMINE);
	m_examine_menu->set_visible(!m_examine_menu->examined_entities.empty() || m_examine_menu->examined_tile != nullptr);

	return true;
}

InputResult InputHandler_Player::ev_mouse_button_down(SDL_Event event)
{
	if (!GameSettings::MOUSE_MODE)
	{
		GameSettings::MOUSE_MODE = true;
		return InputResult();
	}

	m_mouse_position.x = event.motion.x;
	m_mouse_position.y = event.motion.y;



	return InputResult();
}

InputResult InputHandler_Player::ev_mouse_motion(SDL_Event event)
{
	if (!GameSettings::MOUSE_MODE)
	{
		return InputResult();
	}

	m_mouse_position.x = event.motion.x;
	m_mouse_position.y = event.motion.y;

	return InputResult();
}

InputResult InputHandler_Player::ev_key_down(SDL_Event event)
{
	if (!does_player_exist())
	{
		CERR("No player found.");
		return InputResult();
	}

	auto player = get_player();

	auto world = get_world();

	if (!world)
	{
		CERR("No world found.");
		return InputResult();
	}

	auto world_state = get_world_state();

	if (!world_state)
	{
		CERR("No world state found.");
		return InputResult();
	}

	auto game = get_game();

	if (!game)
	{
		CERR("No game found.");
		return InputResult();
	}

	bool is_alive = false;
	bool is_sleeping = false;

	if (player->has_living())
	{
		is_sleeping = player->get_living()->is_asleep();
		is_alive = player->get_living()->is_alive();
	}

	bool can_act = !is_sleeping && is_alive;

	Position player_pos = player->get_movable()->pos();

	// Check if movement keys are pressed
	Position move_dir = InputMap::get_input_direction(event);

	if (can_act && !move_dir.empty())
	{
		return InputResult(new BumpAction(player, move_dir.unit()));
	}

	// Check action keys

	if (InputMap::is_action_pressed(Keybind::Quit, event))
	{
		game->stop_running();
		return InputResult();
	}
	
	if (can_act && InputMap::is_action_pressed(Keybind::Look, event))
	{
		return InputResult(new InputHandler_Look());
	}
	
	if (can_act && InputMap::is_action_pressed(Keybind::Inventory, event))
	{
		return InputResult(new InputHandler_Inventory());
	}

	if (can_act && InputMap::is_action_pressed(Keybind::Interact, event))
	{
		std::unordered_map<Position, std::vector<Entity*>> interactable_entities;

		for (auto p : transforms::get_surrounding_locations(player_pos, false, false, true))
		{
			for (auto& entity : world->get_entities_at(p, { .ignore_held = true, .ignore_ids = { player->ID() }}))
			{
				if (player->can_interact_with(entity))
				{
					interactable_entities[p].push_back(entity);
				}
			}
		}
		
		if (interactable_entities.empty())
			return InputResult();

		auto position_callback = [=](const Position& pos)
			{
				if (!interactable_entities.contains(pos))
					return InputResult();

				auto& entities = interactable_entities.at(pos);

				if (entities.empty())
					return InputResult();

				if (entities.size() == 1)
				{
					return InputResult(new InputHandler_Interaction({ .actor = player, .subject = entities.front() }));
				}

				return InputResult(new InputHandler_ListSelector(
					{
						.title = "Interact with what?",
						.input_keybind = Keybind::Interact,
						.display_input_keybind = false,
						.items = [&]()
						{
							std::vector<ListSelectorItem> items;
							for (auto& entity : entities)
							{
								items.push_back(ListSelectorItem(
									entity->display_name(), entity->fg(), entity->bg(),
									[=]()
									{
										return InputResult(new InputHandler_Interaction({ .actor = player, .subject = entity }));
									}
								));
							}
							return items;
						}(),
					}
				));
			};

		if (interactable_entities.size() == 1 && !interactable_entities.begin()->second.empty())
		{
			return position_callback(interactable_entities.begin()->first);
		}
		
		return InputResult(new InputHandler_PositionSelector(
			{
				.title = "Interact where?",
				.input_keybind = Keybind::Interact,
				.center = player_pos,
				.valid_positions = [&]()
				{
					std::set<Position> positions;

					for (auto& [pos, entities] : interactable_entities)
					{
						if (entities.empty())
							continue;

						positions.insert(pos);
					}

					return positions;
				}(),

				.callback = position_callback
			}
		));
	};
	
	if (can_act && InputMap::is_action_pressed(Keybind::Open, event))
	{
		std::set<Position> open_positions;
		std::unordered_map<Position, Tile*> added_tiles;

		for (auto p : transforms::get_surrounding_locations(player_pos, false, false, true))
		{
			if (!world->get_tile(p)->can_open())
				continue;

			auto tile = world->get_tile(p);
			added_tiles[p] = tile;

			bool is_connection = false;

			for (auto& [other_pos, other_tile] : added_tiles)
			{
				if (other_tile == tile)
					continue;

				if (transforms::is_directly_adjacent(p, other_pos) && (other_tile->can_connect_with(tile) || tile->can_connect_with(other_tile)))
				{
					is_connection = true;
					break;
				}
			}

			if (!is_connection)
				open_positions.insert(p);
		}

		if (open_positions.size() == 1)
		{
			return InputResult(new OpenAction(player, *open_positions.begin()));
		}
		else if (open_positions.size() > 1)
		{
			return InputResult(new InputHandler_PositionSelector({
				.title = "Open where?",
				.input_keybind = Keybind::Open,
				.center = player_pos,
				.valid_positions = open_positions,
				.callback = [=](const Position& pos)
				{
					return InputResult(new OpenAction(player, pos));
				}
			}));
		}

		return InputResult();
	}

	if (can_act && InputMap::is_action_pressed(Keybind::Close, event))
	{
		std::set<Position> close_positions;
		std::unordered_map<Position, Tile*> added_tiles;

		for (auto p : transforms::get_surrounding_locations(player_pos, false, false, true))
		{
			if (!world->get_tile(p)->can_close())
				continue;

			auto tile = world->get_tile(p);
			added_tiles[p] = tile;

			bool is_connection = false;

			for (auto& [other_pos, other_tile] : added_tiles)
			{
				if (other_tile == tile)
					continue;

				if (transforms::is_directly_adjacent(p, other_pos) && (other_tile->can_connect_with(tile) || tile->can_connect_with(other_tile)))
				{
					is_connection = true;
					break;
				}
			}

			if (!is_connection)
				close_positions.insert(p);
		}

		if (close_positions.size() == 1)
		{
			return InputResult(new CloseAction(player, *close_positions.begin()));
		}
		else if (close_positions.size() > 1)
		{
			return InputResult(new InputHandler_PositionSelector({
				.title = "Close where?",
				.input_keybind = Keybind::Close,
				.center = player_pos,
				.valid_positions = close_positions,
				.callback = [=, &player](const Position& pos)
				{
					return InputResult(new CloseAction(player, pos));
				}
			}));
		}

		return InputResult();
	}
	
	if (InputMap::is_action_pressed(Keybind::Sleep, event))
	{
		if (!player->has_living())
		{
			return InputResult();
		}

		if (!is_sleeping)
		{
			if (!player->get_living()->can_sleep())
			{
				world_state->game_message(GameMessage("You're not tired enough to sleep."));
				return InputResult();
			}

			return InputResult(new StartSleepAction(player));
		}
		else
		{
			if (!player->get_living()->can_wake_up())
			{
				world_state->game_message(GameMessage("You're too tired to wake up."));
				return InputResult();
			}

			return InputResult(new EndSleepAction(player));
		}
	}
	
	if (InputMap::is_action_pressed(Keybind::WaitShort, event))
	{
		return InputResult(new WaitAction(TimeUnit::from_seconds(1.0)));
	}
	
	if (InputMap::is_action_pressed(Keybind::WaitLong, event))
	{
		return InputResult(new WaitAction(TimeUnit::from_seconds(10.0)));
	}
	
	if (can_act && InputMap::is_action_pressed(Keybind::Grab, event))
	{
		auto inventory = player->get_inventory();

		if (!inventory)
		{
			CERR("player has no inventory!");
			return InputResult();
		}

		if (inventory->is_full())
		{
			world_state->game_message(GameMessage("Your inventory is full.", color::get("light_red")));
			return InputResult();
		}

		auto items_at = world->get_entities_at(
			player->get_movable()->pos(), 
			EntityFilter{ 
				.ignore_ids = { player->ID() },
				.required_flags = EntityFlag::CAN_BE_HELD 
			}
		);

		if (items_at.empty())
		{
			return InputResult();
		}

		for (auto& item : items_at)
		{
			if (inventory->can_carry(item))
			{
				return InputResult(new GrabAction(player, item));
			}
		}

		world_state->game_message(GameMessage("You don't have the free space to carry that.", color::get("light_red")));

		return InputResult();
	}

	if (can_act && InputMap::is_action_pressed(Keybind::Grapple, event))
	{
		std::vector<Entity*> potential_grapple_target;

		if (player->is_being_grappled() || player->is_grappling_other())
		{
			return InputResult(new CheckGrappleAction(player));
		}
		else
		{
			for (const auto& p : transforms::get_surrounding_locations(player_pos, false, false, true))
			{
				auto entities_at = world->get_entities_at(p,
					EntityFilter
					{
						.ignore_held = true,
						.ignore_ids = { player->ID() },
						.required_flags = { EntityFlag::CAN_BE_GRAPPLED }
					}
				);

				if (!entities_at.empty())
				{
					potential_grapple_target.insert(potential_grapple_target.end(), entities_at.begin(), entities_at.end());
				}
			}
		}
		
		if (!potential_grapple_target.empty())
		{
			if (potential_grapple_target.size() == 1)
			{
				return InputResult(new CheckGrappleAction(player, potential_grapple_target.front()));
			}
			else
			{
				return InputResult(new InputHandler_ListSelector(
					{
						.title = "Grapple what?",
						.input_keybind = Keybind::Interact,
						.display_input_keybind = false,
						.items = [&]()
						{
							std::vector<ListSelectorItem> items;
							for (auto& entity : potential_grapple_target)
							{
								items.push_back(ListSelectorItem(
									entity->display_name(), entity->fg(), entity->bg(),
									[=]()
									{
										return InputResult(new CheckGrappleAction(player, entity));
									}
								));
							}
							return items;
						}(),
					}
				));

			}
		}
	}

	if (InputMap::is_action_pressed(Keybind::Quit, event))
	{
		game->stop_running();
		return InputResult();
	}

	if (InputMap::is_action_just_pressed(Keybind::SetToTurnBasedSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::TURN_BASED);
		return InputResult();
	}

	if (InputMap::is_action_just_pressed(Keybind::SetToRealTimeSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::REAL_TIME);
		return InputResult();
	}

	if (InputMap::is_action_just_pressed(Keybind::SetToRealTime2xSpeed, event))
	{
		world_state->set_tick_speed(TickSpeed::REAL_TIME_2X);

		return InputResult();
	}

	if (InputMap::is_action_just_pressed(Keybind::SetToRealTime4xSpeed, event))
	{
		if (is_sleeping)
		{
			world_state->set_tick_speed(TickSpeed::REAL_TIME_8X);
		}
		else
		{
			world_state->set_tick_speed(TickSpeed::REAL_TIME_4X);
		}
		return InputResult();
	}

	if (InputMap::is_action_just_pressed(Keybind::ToggleMouseMode, event))
	{
		GameSettings::MOUSE_MODE = !GameSettings::MOUSE_MODE;
		return InputResult();
	}

	if (InputMap::is_action_just_pressed(Keybind::ToggleFullscreen, event))
	{
		output::set_fullscreen(!output::is_fullscreen());
		return InputResult();
	}

#ifdef _DEBUG
	switch (event.key.keysym.sym)
	{
	case SDLK_F5:
	{
		if (player->has_movable())
		{
			COUT(player->get_movable()->pos());
		}
		break;
	}
	case SDLK_F6:
	{
		if (player->has_movable())
		{
			for (auto& entity : world->get_entities(EntityFilter{ .must_be_ai = true }))
			{
				entity->get_brain()->push_state(new AIState_Horny({ .horny_target = player }));
			}
		}
		break;
	}
	}
#endif // _DEBUG
	
	return InputResult();
}

void InputHandler_Player::step(const TimeUnit& delta_time)
{
	if (m_look_menu && does_player_exist() && m_look_menu->follow_player)
	{
		m_look_menu->look_position = get_player()->get_movable()->pos();
	}
}

void InputHandler_Player::render(const RenderParams& params) const
{
	output::draw_frame_rgb(
		params.world_render_frame.pos.x, params.world_render_frame.pos.y,
		params.world_render_frame.pos.x + params.world_render_frame.size.x,
		params.world_render_frame.pos.y + params.world_render_frame.size.y,
		color::get("white"), color::get("black"), DEFAULT_RECT_FRAME, false, TCOD_BKGND_SET
	);

	if (!GameSettings::MOUSE_MODE)
		return;

	if (!params.in_world_frame(m_mouse_position))
		return;

	int draw_x{ m_mouse_position.x }, draw_y{ m_mouse_position.y }, draw_z{ 0 };

	auto draw_fg = output::get_bg(draw_x, draw_y);
	auto draw_bg = output::get_fg(draw_x, draw_y);

	output::set_fg(draw_x, draw_y, draw_fg);
	output::set_bg(draw_x, draw_y, draw_bg);
}

bool InputHandler_Look::init()
{
	if (!InputHandler_Player::init())
		return false;

	m_look_menu->show();
	m_look_menu->focus();
	m_look_menu->follow_player = false;
	get_world_state()->set_camera_focus(m_look_menu->look_position);

	if (m_examine_menu->is_visible())
	{
		m_examine_menu->focus();
	}

	return true;
}

void InputHandler_Look::remove()
{
	if (auto world_state = get_world_state())
	{
		world_state->remove_camera_focus();
	}

	m_examine_menu->examined_entities.clear();
	m_examine_menu->examined_tile = nullptr;

	m_look_menu->follow_player = true;
	m_look_menu->unfocus();
	m_look_menu->hide();
}

InputResult InputHandler_Look::ev_key_down(SDL_Event event)
{
	if (!does_player_exist())
	{
		CERR("Player does not exist!");
		return InputResult();
	}

	auto player = get_player();

	auto world_state = get_world_state();
	auto world = get_world();

	bool is_alive = false;
	bool is_sleeping = false;

	if (player->has_living())
	{
		is_sleeping = player->get_living()->is_asleep();
		is_alive = player->get_living()->is_alive();
	}

	// Check if movement keys are pressed
	Position move_dir = InputMap::get_input_direction(event);

	if (!move_dir.empty())
	{
		m_look_menu->look_position += move_dir.unit();
		world_state->set_camera_focus(m_look_menu->look_position);
		return InputResult();
	}

	if (InputMap::is_action_pressed(Keybind::Look, event) || InputMap::is_action_pressed(Keybind::Quit, event))
	{
		return InputResult(InputResult::Type::Pop_InputHandler);
	}

	if (InputMap::is_action_pressed(Keybind::Examine, event))
	{
		m_examine_menu->examined_entities.clear();
		m_examine_menu->examined_tile = nullptr;
		m_examine_menu->hide();

		for (auto& e : world->get_entities_at(m_look_menu->look_position))
		{
			m_examine_menu->examined_entities.push_back(e);
			m_examine_menu->show();
			m_examine_menu->focus();
		}

		if (m_examine_menu->examined_entities.empty())
		{
			auto tile = world->get_tile(m_look_menu->look_position);

			if (tile)
			{
				m_examine_menu->examined_tile = tile;
				m_examine_menu->show();
				m_examine_menu->focus();
			}
		}

		return InputResult();
	}

	return InputHandler_Player::ev_key_down(event);
}

void InputHandler_Look::render(const RenderParams& params) const
{
	int draw_x{ m_look_menu->look_position.x }, draw_y{ m_look_menu->look_position.y }, draw_z{ m_look_menu->look_position.z };

	if (params.camera)
	{
		if (!params.camera->in_view(m_look_menu->look_position))
			return;

		std::tie(draw_x, draw_y, draw_z) = params.camera->get_screen(m_look_menu->look_position).as_tuple();
	}

	if (draw_z != 0)
		return;

	int screen_x = draw_x + params.world_inner_frame_p1().x;
	int screen_y = draw_y + params.world_inner_frame_p1().y;

	if (!params.in_world_frame(screen_x, screen_y))
		return;

	auto draw_fg = output::get_bg(screen_x, screen_y);
	auto draw_bg = output::get_fg(screen_x, screen_y);

	output::set_fg(screen_x, screen_y, draw_fg);
	output::set_bg(screen_x, screen_y, draw_bg);
}

bool InputHandler_Inventory::init()
{
	if (!InputHandler_Player::init())
		return false;

	auto inventory = get_player()->get_inventory();

	if (!inventory)
	{
		CERR("Player has no inventory!");
		return false;
	}

	m_inventory_menu->selected_slot = std::clamp(m_inventory_menu->selected_slot, 0, (int)inventory->get_max_slots());

	m_inventory_menu->focus();
	m_inventory_menu->uncollapse();

	return true;
}

void InputHandler_Inventory::remove()
{
	m_examine_menu->examined_entities.clear();
	m_examine_menu->examined_tile = nullptr;

	m_inventory_menu->unfocus();
	m_inventory_menu->collapse();
}

InputResult InputHandler_Inventory::ev_key_down(SDL_Event event)
{
	if (!does_player_exist())
	{
		CERR("Player does not exist!");
		return InputResult();
	}

	auto player = get_player();

	auto inventory = player->get_inventory();
	auto world_state = get_world_state();

	bool is_alive = false;
	bool is_sleeping = false;

	if (player->has_living())
	{
		is_sleeping = player->get_living()->is_asleep();
		is_alive = player->get_living()->is_alive();
	}

	if (InputMap::is_action_pressed(Keybind::Inventory, event) || InputMap::is_action_pressed(Keybind::Quit, event))
	{
		return InputResult(InputResult::Type::Pop_InputHandler);
	}
	
	if (InputMap::is_action_pressed(Keybind::MenuUp, event))
	{
		int change_amount = 1;

		if (auto selected_item = inventory->get_item(m_inventory_menu->selected_slot))
		{
			change_amount = (int)selected_item->carry_slots();
		}

		m_inventory_menu->selected_slot += change_amount;
		m_inventory_menu->selected_slot = m_inventory_menu->selected_slot % inventory->get_max_slots();
		return InputResult();
	}
	
	if (InputMap::is_action_pressed(Keybind::MenuDown, event))
	{
		int change_amount = 1;

		if (auto selected_item = inventory->get_item(m_inventory_menu->selected_slot))
		{
			change_amount = (int)selected_item->carry_slots();
		}

		m_inventory_menu->selected_slot -= change_amount;
		m_inventory_menu->selected_slot = m_inventory_menu->selected_slot % inventory->get_max_slots();

		return InputResult();
	}

	if (!is_sleeping && is_alive && InputMap::is_action_pressed(Keybind::Interact, event))
	{
		if (auto selected_item = inventory->get_item(m_inventory_menu->selected_slot))
		{
			return InputResult(InputResult::Type::Pop_InputHandler, new InputResult(new InputHandler_Interaction({ player, selected_item })));
		}

		return InputResult();
	}
	
	if (!is_sleeping && is_alive && InputMap::is_action_pressed(Keybind::Drop, event))
	{
		if (auto selected_item = inventory->get_item(m_inventory_menu->selected_slot))
		{
			return InputResult(new DropAction(player, selected_item), new InputResult(InputResult::Type::Pop_InputHandler));
		}

		return InputResult();
	}
	
	if (InputMap::is_action_pressed(Keybind::Examine, event))
	{
		if (auto selected_item = inventory->get_item(m_inventory_menu->selected_slot))
		{
			if (std::find(m_examine_menu->examined_entities.begin(), m_examine_menu->examined_entities.end(), selected_item) == m_examine_menu->examined_entities.end())
			{
				m_examine_menu->examined_entities.clear();
				m_examine_menu->examined_entities.push_back(selected_item);
				m_examine_menu->show();
				m_examine_menu->focus();
			}
			else
			{
				m_examine_menu->examined_entities.erase(std::find(m_examine_menu->examined_entities.begin(), m_examine_menu->examined_entities.end(), selected_item));
				m_examine_menu->hide();
			}
		}

		return InputResult();
	}

	if (InputMap::is_action_pressed(Keybind::Consume, event))
	{
		auto selected_item = inventory->get_item(m_inventory_menu->selected_slot);

		if (!selected_item)
		{
			return InputResult();
		}
		
		if (!selected_item->get_item_def()->is_consumable())
		{
			world_state->game_message(GameMessage("You can't consume that item.", color::get("light_red")));
			return InputResult();
		}

		return InputResult(new ConsumeAction(player, selected_item), new InputResult(InputResult::Type::Pop_InputHandler));
	}
	
	if (InputMap::is_action_pressed(Keybind::PinSidebarMenu, event))
	{
		m_inventory_menu->set_pinned(!m_inventory_menu->is_pinned());
		return InputResult();
	}

	Position move_dir = InputMap::get_input_direction(event);

	if (!move_dir.empty())
	{
		return InputResult();
	}

	return InputHandler_Player::ev_key_down(event);
}


bool InputHandler_Interaction::init()
{
	if (!InputHandler_Player::init())
		return false;

	if (!m_actor || !m_subject)
	{
		CERR("Interaction input handler requires an actor and a subject.");
		return false;
	}

	m_interactions = m_actor->get_interactions_with(m_subject);

	m_selection_menu = register_sidebar<SidebarMenu_GenericSelectionMenu>(sidebar_menus::INTERACTION);
	
	m_selection_menu->title = fmt::format("{}", m_subject->display_name());
	m_selection_menu->frame_fg = m_subject->fg();

	m_selection_menu->focus();
	m_selection_menu->uncollapse();

	for (const auto& interaction : m_interactions)
	{
		m_selection_menu->menu_items.push_back(SelectionMenuItem(
			fmt::format("{} - {}", InputMap::actions_to_string(interaction.input_action), interaction.name),
			color::get("white"), 
			color::get("black")
		));
	}

	if (!m_interactions.empty())
	{
		m_selection_menu->selected_slot = 0;
	}

	return true;
}

void InputHandler_Interaction::remove()
{
	get_world_state()->get_sidebar()->remove_menu(sidebar_menus::INTERACTION);
}

void InputHandler_Interaction::step(const TimeUnit& delta_time)
{
	m_selection_menu->menu_items.clear();
	m_interactions = m_actor->get_interactions_with(m_subject);

	for (const auto& interaction : m_interactions)
	{
		m_selection_menu->menu_items.push_back(SelectionMenuItem(
			fmt::format("{} - {}", InputMap::actions_to_string(interaction.input_action), interaction.name),
			color::get("white"),
			color::get("black")
		));
	}

	if (!m_interactions.empty())
	{
		m_selection_menu->selected_slot = 0;
	}
}

InputResult InputHandler_Interaction::ev_key_down(SDL_Event event)
{
	if (m_interactions.empty() || InputMap::is_action_pressed(Keybind::Interact, event) || InputMap::is_action_pressed(Keybind::Quit, event))
	{
		return InputResult(InputResult::Type::Pop_InputHandler);
	}

	if (InputMap::is_action_pressed(Keybind::MenuUp, event))
	{
		m_selection_menu->selected_slot += 1;
		m_selection_menu->selected_slot = m_selection_menu->selected_slot % m_interactions.size();

		return InputResult();
	}

	if (InputMap::is_action_pressed(Keybind::MenuDown, event))
	{
		m_selection_menu->selected_slot -= 1;
		m_selection_menu->selected_slot = m_selection_menu->selected_slot % m_interactions.size();

		return InputResult();
	}

	if (InputMap::is_action_pressed(Keybind::MenuConfirm, event))
	{
		if (m_selection_menu->selected_slot >= 0 && m_selection_menu->selected_slot < m_interactions.size())
		{
			auto action = m_interactions[m_selection_menu->selected_slot].action(m_actor, m_subject);

			if (!action)
			{
				CERR("Interaction action returned null!");
				return InputResult();
			}

			return InputResult(action, new InputResult(InputResult::Type::Pop_InputHandler));

		}

		return InputResult();
	}

	for (const auto& interaction : m_interactions)
	{
		if (InputMap::is_action_pressed(interaction.input_action, event))
		{
			auto action = interaction.action(m_actor, m_subject);
			if (!action)
			{
				CERR("Interaction action returned null!");
				return InputResult();
			}

			return InputResult(action, new InputResult(InputResult::Type::Pop_InputHandler));
		}
	}

	Position move_dir = InputMap::get_input_direction(event);

	if (!move_dir.empty())
	{
		return InputResult();
	}

	return InputHandler_Player::ev_key_down(event);
}

void InputHandler_Interaction::render(const RenderParams& params) const
{
	if (!m_subject->has_movable())
		return;

	auto subject_pos = m_subject->get_movable()->pos();

	int draw_x{ subject_pos.x }, draw_y{ subject_pos.y }, draw_z{ subject_pos.z };

	if (params.camera)
	{
		if (!params.camera->in_view(subject_pos))
			return;

		std::tie(draw_x, draw_y, draw_z) = params.camera->get_screen(subject_pos).as_tuple();
	}

	if (draw_z != 0)
		return;

	int screen_x = draw_x + params.world_inner_frame_p1().x;
	int screen_y = draw_y + params.world_inner_frame_p1().y;

	if (!params.in_world_frame(screen_x, screen_y))
		return;

	auto draw_fg = output::get_bg(screen_x, screen_y);
	auto draw_bg = output::get_fg(screen_x, screen_y);

	output::set_fg(screen_x, screen_y, draw_fg);
	output::set_bg(screen_x, screen_y, draw_bg);
}

InputResult InputHandler_PositionSelector::ev_key_down(SDL_Event event)
{
	Position move_dir = InputMap::get_input_direction(event);

	if (!move_dir.empty() || InputMap::is_action_pressed(Keybind::MoveCenter, event))
	{
		if (!m_valid_positions.empty() && !m_valid_positions.contains(move_dir + m_center))
		{
			return InputResult(InputResult::Type::Pop_InputHandler);
		}

		return InputResult(InputResult::Type::Pop_InputHandler, new InputResult(m_callback(move_dir + m_center)));
	}

	if (InputMap::is_action_pressed(m_input_keybind, event) || InputMap::is_action_pressed(Keybind::Quit, event))
	{
		return InputResult(InputResult::Type::Pop_InputHandler);
	}

	return InputHandler_Player::ev_key_down(event);
}

bool InputHandler_PositionSelector::init()
{
	if (!InputHandler_Player::init())
		return false;

	get_world_state()->set_tick_speed(TickSpeed::TURN_BASED);

	return true;
}

void InputHandler_PositionSelector::render(const RenderParams& params) const
{
	for (auto& pos : m_valid_positions)
	{
		int draw_x{ pos.x }, draw_y{ pos.y }, draw_z{ pos.z };

		if (params.camera)
		{
			if (!params.camera->in_view(pos))
				return;

			std::tie(draw_x, draw_y, draw_z) = params.camera->get_screen(pos).as_tuple();
		}

		if (draw_z != 0)
			continue;

		int screen_x = draw_x + params.world_inner_frame_p1().x;
		int screen_y = draw_y + params.world_inner_frame_p1().y;

		auto draw_fg = output::get_bg(screen_x, screen_y);
		auto draw_bg = output::get_fg(screen_x, screen_y);

		output::set_fg(screen_x, screen_y, draw_fg);
		output::set_bg(screen_x, screen_y, draw_bg);
	}

	output::console_printf_rgb_rect_ex(
		params.world_inner_frame_p1().x, params.world_inner_frame_p1().y, params.world_inner_frame_size().x, params.world_inner_frame_size().y,
		color::get("white"), color::get("black"),
		TCOD_BKGND_SET, TCOD_CENTER, m_title
	);
}

bool InputHandler_ListSelector::init()
{
	if (!InputHandler_Player::init())
		return false;

	get_world_state()->set_tick_speed(TickSpeed::TURN_BASED);

	return true;
}

InputResult InputHandler_ListSelector::ev_key_down(SDL_Event event)
{
	if (InputMap::is_action_pressed(m_input_keybind, event) || InputMap::is_action_pressed(Keybind::Quit, event))
	{
		return InputResult(InputResult::Type::Pop_InputHandler);
	}

	if (InputMap::is_action_pressed(Keybind::MenuUp, event))
	{
		m_selected_slot += 1;
		m_selected_slot = m_selected_slot % m_items.size();

		return InputResult();
	}

	if (InputMap::is_action_pressed(Keybind::MenuDown, event))
	{
		m_selected_slot -= 1;
		m_selected_slot = m_selected_slot % m_items.size();

		return InputResult();
	}

	if (InputMap::is_action_pressed(Keybind::MenuConfirm, event))
	{
		if (m_selected_slot >= 0 && m_selected_slot < m_items.size())
		{
			return InputResult(
				InputResult::Type::Pop_InputHandler, 
				new InputResult(std::get<3>(m_items[m_selected_slot])())
			);
		}

		return InputResult();
	}

	Position move_dir = InputMap::get_input_direction(event);

	if (!move_dir.empty())
	{
		return InputResult();
	}

	return InputHandler_Player::ev_key_down(event);
}

void InputHandler_ListSelector::render(const RenderParams& params) const
{
	int panel_x = params.world_inner_frame_p1().x;
	int panel_y = params.world_inner_frame_p1().y;

	int draw_x = panel_x + 1;
	int draw_y = panel_y + 1;

	int panel_width = params.world_inner_frame_size().x / 2;
	int panel_height = 2;

	color_t fg = color::get("white");
	color_t bg = color::get("black");

	auto print_text = [&](std::string text, color_t new_fg = color::get("white"), color_t new_bg = color::get("black"))
		{
			draw_y += output::console_printf_rgb_rect_ex(draw_x, draw_y, panel_width - 1, 0, new_fg, new_bg, TCOD_BKGND_SET, TCOD_LEFT, text);
		};

	panel_height = (int)(m_items.size() + 2);

	size_t idx = 0;

	for (const auto& item : m_items)
	{
		bool selected = idx == m_selected_slot;
		print_text(std::get<0>(item), selected ? std::get<2>(item) : std::get<1>(item), selected ? std::get<1>(item) : std::get<2>(item));
		idx++;
	}

	panel_height = std::max(panel_height, (draw_y - panel_y) + 1);


	output::draw_frame_rgb(panel_x, panel_y, panel_width, panel_height, fg, bg, DEFAULT_RECT_FRAME, false, TCOD_BKGND_SET);

	if (!m_title.empty())
	{
		std::string frame_title;

		if (m_input_keybind != Keybind::None && m_display_input_keybind)
		{
			frame_title = fmt::format("{} ({})", m_title, InputMap::actions_to_string(m_input_keybind));
		}
		else
		{
			frame_title = m_title;
		}

		output::console_printf_rgb_rect_ex(panel_x, panel_y, panel_width, 1, bg, fg, TCOD_BKGND_SET, TCOD_CENTER, frame_title);
	}
}