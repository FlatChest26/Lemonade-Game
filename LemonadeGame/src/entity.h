#pragma once

#ifndef LEMONADE_GAME_SRC_ENTITY_H
#define LEMONADE_GAME_SRC_ENTITY_H

#include "render_params.h"
#include "thing.h"
#include "time_units.h"
#include "inventory.h"
#include "action.h"

#include "movable.h"

#include "snowy_macros.h"
#include "game_states.h"

#include <variant>
#include <vector>
#include <map>

class TileMap;
class World;
class Game;
class WorldGameState;

using EntityLocation = std::variant<World*, Entity*>;

using EntityFlags = uint64_t;

namespace EntityFlag {
	constexpr EntityFlags NONE = 0;

	constexpr EntityFlags CAN_FALL = 1 << 0;
	constexpr EntityFlags CAN_FLY = ~CAN_FALL;

	constexpr EntityFlags IS_BLOCKING = 1 << 1;
	constexpr EntityFlags NEVER_BLOCK = ~IS_BLOCKING;

	constexpr EntityFlags CAN_COLLIDE = 1 << 2;
	constexpr EntityFlags NEVER_COLLIDE = ~CAN_COLLIDE;

	constexpr EntityFlags CAN_HOLD_ITEMS = 1 << 3;
	constexpr EntityFlags CANNOT_HOLD_ITEMS = ~CAN_HOLD_ITEMS;

	constexpr EntityFlags CAN_BE_HELD = 1 << 4;
	constexpr EntityFlags CANNOT_BE_HELD = ~CAN_BE_HELD;

	constexpr EntityFlags IS_MAP_BOUND = 1 << 5;

	constexpr EntityFlags ANIMATE = 1 << 6;
	constexpr EntityFlags INANIMATE = ~ANIMATE;

	constexpr EntityFlags HAS_FOV = 1 << 7;

	constexpr EntityFlags FIXED_IN_PLACE = 1 << 8;
	constexpr EntityFlags FREE_TO_MOVE = ~FIXED_IN_PLACE;

	constexpr EntityFlags IS_TRANSPARENT = 1 << 9;
	constexpr EntityFlags IS_OPAQUE = ~IS_TRANSPARENT;
}

class Creature;

/* An entity represents any "thing" which has a position in the world. */
class Entity : public std::enable_shared_from_this<Entity>, public Movable
{
protected:
	TimeUnit m_fall_timer = TimeUnit::none(); // Used to simulate falling over discrete time steps.

	size_t m_priority = 0;

	bool m_hidden = false;
	bool m_is_flashing = false;

	// What stores the entity. Usually the world but can also be another entity.
	EntityLocation m_location;

	std::unique_ptr<Thing> m_thing;
	std::unique_ptr<Inventory> m_inventory;

	// A queue of actions this entity wants to take.
	ActionQueue m_action_queue;

	std::unique_ptr<Action> m_current_action{ nullptr };
	TimeUnit m_time_spent_on_current_action{ TimeUnit::none() };

	// Flags for this entity
	EntityFlags m_entity_flags;

	Position m_last_seen_location;
	bool m_seen_by_player;

public:
	// -- Virtual Variables -- //

	virtual TimeUnit speed() const { return TimeUnit::from_seconds(1.0f); } // How long it takes this entity to move a single tile
	virtual int view_range() const { return 32; } // How far this entity can see

	virtual size_t default_inventory_slots() const { return 1; }
	virtual size_t inventory_size() const { return 1; } // How many inventory slots this entity takes up

	virtual size_t get_max_actions_queued() const { return 5; }

public:
	Entity(Thing* thing, Transform transform = { DEFAULT_POSITION, DEFAULT_SIZE, NORTH }, World* world = nullptr, EntityFlags entity_flags = 0);
	~Entity();

public:

	virtual constexpr bool is_creature() const { return false; }
	virtual constexpr Creature* as_creature() { return nullptr; }

	// -- Getters -- //

	// Return variant of this entity's location. Can be either a weak pointer to the world or a weak pointer to another entity.
	EntityLocation get_location() const { return m_location; }
	Inventory* get_inventory() const { return m_inventory.get(); }
	Inventory* get_holder_inventory() const;

	// Returns a weak pointer to the world holding this entity.
	World* get_world() const;

	// Returns a weak pointer to the entity carrying this entity, if one exists.
	Entity* get_entity_holder() const;
	World* get_world_holder() const;

	// Returns a weak pointer to the world game state
	GameState* get_game_state() const;
	WorldGameState* get_world_game_state() const;

	// Returns a weak pointer to the game.
	Game* get_game() const;

	// Returns a pointer to the player entity.
	Entity* get_player() const;

	// Return a pointer to the tile map this entity is on.
	TileMap* get_tile_map() const;

	Position get_last_seen_location() const
	{
		return m_last_seen_location;
	}

	bool has_been_seen_by_player() const
	{
		return m_seen_by_player;
	}

	// Thing //

	virtual Thing* get_thing() const { ASSERT(m_thing, "entity doesn't exist");  return m_thing.get(); }

	virtual ThingID ID() const { return get_thing()->ID(); }
	virtual std::string description() const { return get_thing()->description(); }

	virtual constexpr Renderable get_renderable() const { return get_thing()->get_renderable(); }
	virtual glyph_t glyph() const { return get_renderable().glyph; }
	virtual color_t fg() const { return get_renderable().fg; }
	virtual color_t bg() const { return get_renderable().bg; }

	virtual constexpr Noun get_noun() const { return get_thing()->get_noun(); }
	virtual constexpr Name get_name() const { return get_thing()->get_name(); }
	virtual constexpr Pronouns get_pronouns() const { return get_thing()->get_pronouns(); }

	constexpr std::string singular_noun() const { return get_noun().singular; }
	constexpr std::string plural_noun() const { return get_noun().plural; }

	constexpr bool is_named() const { return !get_name().empty(); }

	constexpr std::string nickname() const
	{
		if (is_named()) return prefix() + get_name().nickname + suffix();
		return prefix() + singular_noun() + suffix();
	}

	constexpr std::string full_name() const
	{
		if (is_named()) return prefix() + get_name().full_name + suffix();
		return prefix() + singular_noun() + suffix();
	}

	constexpr std::string they() const { return get_pronouns().subjective; }
	constexpr std::string them() const { return get_pronouns().objective; }
	constexpr std::string their() const { return get_pronouns().determiner; }
	constexpr std::string theirs() const { return get_pronouns().independent_possessive; }
	constexpr std::string themselves() const { return get_pronouns().reflexive; }

	virtual constexpr bool is_plural() const { return get_thing()->is_plural(); }
	virtual constexpr bool is_pronouns_plural() const { return get_thing()->is_pronouns_plural(); }

	virtual constexpr std::string prefix() const { return get_thing()->prefix(); }
	virtual constexpr std::string suffix() const { return get_thing()->suffix(); }

	// Misc //

	std::string get_display_name() const { return full_name(); }
	operator std::string() const { return get_display_name(); }

public:

	// -- Checks -- //

	bool operator==(Entity other);
	virtual bool is_player() const;

	virtual bool can_be_held() const;
	virtual bool can_hold_items() const;

	virtual bool is_map_bound() const;
	virtual bool can_collide() const;

	virtual bool is_blocking() const;

	virtual bool can_fall() const;

	virtual bool is_visible() const;
	virtual bool is_hidden() const;

	virtual bool has_field_of_view() const;
	virtual bool is_animate() const;

	virtual bool fixed_in_place() const;

	virtual bool is_transparent() const;
	virtual bool is_opaque() const;

	virtual bool can_queue_action() const { return get_max_actions_queued() >= m_action_queue.size(); }

	virtual bool has_any_action() const { return has_queued_action() || has_current_action(); }
	virtual bool has_queued_action() const { return m_action_queue.size() > 0; }
	virtual bool has_current_action() const { return m_current_action != nullptr; }

	virtual Action* get_current_action() const { return m_current_action.get(); }

	// -- Setters -- //

	bool set_location(World* world);
	bool set_location(Entity* entity);

	void set_priority(const size_t& p) { m_priority = p; }

protected:
	void update_last_seen();

public:
	// - Field of View -- //

	bool can_see(const Coord& x, const Coord& y, const Coord& z) const;
	bool can_see(const Position& pos) const { return can_see(pos.x, pos.y, pos.z); }
	bool can_see(const Entity* entity) const;

	bool has_seen(const Coord& x, const Coord& y, const Coord& z) const;
	bool has_seen(const Position& pos) const { return has_seen(pos.x, pos.y, pos.z); }

public:
	// -- Utilities -- //

	virtual bool is_on_floor() const;
	virtual bool being_held() const;

	Inventory* setup_inventory(size_t max_slots);
	bool add_to_inventory(Entity* entity);
	void remove_from_inventory(Entity* entity);

	bool queue_action(std::unique_ptr<Action> action);
	Action* get_next_action();

	virtual void start_next_action();

	virtual void start_action(std::unique_ptr<Action> action);
	virtual void continue_action(TimeUnit delta);
	virtual void complete_action();

	virtual void on_action_complete(Action* completed_action, TimeUnit time_took_to_complete) {}

	virtual bool flash();
	virtual void game_message(std::string msg);

	// -- Game Loop -- //

	virtual void step(TimeUnit time_delta);
	virtual void update();
	//virtual void render(RenderParams params) const;
};

#endif // !LEMONADE_GAME_SRC_ENTITY_H