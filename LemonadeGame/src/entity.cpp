#include "entity.h"

#include "game.h"
#include "world_state.h"
#include "world.h"

#include "action.h"

Entity::Entity(Thing* thing, EntityFlag entity_flags) :
	m_thing(thing), m_flags(entity_flags)
{
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


bool Entity::operator==(Entity other)
{
	return ID() == other.ID();
}

bool Entity::is_player() const
{
	if (auto player = get_player())
	{
		return this->ID() == player->ID();
	}
	return false;
}

bool Entity::set_location(World* world)
{
	if (auto holder = get_entity_holder())
	{
		holder->remove_from_inventory(this);
	}

	m_location = world;

	return true;
}

void Entity::add_soul(CreatureSoul* soul)
{
	soul->set_physical_form(this);
	m_thing.reset(soul);

	add_flag(EntityFlag::ANIMATE);
}


std::string Entity::get_display_name() const
{
	if (is_name_known())
	{
		return full_name();
	}

	Noun display_noun = get_noun();

	if (has_soul() && get_soul()->get_gender().uses_noun)
	{
		Noun gender_noun = get_soul()->gender_noun();

		bool species_mention = false;

		if (has_living())
		{
			if (get_living()->is_young())
			{
				if (has_species() && get_species()->gendered_nouns.has_immature_gendered_noun(get_soul()->get_gender()))
				{
					gender_noun = get_species()->gendered_nouns.get_immature_gendered_noun(get_soul()->get_gender());
					species_mention = true;
				}
				else
				{
					gender_noun = get_soul()->young_gender_noun();
				}
			}
			else
			{
				if (has_species() && get_species()->gendered_nouns.has_mature_gendered_noun(get_soul()->get_gender()))
				{
					gender_noun = get_species()->gendered_nouns.get_mature_gendered_noun(get_soul()->get_gender());
					species_mention = true;
				}
				else
				{
					gender_noun = get_soul()->gender_noun();
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


	return snowy::grammar::a(display_noun);
}

constexpr Renderable Entity::get_renderable() const
{
	Renderable renderable{ '?', color::get("white"), color::get("black") };

	if (get_thing())
	{
		renderable = get_thing()->get_renderable();
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
		renderable.fg = get_soul()->fg();
	}

	return renderable;
}

bool Entity::set_location(Entity* entity)
{
	if (auto holder = get_entity_holder())
	{
		holder->remove_from_inventory(this);
	}

	if (auto e = entity)
	{
		m_location = entity;
	}

	return false;
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
		CERR("No world found on entity " << m_thing->ID());
		return true;
	}

	auto tile = world->get_tile(get_movable()->pos());

	if (!tile)
	{
		CERR("No tile found at entity " << m_thing->ID());
		return false;
	}

	if (tile->has_floor() || tile->can_descend())
	{
		return true;
	}

	if (auto world = get_world())
	{
		if (world->is_blocked(get_movable()->posx(), get_movable()->posy(), get_movable()->posz() - 1, this, -1))
		{
			return true;
		}
	}

	return false;
}

bool Entity::being_held() const
{
	return std::holds_alternative<Entity*>(m_location);
}

Inventory* Entity::setup_inventory(size_t max_slots)
{
	if (!m_inventory)
	{
		m_inventory = std::make_unique<Inventory>(this, max_slots);
	}
	else
	{
		m_inventory->clear();
		m_inventory->set_max_slots(max_slots);
	}

	return m_inventory.get();
}

bool Entity::add_to_inventory(Entity* entity)
{
	if (!entity)
		return false;

	if (!entity->can_be_held())
		return false;

	if (!can_hold_items())
		return false;

	if (!m_inventory)
	{
		setup_inventory(default_inventory_slots());
	}

	if (!m_inventory->add_item(entity))
	{
		return false;
	}

	entity->set_location(this);

	return true;
}

void Entity::remove_from_inventory(Entity* entity)
{
	if (!entity)
		return;

	if (!m_inventory)
		return;

	m_inventory->remove_item(entity);
}

bool Entity::queue_action(std::unique_ptr<Action> action)
{
	if (!action->check_action())
	{
		return false;
	}

	if (!can_queue_action())
	{
		return false;
	}

	m_action_queue.push_back(std::move(action));
	return true;
}

Action* Entity::get_next_action()
{
	if (m_action_queue.empty())
		return nullptr;

	return m_action_queue.front().get();
}

void Entity::start_next_action()
{
	if (m_action_queue.empty())
		return;

	auto action = std::move(m_action_queue.front());
	m_action_queue.erase(m_action_queue.begin());

	start_action(std::move(action));
}

void Entity::start_action(std::unique_ptr<Action> action)
{
	m_current_action = std::move(action);
	m_time_spent_on_current_action = TimeUnit::none();

	if (m_current_action->is_action_complete(m_time_spent_on_current_action))
	{
		complete_action();
	}
}

void Entity::continue_action(TimeUnit delta)
{
	if (!m_current_action)
	{
		complete_action();
		return;
	}

	m_time_spent_on_current_action.add(delta);

	if (m_current_action->is_action_complete(m_time_spent_on_current_action))
	{
		complete_action();
	}
}

void Entity::complete_action()
{
	m_time_spent_on_current_action = TimeUnit::none();
	on_action_complete(m_current_action.get(), m_time_spent_on_current_action);

	if (!m_current_action)
	{
		return;
	}

	if (auto world_state = get_world_game_state())
	{
		world_state->handle_action(std::move(m_current_action));
	}
	else
	{
		m_current_action.reset();
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
		int dx = get_movable()->posx() - x;
		int dy = get_movable()->posy() - y;
		int dz = get_movable()->posz() - z;

		if ((dx * dx) + (dy * dy) + (dz * dz) < static_cast<Coord>(pow(view_range(), 2)))
			return true;
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
			return tile_map->is_explored(x, y, z);
	}

	return false;
}

bool Entity::should_flash()
{
	if (being_held() || is_hidden())
	{
		return false;
	}

	if (auto world = get_world())
	{
		auto blocking = world->get_entities_at(get_movable()->pos());

		if (blocking.size() < 2)
		{
			return false;
		}

		auto my_index = std::find_if(
			blocking.begin(), blocking.end(),
			[&](const auto& entity) { return entity == this; }
		);

		auto i = std::distance(blocking.begin(), my_index);
		return (int(get_game()->frame_count() / (60.0f / blocking.size())) % blocking.size()) != i;
	}

	return false;
}

void Entity::game_message(std::string msg)
{
	if (msg.empty())
	{
		return;
	}
		
	if (auto world_state = get_world_game_state())
	{
		world_state->game_message(msg);
	}
}

void Entity::step(TimeUnit time_delta)
{
	// Fall if not on ground
	if (has_movable() && can_fall() && !is_on_floor())
	{
		if (m_fall_timer < TimeUnit::from_seconds(1.0))
		{
			m_fall_timer.add(time_delta);
		}

		if (m_fall_timer >= TimeUnit::from_seconds(1.0))
		{
			get_movable()->move(0, 0, -1);
			m_fall_timer.subtract(TimeUnit::from_seconds(1.0)); // Reset the fall timer
		}
	}

	// Handle current action
	if (has_current_action())
	{
		continue_action(time_delta);
	}
	else if (has_queued_action())
	{
		start_next_action();
		continue_action(time_delta);
	}

	// Handle last seen location
	if (is_visible_to_player())
	{
		m_last_seen_location = std::make_unique<Position>(get_movable()->pos());
	}
	else if (m_last_seen_location && get_player() && !get_player()->can_see(*m_last_seen_location))
	{
		m_last_seen_location.reset();
	}
}

void Entity::update()
{
	if (can_hold_items() && !m_inventory)
	{
		setup_inventory(default_inventory_slots());
	}
}