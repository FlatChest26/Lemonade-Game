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
	// COUT(get_display_name() << " killed in action!");
}

TimeUnit Entity::speed() const
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

int Entity::view_range() const
{
	if (is_creature())
	{
		return 32;
	}

	return 0;
}

size_t Entity::default_inventory_slots() const
{
	if (m_anatomy)
	{
		return m_anatomy->count_body_parts_with_flag(BodyPartType::Flag::GRASP);
	}

	return 1;
}

size_t Entity::inventory_size() const
{
	return 1;
}

size_t Entity::max_actions_queued() const
{
	return 5;
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

GameSettings* Entity::get_game_settings() const
{
	if (auto game = get_game())
	{
		return &game->get_settings();
	}

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

	if (m_creature_soul)
	{
		desc += m_creature_soul->description;
	}

	if (m_item_def)
	{
		desc += " " + m_item_def->description;
	}

	if (m_species)
	{
		desc += " " + m_species->description;
	}

	if (desc.empty())
	{
		return "This entity has no description.";
	}

	return desc;
}

std::string Entity::display_name() const
{
	return snowy::grammar::a(display_noun());
}

GrammarNoun Entity::display_noun() const
{
	if (is_player() && get_game_settings()->use_second_person_pronouns)
		return "you";

	if (is_name_known())
	{
		return full_name();
	}

	Noun display_noun = noun();

	if (m_creature_soul && m_creature_soul->gender.uses_noun)
	{
		Noun gender_noun = m_creature_soul->gender_noun();

		bool species_mention = false;

		if (m_living)
		{
			if (m_living->is_young())
			{
				if (m_species && m_species->gendered_nouns.has_immature_gendered_noun(m_creature_soul->gender))
				{
					gender_noun = m_species->gendered_nouns.get_immature_gendered_noun(m_creature_soul->gender);
					species_mention = true;
				}
				else
				{
					gender_noun = m_creature_soul->young_gender_noun();
				}
			}
			else
			{
				if (has_species() && get_species()->gendered_nouns.has_mature_gendered_noun(m_creature_soul->gender))
				{
					gender_noun = m_species->gendered_nouns.get_mature_gendered_noun(m_creature_soul->gender);
					species_mention = true;
				}
				else
				{
					gender_noun = m_creature_soul->gender_noun();
				}
			}
		}

		if (species_mention || (m_species && m_species->is_mundane()))
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

ThingID Entity::ID() const
{
	return m_ID;
}

Renderable Entity::get_renderable_at(Coord offset_x, Coord offset_y, Coord offset_z) const
{
	return renderable();
}

Renderable Entity::renderable() const
{
	if (is_player())
	{
		return Renderable{ PLAYER_CHAR, DEFAULT_FG_COLOR, color::get("black") };
	}

	Renderable renderable{ '?', color::get("white"), color::get("black") };

	if (m_creature_soul)
	{
		renderable = m_creature_soul->renderable();
	}

	if (m_item_def)
	{
		renderable = m_item_def->renderable;
	}

	if (m_species)
	{
		renderable = m_species->renderable;
	}

	if (is_named() && is_name_known())
	{
		renderable.glyph = nickname()[0];

		if (m_living && m_living->is_young())
		{
			renderable.glyph = tolower(nickname()[0]);
		}
		else
		{
			renderable.glyph = toupper(nickname()[0]);
		}
	}

	if (m_creature_soul)
	{
		renderable.fg = m_creature_soul->color;
	}

	return renderable;
}

Noun Entity::noun() const
{
	if (m_species)
	{
		return m_species->noun;
	}

	if (m_item_def)
	{
		return m_item_def->noun;
	}

	if (m_creature_soul)
	{
		return m_creature_soul->noun();
	}

	return Noun();
}

Name Entity::name() const
{
	if (m_creature_soul)
	{
		return m_creature_soul->name;
	}

	return Name();
}

Pronouns Entity::pronouns() const
{
	if (m_creature_soul)
	{
		return m_creature_soul->get_pronouns();
	}

	return is_animate() ? pronouns::THEY_THEM : pronouns::IT_ITS;
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

Inventory* Entity::setup_inventory()
{
	if (!m_inventory)
	{
		m_inventory = std::make_unique<Inventory>(this, default_inventory_slots());
	}
	else
	{
		m_inventory->clear();
		m_inventory->set_max_slots(default_inventory_slots());
	}

	return m_inventory.get();
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

	if (!m_inventory)
	{
		CERR("Entity has no inventory! " << ID());
		return false;
	}

	if (!m_inventory->add_item(item))
	{
		CERR("Item cannot be held! " << item->ID());
		return false;
	}

	item->set_location(this, false);
	return true;
}

void Entity::remove_from_inventory(Entity* item)
{
	if (!item)
		return;

	if (!m_inventory)
		return;

	m_inventory->remove_item(item);
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

	m_action_user->action_queue.push_back(std::move(action));
	return true;
}

Action* Entity::get_next_action()
{
	if (!has_action_user())
	{
		return nullptr;
	}

	if (m_action_user->action_queue.empty())
		return nullptr;

	return m_action_user->action_queue.front().get();
}

void Entity::start_next_action()
{
	if (!has_action_user())
	{
		return;
	}

	if (m_action_user->action_queue.empty())
		return;

	auto action = std::move(m_action_user->action_queue.front());
	m_action_user->action_queue.erase(m_action_user->action_queue.begin());

	start_action(std::move(action));
}

void Entity::start_action(std::unique_ptr<Action> action)
{
	if (!has_action_user())
	{
		return;
	}

	m_action_user->current_action = std::move(action);

	if (m_action_user->current_action->reset_progress(TimeUnit::none()))
	{
		complete_action();
	}
}

void Entity::reset_actions(std::unique_ptr<Action> action)
{
	m_action_user->action_queue.clear();
	start_action(std::move(action));
}

void Entity::continue_action(TimeUnit delta)
{
	if (!has_action_user())
	{
		return;
	}

	if (!m_action_user->current_action)
	{
		complete_action();
		return;
	}

	if (m_action_user->current_action->progress(delta))
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

	if (!m_action_user->current_action)
	{
		return;
	}

	if (auto world_state = get_world_game_state())
	{
		world_state->handle_action(std::move(m_action_user->current_action));
	}
	else
	{
		m_action_user->current_action.reset();
	}
}

bool Entity::can_see(const Coord& x, const Coord& y, const Coord& z) const
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
		return TileMap::euclidian_distance_between_squared(Position(x, y, z), m_movable->pos()) <= std::pow(view_range(), 2);
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

bool Entity::has_seen(const Coord& x, const Coord& y, const Coord& z) const
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
	if (can_hold_items() && !m_inventory)
	{
		setup_inventory();
	}

	if (!can_hold_items() && m_inventory)
	{
		m_inventory.reset();
	}

	if (can_do_actions() && !m_action_user)
	{
		m_action_user = std::make_unique<ActionUser>();
	}

	if (!can_do_actions() && m_action_user)
	{
		m_action_user.reset();
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
		auto entities_in_tile = world->get_entities(EntityFilter{ .ignore_held = true, .required_positions = { get_movable()->pos()}});

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
	if (!m_movable)
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
	// Try to escape wall

	if (m_movable && can_collide() && free_to_move() && get_tile_map()->get_tile(m_movable->pos())->is_blocked())
	{
		auto new_pos = escape_wall(m_movable->pos());
		
		if (new_pos == m_movable->pos())
		{
			CERR("Entity cannot escape wall!");
		}
		else
		{
			m_movable->pos() = new_pos;
		}
	}

	// Fall if not on ground
	if (m_movable && can_fall() && !is_on_floor())
	{
		if (m_fall_timer < TimeUnit::from_seconds(1.0))
		{
			m_fall_timer += time_delta;
		}

		if (m_fall_timer >= TimeUnit::from_seconds(1.0))
		{
			get_movable()->move(0, 0, -1);
			m_fall_timer -= TimeUnit::from_seconds(1.0); // Reset the fall timer
		}
	}

	if (m_movable && is_being_held() && get_entity_holder()->get_movable())
	{
		m_movable->pos() = get_entity_holder()->get_movable()->pos();
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

	// Step AI Brain
	if (m_brain)
	{
		m_brain->step(time_delta);
	}
}
