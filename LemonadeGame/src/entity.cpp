#include "entity.h"

#include "game.h"
#include "world_state.h"
#include "world.h"

#include "action.h"

Entity::Entity(Thing* thing, Transform transform, World* world, EntityFlags entity_flags) :
	Movable(transform), m_thing(nullptr), m_entity_flags(entity_flags)
{
	m_thing.reset(thing);

	if (world)
		set_location(world);

	if (m_entity_flags & EntityFlag::CAN_HOLD_ITEMS)
	{
		setup_inventory(default_inventory_slots());
	}
}

Entity::~Entity()
{
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

	CERR("No world holder or entity holder found on entity " << m_thing->ID());

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

bool Entity::can_be_held() const
{
	return m_entity_flags & EntityFlag::CAN_BE_HELD;
}

bool Entity::can_hold_items() const
{
	return m_entity_flags & EntityFlag::CAN_HOLD_ITEMS;
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

bool Entity::is_map_bound() const
{
	return m_entity_flags & EntityFlag::IS_MAP_BOUND;
}

bool Entity::can_collide() const
{
	return m_entity_flags & EntityFlag::CAN_COLLIDE;
}

bool Entity::is_blocking() const
{
	return m_entity_flags & EntityFlag::IS_BLOCKING;
}

bool Entity::can_fall() const
{
	return m_entity_flags & EntityFlag::CAN_FALL;
}

bool Entity::is_visible() const
{
	if (is_hidden())
		return false;

	if (auto player = get_player())
	{
		return player->can_see(this);
	}

	return false;
}

bool Entity::is_hidden() const
{
	if (m_hidden)
		return true;

	if (m_is_flashing)
		return true;

	return false;
}

bool Entity::has_field_of_view() const
{
	return m_entity_flags & EntityFlag::HAS_FOV;
}

bool Entity::is_animate() const
{
	return m_entity_flags & EntityFlag::ANIMATE;
}

bool Entity::fixed_in_place() const
{
	return m_entity_flags & EntityFlag::FIXED_IN_PLACE;
}

bool Entity::is_transparent() const
{
	return m_entity_flags & EntityFlag::IS_TRANSPARENT;
}

bool Entity::is_opaque() const
{
	return !is_transparent();
}

bool Entity::is_on_floor() const
{
	auto world = get_world();
	if (!world)
	{
		CERR("No world found on entity " << m_thing->ID());
		return true;
	}

	auto tile = world->get_tile(pos());

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
		if (world->is_blocked(posx(), posy(), posz() - 1, this, -1))
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

void Entity::update_last_seen()
{
	m_last_seen_location = pos();
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
		int dx = posx() - x;
		int dy = posy() - y;
		int dz = posz() - z;

		if ((dx * dx) + (dy * dy) + (dz * dz) < static_cast<Coord>(pow(view_range(), 2)))
			return true;
	}

	return false;
}

bool Entity::can_see(const Entity* entity) const
{
	if (entity->is_hidden())
		return false;

	for (const auto& [px, py, pz] : entity->get_points())
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

bool Entity::flash()
{
	if (m_hidden && being_held())
		return false;

	if (auto world = get_world())
	{
		auto blocking = world->get_entities_at(pos());
		if (blocking.size() < 2)
			return false;

		auto frame = get_game()->frame_count();
		auto my_index = std::find_if(
			blocking.begin(), blocking.end(),
			[&](const auto& entity) { return entity == this; }
		);

		auto i = std::distance(blocking.begin(), my_index);

		return (int(frame / (2000.0f / blocking.size())) % blocking.size()) != i;
	}

	return false;
}

void Entity::game_message(std::string msg)
{
	if (auto world_state = get_world_game_state())
	{
		world_state->game_message(msg);
	}
}

void Entity::step(TimeUnit time_delta)
{
	// Fall if not on ground
	if (can_fall() && !is_on_floor())
	{
		if (m_fall_timer < TimeUnit::from_seconds(1.0))
		{
			m_fall_timer.add(time_delta);
		}

		if (m_fall_timer >= TimeUnit::from_seconds(1.0))
		{
			move(0, 0, -1);
			m_fall_timer = TimeUnit::none(); // Reset the fall timer
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
	if (is_visible())
	{
		m_seen_by_player = true;
		update_last_seen();
	}
	else if (auto player = get_player())
	{
		if (!player->can_see(m_last_seen_location))
		{
			m_seen_by_player = false;
		}
	}
}

void Entity::update()
{
	auto prev = m_is_flashing;
	m_is_flashing = flash();

	if (prev != m_is_flashing)
	{
		if (auto game = get_game())
			game->request_screen_update();
	}

	if (can_hold_items() && !m_inventory)
	{
		setup_inventory(default_inventory_slots());
	}
}