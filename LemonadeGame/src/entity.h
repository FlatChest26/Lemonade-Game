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

#include "enum_traits.h"
#include "anatomy.h"
#include "species.h"
#include "gait.h"
#include "living.h"

#include "creature_soul.h"

#include "raw_master.h"

#include <variant>
#include <vector>
#include <map>

class TileMap;
class World;
class Game;
class WorldGameState;

using EntityLocation = std::variant<World*, Entity*>;

enum class EntityFlag : int32_t
{
	NONE = 0,

	CAN_FALL = 1 << 0,
	CAN_FLY = ~CAN_FALL,

	IS_BLOCKING = 1 << 1,
	NEVER_BLOCK = ~IS_BLOCKING,

	CAN_COLLIDE = 1 << 2,
	NEVER_COLLIDE = ~CAN_COLLIDE,

	CAN_HOLD_ITEMS = 1 << 3,
	CANNOT_HOLD_ITEMS = ~CAN_HOLD_ITEMS,

	CAN_BE_HELD = 1 << 4,
	CANNOT_BE_HELD = ~CAN_BE_HELD,

	NOT_MAP_BOUND = 1 << 5,
	IS_MAP_BOUND = ~NOT_MAP_BOUND,

	ANIMATE = 1 << 6,
	INANIMATE = ~ANIMATE,

	HAS_FOV = 1 << 7,

	FREE_TO_MOVE = 1 << 8,
	FIXED_IN_PLACE = ~FREE_TO_MOVE,

	IS_OPAQUE = 1 << 9,
	IS_TRANSPARENT = ~IS_OPAQUE,

	IS_ALIVE = 1 << 10,
	IS_DEAD = ~IS_ALIVE,

	IS_HIDDEN = 1 << 11,
	IS_FLASHING = 1 << 12,

	IS_CREATURE = 1 << 13,
	IS_ITEM = 1 << 14,
	IS_FURNITURE = 1 << 15,
};

IS_FLAG_ENUM(EntityFlag);

/* An entity represents any "thing" which has a position in the world. */
class Entity : public std::enable_shared_from_this<Entity>
{
protected:
	EntityFlag m_flags;
	EntityLocation m_location; // What stores the entity. Usually the world but can also be another entity.

	// Actions //
	ActionQueue m_action_queue;

	std::unique_ptr<Action> m_current_action{ nullptr };
	TimeUnit m_time_spent_on_current_action{ TimeUnit::none() };

	// Components //

	std::unique_ptr<Thing> m_thing{ nullptr };
	std::unique_ptr<TileMovable> m_movable{ nullptr };
	std::unique_ptr<Inventory> m_inventory{ nullptr };
	std::unique_ptr<Anatomy> m_anatomy{ nullptr };
	std::unique_ptr<Gait> m_gait{ nullptr };
	std::unique_ptr<Living> m_living{ nullptr };

	RawMaster::Ref<Species> m_species{ nullptr };

	// Misc //
	std::unique_ptr<Position> m_last_seen_location{ nullptr }; // Will be nullptr if the entity hasn't been seen

	TimeUnit m_fall_timer = TimeUnit::none(); // Used to simulate falling over discrete time steps.
	
	bool m_known_name{ false };
	bool m_should_remove{ false };

public:
	// -- Virtual Variables -- //

	virtual TimeUnit speed() const; // How long it takes this entity to move a single tile
	virtual int view_range() const; // How far this entity can see

	virtual size_t default_inventory_slots() const;
	virtual size_t inventory_size() const; // How many inventory slots this entity takes up

	virtual size_t get_max_actions_queued() const { return 5; }

public:
	Entity(Thing* thing = nullptr, EntityFlag entity_flags = EntityFlag::NONE);
	~Entity();

public:

	// -- Getters & Setters -- //

	GameState* get_game_state() const;
	WorldGameState* get_world_game_state() const;
	Game* get_game() const;
	Entity* get_player() const;
	TileMap* get_tile_map() const;
	World* get_world() const;

	EntityLocation get_location() const { return m_location; } // Return variant of this entity's location. Can be either a pointer to the world or a another entity.
	Inventory* get_holder_inventory() const;
	
	Entity* get_entity_holder() const; // Returns a pointer to the entity holding this entity (if it exists)
	World* get_world_holder() const; // Returns a pointer to the world holding this entity (if it exists)

	// Actions // 

	virtual bool can_queue_action() const { return get_max_actions_queued() >= m_action_queue.size(); }

	virtual bool has_any_action() const { return has_queued_action() || has_current_action(); }
	virtual bool has_queued_action() const { return m_action_queue.size() > 0; }
	virtual bool has_current_action() const { return m_current_action != nullptr; }

	virtual Action* get_current_action() const { return m_current_action.get(); }

	// Components //

	bool set_location(Entity* entity);
	bool set_location(World* world);

	virtual TileMovable* movable()
	{
		if (has_movable())
			return get_movable();

		set_movable({});
		
		return get_movable();
	}

	virtual TileMovable* get_movable() const { return m_movable.get(); }
	virtual bool has_movable() const { return m_movable != nullptr; }
	virtual void set_movable(const TileTransform& transform)
	{
		m_movable = std::make_unique<TileMovable>(transform);
	}

	virtual Inventory* get_inventory() const { return m_inventory.get(); }

	virtual Position get_last_seen_location() const
	{
		if (m_last_seen_location) return *m_last_seen_location;
		return {};
	}

	virtual bool has_been_seen_by_player() const { return m_last_seen_location != nullptr; }

	virtual bool has_species() const { return m_species != nullptr; }
	virtual const Species* get_species() const { return m_species.get(); }
	
	void set_species(const Species* species) { m_species.reset(species); }
	void set_species(RawMaster::Ref<Species> species) { m_species = species; }

	virtual Noun species_noun() const { if (m_species) return m_species->noun; return {}; }

	virtual Anatomy* get_anatomy() const { return m_anatomy.get(); }
	void set_anatomy(Anatomy* anatomy) { m_anatomy.reset(anatomy); m_anatomy->set_owner(this); }

	bool is_name_known() const { return m_known_name; }
	void learn_name() { m_known_name = true; }
	void forget_name() { m_known_name = false; }

	Gait* get_gait() const { return m_gait.get(); }
	void set_gait(Gait* gait) { m_gait.reset(gait); }

	void change_gait_type(std::string gait_id)
	{
		if (m_gait)
		{
			m_gait->set_current_gait(gait_id);
		}
	}

	bool has_living() const { return m_living != nullptr; }
	Living* get_living() const { return m_living.get(); }
	void set_living(Living* living) { m_living.reset(living); m_living->set_owner(this); }

	// Thing //

	virtual Thing* get_thing() const { return m_thing.get(); }

	virtual void add_soul(CreatureSoul* soul);

	virtual bool has_soul() const { return m_thing != nullptr && m_thing->is_creature_soul(); }
	virtual CreatureSoul* get_soul() const
	{
		if (has_soul())
		{
			return m_thing->as_creature_soul();
		}

		return nullptr;
	}


	virtual ThingID ID() const { return m_thing->ID(); }
	virtual std::string description() const { return m_thing->description(); }

	virtual constexpr Renderable get_renderable() const;
	virtual glyph_t glyph() const { return get_renderable().glyph; }
	virtual color_t fg() const { return get_renderable().fg; }
	virtual color_t bg() const { return get_renderable().bg; }

	virtual constexpr Noun get_noun() const { return m_thing->get_noun(); }
	virtual constexpr Name get_name() const { return m_thing->get_name(); }
	virtual constexpr Pronouns get_pronouns() const { return m_thing->get_pronouns(); }

	constexpr std::string singular_noun() const { return get_noun().singular; }
	constexpr std::string plural_noun() const { return get_noun().plural; }

	constexpr bool is_named() const { return !get_name().empty(); }

	constexpr std::string nickname() const
	{
		if (is_named())
		{
			return prefix() + get_name().nickname + suffix();
		}

		return prefix() + singular_noun() + suffix();
	}

	constexpr std::string full_name() const
	{
		if (is_named())
		{
			return prefix() + get_name().full_name + suffix();
		}

		return prefix() + singular_noun() + suffix();
	}

	constexpr std::string they() const { return get_pronouns().subjective; }
	constexpr std::string them() const { return get_pronouns().objective; }
	constexpr std::string their() const { return get_pronouns().determiner; }
	constexpr std::string theirs() const { return get_pronouns().independent_possessive; }
	constexpr std::string themselves() const { return get_pronouns().reflexive; }

	virtual constexpr bool is_plural() const { return m_thing->is_plural(); }
	virtual constexpr bool is_pronouns_plural() const { return m_thing->is_pronouns_plural(); }

	virtual constexpr std::string prefix() const { return m_thing->prefix(); }
	virtual constexpr std::string suffix() const { return m_thing->suffix(); }

	// Misc //

	virtual std::string get_display_name() const;
	operator std::string() const { return get_display_name(); }

	void queue_remove() { m_should_remove = true; }
	bool should_remove() const { return m_should_remove; }

public:

	// -- Checks -- //

	bool operator==(Entity other);

	virtual bool is_player() const;
	virtual bool is_visible_to_player() const;

	// Flags //
	
	virtual void set_flags(EntityFlag flag) { m_flags = flag; }

	virtual void add_flag(EntityFlag flag)
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
	}

	virtual void remove_flag(EntityFlag flag)
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
	}
	
	virtual void set_flag_to(EntityFlag flag, bool set_to)
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

	virtual bool can_be_held() const { return m_flags & EntityFlag::CAN_BE_HELD; }
	virtual bool can_hold_items() const { return m_flags & EntityFlag::CAN_HOLD_ITEMS; }

	virtual bool is_not_map_bound() const { return m_flags & EntityFlag::NOT_MAP_BOUND; }
	virtual bool is_map_bound() const { return !is_not_map_bound(); }

	virtual bool can_collide() const { return m_flags & EntityFlag::CAN_COLLIDE; }
	virtual bool has_no_collision() const { return !can_collide(); }

	virtual bool is_blocking() const { return m_flags & EntityFlag::IS_BLOCKING; }
	virtual bool is_not_blocking() const { return !is_blocking(); }

	virtual bool can_fall() const { return m_flags & EntityFlag::CAN_FALL; }
	virtual bool cannot_fall() const { return !can_fall(); }

	virtual bool has_field_of_view() const { return m_flags & EntityFlag::HAS_FOV; }

	virtual bool is_animate() const { return m_flags & EntityFlag::ANIMATE; }
	virtual bool is_inanimate() const { return !is_animate(); }

	virtual bool free_to_move() const { return m_flags & EntityFlag::FREE_TO_MOVE; }
	virtual bool fixed_in_place() const { return !free_to_move(); }

	virtual bool is_opaque() const { return m_flags & EntityFlag::IS_OPAQUE; }
	virtual bool is_transparent() const { return !is_opaque(); }

	virtual bool is_alive() const { return m_flags & EntityFlag::IS_ALIVE; }
	virtual bool is_dead() const { return !is_alive(); }

	virtual bool is_hidden() const { return m_flags & EntityFlag::IS_HIDDEN; }
	virtual bool is_flashing() const { return m_flags& EntityFlag::IS_FLASHING; }

	virtual bool is_creature() const { return m_flags & EntityFlag::IS_CREATURE; }

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

	virtual bool should_flash();
	virtual void game_message(std::string msg);

	// -- Game Loop -- //

	virtual void step(TimeUnit time_delta);
	virtual void update();
	//virtual void render(RenderParams params) const;
};



#endif // !LEMONADE_GAME_SRC_ENTITY_H