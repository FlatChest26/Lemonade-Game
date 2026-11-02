#include "entity.h"

#include "game.h"
#include "world_state.h"
#include "world.h"

#include "game_log.h"

#include "action.h"

Entity::Entity(EntityID ID, EntityFlag entity_flags) :
	m_ID(ID), m_flags(entity_flags)
{
	on_flags_updated();
}

Entity::~Entity()
{
}

TimeUnit Entity::move_speed() const
{
	if (auto gait = get_gait())
	{
		return TimeUnit::from_seconds(gait->get_current_speed());
	}
	else if (is_player())
	{
		return TimeUnit::from_seconds(0.7);
	}
	else if (is_creature())
	{
		return TimeUnit::from_seconds(1.2);
	}

	return TimeUnit::from_seconds(1.0f);
}

TimeUnit Entity::interact_speed() const
{
	return TimeUnit::from_seconds(0.5f);
}

int Entity::view_range() const
{
	if (has_field_of_view())
	{
		return 32;
	}

	return 0;
}

size_t Entity::default_inventory_slots() const
{
	if (has_anatomy())
	{
		return get_anatomy()->count_body_parts_with_flag(BodyPartFlag::GRASP);
	}

	return 1;
}

size_t Entity::carry_slots() const
{
	if (has_item_def())
	{
		return get_item_def()->carry_slots;
	}

	return 1;
}

size_t Entity::max_actions_queued() const
{
	return 1;
}

Inventory* Entity::get_holder_inventory() const
{
	if (auto holder = get_entity_holder())
	{
		return holder->get_inventory();
	}

	return nullptr;
}

World* Entity::get_world() const
{
	if (auto holder = get_world_holder())
	{
		return holder;
	}

	if (auto holder = get_entity_holder())
	{
		return holder->get_world();
	}

	CERR("No world holder or entity holder found on entity " << ID());

	return nullptr;
}

Entity* Entity::get_entity_holder() const
{
	if (std::holds_alternative<Entity*>(m_location))
	{
		return std::get<Entity*>(m_location);
	}

	return {};
}

World* Entity::get_world_holder() const
{
	if (std::holds_alternative<World*>(m_location))
	{
		return std::get<World*>(m_location);
	}

	return {};
}

GameState* Entity::get_game_state() const
{
	if (auto world = get_world())
	{
		return world->get_game_state();
	}

	return {};
}

WorldGameState* Entity::get_world_game_state() const
{
	if (auto world = get_world())
	{
		return world->get_world_game_state();
	}

	return {};
}

Game* Entity::get_game() const
{
	if (auto world = get_world())
	{
		return world->get_game();
	}

	return {};
}

Entity* Entity::get_player() const
{
	if (auto world_game_state = get_world_game_state())
	{
		return world_game_state->get_player();
	}

	return nullptr;
}

TileMap* Entity::get_tile_map() const
{
	if (auto world = get_world())
	{
		return world->get_tile_map();
	}

	return nullptr;
}

bool Entity::set_location(World* world, bool change_holder)
{
	if (!world)
	{
		CERR("world is null!");
		return false;
	}

	if (change_holder)
	{
		if (auto holder = get_entity_holder())
		{
			auto inventory = holder->get_inventory();
			if (inventory && holder->get_inventory()->has_item(this))
			{
				inventory->remove_item(this);
			}
		}
	}

	m_location = world;
	return true;
}

bool Entity::set_location(Entity* entity, bool change_holder)
{
	if (!entity)
	{
		CERR("entity is null!");
		return false;
	}

	if (change_holder)
	{
		if (auto holder = get_entity_holder())
		{
			auto inventory = holder->get_inventory();
			if (inventory && holder->get_inventory()->has_item(this))
			{
				inventory->remove_item(this);
			}
		}
	}

	m_location = entity;
	return true;
}

bool Entity::is_plural() const
{
	return false;
}

std::string Entity::prefix() const
{
	return "";
}

std::string Entity::suffix() const
{
	return "";
}

std::string Entity::description() const
{
	std::string desc;

	if (has_soul())
	{
		desc += get_soul()->description;
	}

	if (has_item_def())
	{
		desc += " " + get_item_def()->description;
	}

	if (has_species())
	{
		desc += " " + get_species()->description;
	}

	return trim(desc);
}

std::string Entity::display_name() const
{
	return grammar::a(display_noun());
}

GrammarNoun Entity::display_noun() const
{
	if (is_player() && GameSettings::USE_SECOND_PERSON_PRONOUNS)
		return "you";

	if (is_name_known())
	{
		return full_name();
	}

	Noun display_noun = noun();

	if (has_soul() && get_soul()->gender->uses_noun)
	{
		Noun gender_noun = get_soul()->gender->mature_noun;

		bool species_mention = false;

		if (has_living())
		{
			if (get_living()->is_young())
			{
				if (has_species() && get_species()->gendered_nouns.has_immature_gendered_noun(get_soul()->gender->gender_type))
				{
					gender_noun = get_species()->gendered_nouns.get_immature_gendered_noun(get_soul()->gender->gender_type);
					species_mention = true;
				}
				else
				{
					gender_noun = get_soul()->gender->immature_noun;
				}
			}
			else
			{
				if (has_species() && get_species()->gendered_nouns.has_mature_gendered_noun(get_soul()->gender->gender_type))
				{
					gender_noun = get_species()->gendered_nouns.get_mature_gendered_noun(get_soul()->gender->gender_type);
					species_mention = true;
				}
				else
				{
					gender_noun = get_soul()->gender->mature_noun;
				}
			}
		}

		if (species_mention || (has_species() && get_species()->is_mundane()))
		{
			display_noun = gender_noun;
		}
		else
		{
			display_noun.singular += " " + gender_noun.singular;
		}
	}

	return display_noun;
}

bool Entity::has_flags(EntityFlag flags) const
{
	if (static_cast<int>(flags) > 0)
	{
		return m_flags & flags;
	}
	else
	{
		return !(m_flags & ~flags);
	}
}

bool Entity::is_flags(EntityFlag flags) const
{
	return m_flags == flags;
}

void Entity::set_flags(EntityFlag flags)
{
	m_flags = flags;
	on_flags_updated();
}

void Entity::add_flag(EntityFlag flag)
{
	if (static_cast<int>(flag) > 0)
	{
		if (m_flags & flag)
			return;

		m_flags |= flag;
	}
	else
	{
		if (!(m_flags & ~flag))
			return;

		m_flags &= flag;
	}

	on_flags_updated();
}

void Entity::remove_flag(EntityFlag flag)
{
	if (static_cast<int>(flag) > 0)
	{
		if (!(m_flags & ~flag))
			return;

		m_flags &= ~flag;
	}
	else
	{
		if (m_flags & flag)
			return;

		m_flags |= ~flag;
	}

	on_flags_updated();
}

void Entity::set_flag_to(EntityFlag flag, bool set_to)
{
	if (set_to)
	{
		add_flag(flag);
	}
	else
	{
		remove_flag(flag);
	}
}

Renderable Entity::get_renderable_at(Coord offset_x, Coord offset_y, Coord offset_z) const
{
	if (has_furniture_def())
	{
		return get_furniture_def()->get_renderable_at(offset_x, offset_y, offset_z);
	}

	return renderable();
}

Renderable Entity::renderable() const
{
	if (is_player())
	{
		return Renderable{ PLAYER_CHAR, DEFAULT_FG_COLOR, color::get("black") };
	}

	Renderable renderable{ '?', color::get("white"), color::get("black") };

	if (has_soul())
	{
		renderable = get_soul()->renderable();
	}

	if (has_item_def())
	{
		renderable = get_item_def()->renderable;
	}

	if (has_species())
	{
		renderable = get_species()->renderable;
	}

	if (is_named() && is_name_known())
	{
		renderable.glyph = nickname()[0];

		if (has_living() && get_living()->is_young())
		{
			renderable.glyph = tolower(nickname()[0]);
		}
		else
		{
			renderable.glyph = toupper(nickname()[0]);
		}
	}

	if (has_soul())
	{
		renderable.fg = get_soul()->color;
	}

	return renderable;
}

Noun Entity::noun() const
{
	if (has_species())
	{
		return get_species()->noun;
	}

	if (has_item_def())
	{
		return get_item_def()->noun;
	}

	if (has_soul())
	{
		return get_soul()->noun();
	}

	return Noun();
}

Name Entity::name() const
{
	if (has_soul())
	{
		return get_soul()->name;
	}

	return Name();
}

RawMaster::Ref<Pronouns> Entity::pronouns() const
{
	if (has_soul())
	{
		return get_soul()->gender->pronouns;
	}

	return is_animate() ? RawMaster::get<Pronouns>("NEUTRAL") : RawMaster::get<Pronouns>("OBJECT");
}

bool Entity::is_visible_to_player() const
{
	if (is_hidden())
	{
		return false;
	}

	if (is_player())
	{
		return true;
	}

	if (auto player = get_player())
	{
		return player->can_see(this);
	}

	return false;
}

bool Entity::is_on_floor() const
{
	if (!has_movable())
	{
		return false;
	}

	auto world = get_world();
	if (!world)
	{
		CERR("No world found on entity " << ID());
		return true;
	}

	auto tile = world->get_tile(get_movable()->pos());

	if (!tile)
	{
		CERR("No tile found at entity " << ID());
		return false;
	}

	if (tile->has_floor() || tile->can_descend())
	{
		return true;
	}

	return world->is_blocked(get_movable()->posx(), get_movable()->posy(), get_movable()->posz() - 1, this, -1);
}

bool Entity::is_being_held() const
{
	return std::holds_alternative<Entity*>(m_location);
}

#include "interactions.h"

bool Entity::can_interact_with(Entity* subject) const
{
	if (has_inventory())
	{
		if (get_inventory()->has_item(subject))
		{
			return true;
		}

		if (!get_inventory()->has_item(subject) && get_inventory()->can_carry(subject))
		{
			return true;
		}
	}

	if (subject->has_item_def() && subject->get_item_def()->is_consumable())
	{
		return true;
	}

	return false;
}

std::vector<InteractionOption> Entity::get_interactions_with(Entity* subject)
{
	std::vector<InteractionOption> potential_actions;

	if (has_inventory())
	{
		if (get_inventory()->has_item(subject))
		{
			potential_actions.push_back({
				.name = "drop",
				.input_action = Keybind::Drop,
				.action = [](Entity* actor, Entity* subject) { return new DropAction(actor->interact_speed(), actor, subject); }
			});
		}

		if (!get_inventory()->has_item(subject) && get_inventory()->can_carry(subject))
		{
			potential_actions.push_back({
				.name = "grab",
				.input_action = Keybind::Grab,
				.action = [](Entity* actor, Entity* subject) { return new GrabAction(actor->interact_speed(), actor, subject); }
			});
		}
	}

	if (subject->has_item_def() && subject->get_item_def()->is_consumable())
	{
		potential_actions.push_back({
			.name = "consume",
			.input_action = Keybind::Consume,
			.action = [](Entity* actor, Entity* subject) { return new ConsumeAction(actor->interact_speed(), actor, subject); }
		});
	}

	return potential_actions;
}

Inventory* Entity::setup_inventory()
{
	if (!has_inventory())
	{
		set_inventory(new Inventory(this, default_inventory_slots()));
	}
	else
	{
		get_inventory()->set_max_slots(default_inventory_slots());
	}

	return get_inventory();
}

bool Entity::add_to_inventory(Entity* item)
{
	if (!item)
	{
		CERR("Item is null! " << item->ID());
		return false;
	}

	if (!item->can_be_held())
	{
		CERR("Item cannot be held! " << item->ID());
		return false;
	}

	if (!can_hold_items())
	{
		CERR("Entity cannot hold items! " << ID());
		return false;
	}

	if (!has_inventory())
	{
		CERR("Entity has no inventory! " << ID());
		return false;
	}

	if (!get_inventory()->add_item(item))
	{
		CERR("Item cannot be held! " << item->ID());
		return false;
	}

	item->set_location(this, false);
	return true;
}

void Entity::remove_from_inventory(Entity* item, bool update_inventory)
{
	if (!item)
		return;

	if (!has_inventory())
		return;

	if (item->get_entity_holder() != this)
		return;

	if (update_inventory)
	{
		get_inventory()->remove_item(item);
	}

	item->set_location(get_world(), false);
}

bool Entity::queue_action(std::unique_ptr<Action> action)
{
	if (!has_action_user())
	{
		return false;
	}

	if (!action->check_action())
	{
		return false;
	}

	if (!can_queue_action())
	{
		return false;
	}

	if (action->has_subject() && cannot_interact_with_others())
	{
		return false;
	}

	get_action_user()->action_queue.push_back(std::move(action));
	return true;
}

Action* Entity::get_next_action()
{
	if (!has_action_user())
	{
		return nullptr;
	}

	if (get_action_user()->action_queue.empty())
		return nullptr;

	return get_action_user()->action_queue.front().get();
}

void Entity::start_next_action()
{
	if (!has_action_user())
	{
		return;
	}

	if (get_action_user()->action_queue.empty())
		return;

	auto action = std::move(get_action_user()->action_queue.front());
	get_action_user()->action_queue.erase(get_action_user()->action_queue.begin());

	start_action(std::move(action));
}

void Entity::start_action(std::unique_ptr<Action> action)
{
	if (!has_action_user())
	{
		return;
	}

	get_action_user()->current_action = std::move(action);

	if (get_action_user()->current_action->reset_progress(TimeUnit::none()))
	{
		complete_action();
	}
}

void Entity::reset_actions(std::unique_ptr<Action> action)
{
	get_action_user()->action_queue.clear();
	start_action(std::move(action));
}

void Entity::continue_action(TimeUnit delta)
{
	if (!has_action_user())
	{
		return;
	}

	if (!get_action_user()->current_action)
	{
		complete_action();
		return;
	}

	if (get_action_user()->current_action->progress(delta))
	{
		complete_action();
	}
}

void Entity::complete_action()
{
	if (!has_action_user())
	{
		return;
	}

	if (!get_action_user()->current_action)
	{
		return;
	}

	if (auto world_state = get_world_game_state())
	{
		world_state->handle_action(std::move(get_action_user()->current_action));
	}
	else
	{
		get_action_user()->current_action.reset();
	}
}

bool Entity::can_see(Coord x, Coord y, Coord z) const
{
	if (is_player())
	{
		if (auto tile_map = get_tile_map())
		{
			return tile_map->is_visible(x, y, z);
		}
	}
	else
	{
		auto result = los::raycast3D(
			get_world(),
			get_movable()->pos(),
			Position(x, y, z),
			los::RaycastParams{
				.max_range = static_cast<double>(view_range()),
				.stop_when_blocked = false,
				.stop_when_opaque = true,
				.fit = this
			}
		);

		return result.reached_end;
	}

	return false;
}

bool Entity::can_see(const Entity* entity) const
{
	if (entity->is_hidden())
		return false;

	for (const auto& [px, py, pz] : entity->get_movable()->get_points())
	{
		if (can_see(px, py, pz))
			return true;
	}

	return false;
}

bool Entity::has_seen(Coord x, Coord y, Coord z) const
{
	if (is_player())
	{
		if (auto tile_map = get_tile_map())
		{
			return tile_map->is_explored(x, y, z);
		}
	}

	return false;
}

bool Entity::operator==(const Entity& other) const
{
	return m_ID == other.m_ID;
}

bool Entity::is_player() const
{
	if (auto player = get_player())
	{
		return this->ID() == player->ID();
	}
	return false;
}

void Entity::on_flags_updated()
{
	if (can_hold_items() && !has_inventory())
	{
		setup_inventory();
	}

	if (!can_hold_items() && has_inventory())
	{
		remove_component<Inventory>();
	}

	if (can_do_actions() && !has_action_user())
	{
		set_action_user(new ActionUser());
	}

	if (!can_do_actions() && has_action_user())
	{
		remove_component<ActionUser>();
	}
}

bool Entity::should_flash()
{
	if (is_being_held() || is_hidden())
	{
		return false;
	}

	if (!has_movable())
	{
		return false;
	}

	if (auto world = get_world())
	{
		auto entities_in_tile = world->get_entities(EntityFilter{
			.ignore_held = true,
			.required_flags = type_flag(),
			.required_positions = { get_movable()->pos() }
			});

		if (entities_in_tile.size() < 2)
		{
			return false;
		}

		auto i = std::distance(
			entities_in_tile.begin(),
			std::find_if(
				entities_in_tile.begin(), entities_in_tile.end(),
				[&](const auto& entity) { return entity == this; }
			)
		);

		int frame = get_game()->frame_count();
		int flash_frames = 30;

		return (frame / flash_frames) % int(entities_in_tile.size()) != int(i);
	}

	return false;
}

void Entity::game_message(GameMessage msg)
{
	if (auto world_state = get_world_game_state())
	{
		world_state->game_message(msg);
	}
}

Position Entity::escape_wall(const Position& pos) const
{
	if (!has_movable())
		return pos;

	auto check_block = [&](const Position& other_pos)
		{
			return get_world()->is_blocked(other_pos, this, other_pos.z - pos.z);
		};

	if (!check_block(pos))
		return pos;

	Position valid_pos = pos;

	for (const auto& check_pos : transforms::get_surrounding_locations(pos))
	{
		if (!check_block(check_pos))
			return check_pos;
	}

	return pos;
}

void Entity::step(const TimeUnit& time_delta)
{
	if (has_movable())
	{
		// Try to escape wall
		if (!is_being_held() && can_collide() && free_to_move() && get_tile_map()->get_tile(get_movable()->pos())->is_blocked())
		{
			auto new_pos = escape_wall(get_movable()->pos());

			if (new_pos == get_movable()->pos())
			{
				CERR("Entity cannot escape wall!");
			}
			else
			{
				get_movable()->pos() = new_pos;
			}
		}

		// Fall if not on ground
		if (can_fall() && !is_on_floor())
		{
			if (!has_component<FallTimer>())
			{
				set_component<FallTimer>(new FallTimer());
			}

			if (*get_component<FallTimer>() < TimeUnit::from_seconds(1.0))
			{
				*get_component<FallTimer>() += time_delta;
			}

			if (*get_component<FallTimer>() >= TimeUnit::from_seconds(1.0))
			{
				get_movable()->move(0, 0, -1);
				*get_component<FallTimer>() -= TimeUnit::from_seconds(1.0); // Reset the fall timer
			}
		}

		if (is_being_held() && get_entity_holder()->has_movable())
		{
			get_movable()->pos() = get_entity_holder()->get_movable()->pos() - get_movable()->get_center();
		}
	}

	// Handle current action
	if (has_action_user())
	{
		if (has_current_action())
		{
			continue_action(time_delta);
		}
		else if (has_queued_action())
		{
			start_next_action();
			continue_action(time_delta);
		}
	}
	
	if (has_living())
	{
		get_living()->step(time_delta);
	}

	if (has_brain())
	{
		get_brain()->step(time_delta);
	}
}