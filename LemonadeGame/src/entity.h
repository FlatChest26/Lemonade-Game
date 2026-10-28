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
#include "item_def.h"
#include "ai_brain.h"
#include "entity_memory.h"
#include "entity_id.h"

#include <variant>
#include <vector>
#include <map>

class TileMap;
class World;
class Game;
class WorldGameState;
struct GameSettings;
struct GameMessage;

using EntityLocation = std::variant<World*, Entity*>;

enum class EntityFlag : int32_t
{
	NONE = 0,

	CAN_FALL = 1 << 0,											// The entity will fall with respect to gravity.
	CAN_FLY = ~CAN_FALL,										// The entity will ignore gravity.

	IS_BLOCKING = 1 << 1,										// The entity will act as an obstacle to other entities.
	NEVER_BLOCK = ~IS_BLOCKING,									// The entity will never act as an obstactle to other entities

	CAN_COLLIDE = 1 << 2,										// The entity cannot enter blocked tiles or entities
	NEVER_COLLIDE = ~CAN_COLLIDE,								// The entity can enter blocked tiles or entities

	CAN_HOLD_ITEMS = 1 << 3,									// The entity has an inventory that can hold items
	CANNOT_HOLD_ITEMS = ~CAN_HOLD_ITEMS,						// The entity does not have an inventory that can hold items

	CAN_BE_HELD = 1 << 4,										// The entity can be carried in another entity's inventory.
	CANNOT_BE_HELD = ~CAN_BE_HELD,								// The entity cannot be carried in another entity's inventory.

	NOT_MAP_BOUND = 1 << 5,										// The entity is restrained to the boundaries of the tile map.
	IS_MAP_BOUND = ~NOT_MAP_BOUND,								// The entity is NOT restrained to the boundaries of the tile map.

	ANIMATE = 1 << 6,											// The entity is (or was) a living thing.
	INANIMATE = ~ANIMATE,										// The entity is an object.

	HAS_FOV = 1 << 7,											// The entity can see things within its vision.

	FREE_TO_MOVE = 1 << 8,										// The entity can move (or be moved) around.
	FIXED_IN_PLACE = ~FREE_TO_MOVE,								// The entity is stuck in place unless manually moved by a function.

	IS_OPAQUE = 1 << 9,											// The entity can be seen through.
	IS_TRANSPARENT = ~IS_OPAQUE,								// The entity cannot be seen through.

	IS_ALIVE = 1 << 10,											// The entity is alive.
	IS_DEAD = ~IS_ALIVE,										// The entity is not alive.

	IS_HIDDEN = 1 << 11,										// The entity cannot be seen.
	IS_FLASHING = 1 << 12,										// The entity is briefly not being rendered (has no effect on the world)

	IS_CREATURE = 1 << 13,										// The entity is a creature (whatever that means).
	IS_ITEM = 1 << 14,											// The entity is an item (whatever that means).
	IS_FURNITURE = 1 << 15,										// The entity is furniture (whatever that means).

	CAN_DO_ACTIONS = 1 << 16,									// The entity can queue up and perform actions.
	
	CAN_INTERACT_WITH_OTHERS = 1 << 17,							// The entity can interact with other things during actions.
	CANNOT_INTERACT_WITH_OTHERS = ~CAN_INTERACT_WITH_OTHERS,	// The entity can interact with other things during actions.
};

IS_FLAG_ENUM(EntityFlag);

struct ActionUser
{
	ActionQueue action_queue;

	std::unique_ptr<Action> current_action{ nullptr };
};

/* An entity represents any "thing" which has a position in the world. */
class Entity : public std::enable_shared_from_this<Entity>
{
protected:

	EntityID m_ID;

	EntityFlag m_flags;
	EntityLocation m_location; // What stores the entity. Usually the world but can also be another entity.
	bool m_known_name{ false }; // Whether the player knows this entity's name
	bool m_should_remove{ false }; // Whether the entity should be removed on the next frame.

	TimeUnit m_fall_timer = TimeUnit::none(); // Used to simulate falling over discrete time steps.

	// Components //

	RawMaster::Ref<Species> m_species{ nullptr };
	RawMaster::Ref<ItemDefinition> m_item_def{ nullptr };

	std::unique_ptr<ActionUser> m_action_user{ nullptr };
	std::unique_ptr<CreatureSoul> m_creature_soul{ nullptr };
	std::unique_ptr<TileMovable> m_movable{ nullptr };
	std::unique_ptr<Inventory> m_inventory{ nullptr };
	std::unique_ptr<Anatomy> m_anatomy{ nullptr };
	std::unique_ptr<Gait> m_gait{ nullptr };
	std::unique_ptr<Living> m_living{ nullptr };
	std::unique_ptr<AIBrain> m_brain{ nullptr };

	std::vector<EntityMemory> m_memories;

public:
	// -- Virtual Variables -- //

	virtual TimeUnit speed() const; // How long it takes this entity to move a single tile
	virtual int view_range() const; // How far this entity can see

	virtual size_t default_inventory_slots() const; // How many inventory slots this entity has by default
	virtual size_t inventory_size() const; // How many inventory slots this entity takes up

	virtual size_t max_actions_queued() const; // How many actions this entity can queue at once

public:
	Entity(EntityID ID, EntityFlag entity_flags = EntityFlag::NONE);
	~Entity();

public:

	// -- Getters & Setters -- 

	GameState* get_game_state() const;
	WorldGameState* get_world_game_state() const;
	Game* get_game() const;
	Entity* get_player() const;
	TileMap* get_tile_map() const;
	World* get_world() const;
	GameSettings* get_game_settings() const;

	EntityLocation get_location() const { return m_location; } // Return variant of this entity's location. Can be either a pointer to the world or a another entity.
	Inventory* get_holder_inventory() const;
	
	Entity* get_entity_holder() const; // Returns a pointer to the entity holding this entity (if it exists)
	World* get_world_holder() const; // Returns a pointer to the world holding this entity (if it exists)

	void queue_remove() { m_should_remove = true; }
	bool should_remove() const { return m_should_remove; }

	virtual EntityID ID() const;

public: 
	// Actions
	
	ActionUser* get_action_user() const { return m_action_user.get(); }
	bool has_action_user() const { return m_action_user != nullptr; }
	void set_action_user(ActionUser* action_user) { m_action_user.reset(action_user); }

	bool queue_action(std::unique_ptr<Action> action);
	Action* get_next_action();

	virtual void start_next_action();

	virtual void start_action(std::unique_ptr<Action> action);
	virtual void reset_actions(std::unique_ptr<Action> action);
	virtual void continue_action(TimeUnit delta);
	virtual void complete_action();

	virtual bool can_queue_action() const
	{
		if (!has_action_user())
			return false;

		return max_actions_queued() >= get_action_user()->action_queue.size();
	}

	virtual bool has_any_action() const
	{
		if (!has_action_user())
			return false;
		return has_queued_action() || has_current_action();
	}

	virtual bool has_queued_action() const
	{
		if (!has_action_user())
			return false;
		return get_action_user()->action_queue.size() > 0;
	}

	virtual bool has_current_action() const
	{
		if (!has_action_user())
			return false;

		return get_action_user()->current_action != nullptr;
	}

	virtual Action* get_current_action() const { return get_action_user()->current_action.get(); }

public: 
	
	// -- Components -- //
	
	// Inventory  //
	
	// Adds an `Inventory` component with the set amount of max slots.
	Inventory* setup_inventory(); 
	// Adds an item to this entity's `Inventory` component. Returns true if adding the item was successful.
	bool add_to_inventory(Entity* item);
	// Removes an item from this entity's `Inventory` component.
	void remove_from_inventory(Entity* item);
	// Returns this entity's `Inventory` component, if it exists.
	virtual Inventory* get_inventory() const { return m_inventory.get(); }

	// Location //

	// Sets this entity's holder to the input. Automatically removes them from another's inventory if necessary.
	bool set_location(Entity* entity, bool change_holder = true);
	// Sets this entity's holder to the input. Automatically removes them from another's inventory if necessary.
	bool set_location(World* world, bool change_holder = true);

	// Movable //

	// Returns this entity's `Movable` component, if it exists.
	virtual TileMovable* get_movable() const { return m_movable.get(); }
	// Returns true if this entity has a `Movable` component.
	virtual bool has_movable() const { return m_movable != nullptr; }
	// Sets this entity's `Movable` component to the input.
	virtual void set_movable(const TileTransform& transform) { m_movable = std::make_unique<TileMovable>(transform); }

	// Species //

	// Returns true if this entity has a `Species` component.
	virtual bool has_species() const { return m_species != nullptr; }
	// Returns this entity's `Species` component, if it exists.
	virtual const Species* get_species() const { return m_species.get(); }
	
	// Sets this entity's `Species` component to the input.
	virtual void set_species(const Species* species) { m_species.reset(species); }
	// Sets this entity's `Species` component to the input.
	virtual void set_species(RawMaster::Ref<Species> species) { m_species = species; }
	// Sets this entity's `Species` component to the input (this tme using a species ID).
	virtual void set_species(RawMaster::RawID species_id) { m_species = RawMaster::get<Species>(species_id); }

	// Anatomy //

	// Returns this entity's `Anatomy` component, if it exists.
	virtual Anatomy* get_anatomy() const { return m_anatomy.get(); }
	// Sets this entity's `Anatomy` component to the input.
	virtual void set_anatomy(Anatomy* anatomy) { m_anatomy.reset(anatomy); m_anatomy->set_owner(this); }

	// Known name //

	// Returns true if the player knows this entity's name.
	virtual bool is_name_known() const { return m_known_name; }
	// The player learns this entity's name.
	virtual void learn_name() { m_known_name = true; }
	// The player forgets this entity's name.
	virtual void forget_name() { m_known_name = false; }

	// Gait //
	
	// Returns this entity's `Gait` component, if it exists.
	virtual Gait* get_gait() const { return m_gait.get(); }
	// Sets this entity's `Gait` component to the input.
	virtual void set_gait(Gait* gait) { m_gait.reset(gait); }

	// Changes this entity's gait type only if they have a `Gait` component.
	virtual void change_gait_type(std::string gait_id)
	{
		if (m_gait)
		{
			m_gait->set_current_gait(gait_id);
		}
	}

	// Living //

	// Returns true if this entity has a `Living` component.
	virtual bool has_living() const { return m_living != nullptr; }
	// Returns this entity's `Living` component, if it exists.
	virtual Living* get_living() const { return m_living.get(); }
	// Sets this entity's `Living` component to the input.
	virtual void set_living(Living* living) { m_living.reset(living); m_living->set_owner(this); }

	// Item Definition //

	// Returns this entity's `ItemDefinition` component, if it exists.
	virtual const ItemDefinition* get_item_def() const { return m_item_def.get(); }
	// Sets this entity's `ItemDefinition` component to the input.
	virtual void set_item_def(RawMaster::Ref<ItemDefinition> item_def) { m_item_def = item_def; }

	// Soul //

	// Sets this entity's `CreatureSoul` component to the input.
	virtual void add_soul(CreatureSoul* soul)
	{
		m_creature_soul.reset(soul); 
		add_flag(EntityFlag::ANIMATE);
	}

	// Removes this entity's `CreatureSoul` component and returns its unique_ptr.
	virtual std::unique_ptr<CreatureSoul> remove_soul()
	{
		remove_flag(EntityFlag::ANIMATE);
		return std::move(m_creature_soul);
	}

	// Returns true if this entity has a `CreatureSoul` component.
	virtual bool has_soul() const { return m_creature_soul != nullptr; }
	// Returns this entity's `CreatureSoul` component, if it exists.
	virtual CreatureSoul* get_soul() const { return m_creature_soul.get(); }

	// Brain //

	virtual AIBrain* add_brain(AIBrain* brain)
	{
		if (brain == nullptr)
			return nullptr;

		m_brain.reset(brain);
		m_brain->set_entity(this);

		return m_brain.get();
	}

	virtual std::unique_ptr<AIBrain> remove_brain()
	{
		m_brain->set_entity(nullptr);
		return std::move(m_brain);
	}

	virtual AIBrain* get_brain() { return m_brain.get(); }

	virtual bool has_brain() const { return m_brain != nullptr; }

public:
	// Thing //

	

	// For entities larger than 1x1x1, return the renderable at that coordinate, relative to the entity's position.
	virtual Renderable get_renderable_at(Coord offset_x, Coord offset_y, Coord offset_z) const;
	// For entities larger than 1x1x1, return the renderable at that coordinate, relative to the entity's position.
	virtual Renderable get_renderable_at(Position offset) const { return get_renderable_at(offset.x, offset.y, offset.z); }

	virtual Renderable renderable() const;
	virtual glyph_t glyph() const { return renderable().glyph; }
	virtual color_t fg() const { return renderable().fg; }
	virtual color_t bg() const { return renderable().bg; }

	virtual Noun noun() const;
	virtual Name name() const;
	virtual Pronouns pronouns() const;

	std::string singular_noun() const { return noun().singular; }
	std::string plural_noun() const { return noun().plural; }

	bool is_named() const { return !name().empty(); }

	std::string nickname() const
	{
		if (is_named())
		{
			return prefix() + name().nickname + suffix();
		}

		return prefix() + singular_noun() + suffix();
	}

	std::string full_name() const
	{
		if (is_named())
		{
			return prefix() + name().full_name + suffix();
		}

		return prefix() + singular_noun() + suffix();
	}

	std::string they() const { return pronouns().subjective; }
	std::string them() const { return pronouns().objective; }
	std::string their() const { return pronouns().determiner; }
	std::string theirs() const { return pronouns().independent_possessive; }
	std::string themselves() const { return pronouns().reflexive; }

	virtual bool is_plural() const;
	virtual bool is_pronouns_plural() const { return pronouns().acts_plural; }

	virtual std::string prefix() const;
	virtual std::string suffix() const;

	virtual std::string description() const;

	virtual std::string display_name() const;
	virtual GrammarNoun display_noun() const;

	operator std::string() const { return display_name(); }

public:

	// -- Flags --

	// Returns true if this entity has the given flags.
	virtual bool has_flags(EntityFlag flags) const;
	// Returns true if this entity has these exact flags.
	virtual bool is_flags(EntityFlag flags) const;

	
	// Reset this entity's flags to the input.
	virtual void set_flags(EntityFlag flags); 
	// Add the input to this entity's flags.
	virtual void add_flag(EntityFlag flag); 
	// Remove the input from this entity's flags.
	virtual void remove_flag(EntityFlag flag); 
	// Switch the inputted flag on or off.
	virtual void set_flag_to(EntityFlag flag, bool set_to); 

	// Checks if EntityFlag::CAN_BE_HELD is set.
	virtual bool can_be_held() const { return m_flags & EntityFlag::CAN_BE_HELD; }
	// Checks if EntityFlag::CAN_BE_HELD is NOT set.
	virtual bool cannot_be_held() const { return !can_be_held(); }
	// Checks if EntityFlag::CAN_HOLD_ITEMS is set.
	virtual bool can_hold_items() const { return m_flags & EntityFlag::CAN_HOLD_ITEMS; }
	// Checks if EntityFlag::CAN_HOLD_ITEMS is NOT set.
	virtual bool cannot_hold_items() const { return !can_hold_items(); }

	// Checks if EntityFlag::NOT_MAP_BOUND is set.
	virtual bool is_not_map_bound() const { return m_flags & EntityFlag::NOT_MAP_BOUND; }
	// Checks if EntityFlag::NOT_MAP_BOUND is NOT set.
	virtual bool is_map_bound() const { return !is_not_map_bound(); }

	// Checks if EntityFlag::CAN_COLLIDE is set.
	virtual bool can_collide() const { return m_flags & EntityFlag::CAN_COLLIDE; }
	// Checks if EntityFlag::CAN_COLLIDE is NOT set.
	virtual bool has_no_collision() const { return !can_collide(); }

	// Checks if EntityFlag::IS_BLOCKING is set.
	virtual bool is_blocking() const { return m_flags & EntityFlag::IS_BLOCKING; }
	// Checks if EntityFlag::IS_BLOCKING is NOT set.
	virtual bool is_not_blocking() const { return !is_blocking(); }

	// Checks if EntityFlag::CAN_FALL is set.
	virtual bool can_fall() const { return m_flags & EntityFlag::CAN_FALL; }
	// Checks if EntityFlag::CAN_FALL is NOT set.
	virtual bool cannot_fall() const { return !can_fall(); }

	// Checks if EntityFlag::HAS_FOV is set.
	virtual bool has_field_of_view() const { return m_flags & EntityFlag::HAS_FOV; }

	// Checks if EntityFlag::ANIMATE is set.
	virtual bool is_animate() const { return m_flags & EntityFlag::ANIMATE; }
	// Checks if EntityFlag::ANIMATE is NOT set.
	virtual bool is_inanimate() const { return !is_animate(); }

	// Checks if EntityFlag::FREE_TO_MOVE is set.
	virtual bool free_to_move() const { return m_flags & EntityFlag::FREE_TO_MOVE; }
	// Checks if EntityFlag::FREE_TO_MOVE is NOT set.
	virtual bool fixed_in_place() const { return !free_to_move(); }

	// Checks if EntityFlag::IS_OPAQUE is set.
	virtual bool is_opaque() const { return m_flags & EntityFlag::IS_OPAQUE; }
	// Checks if EntityFlag::IS_OPAQUE is NOT set.
	virtual bool is_transparent() const { return !is_opaque(); }

	// Checks if EntityFlag::IS_ALIVE is set.
	virtual bool is_alive() const { return m_flags & EntityFlag::IS_ALIVE; }
	// Checks if EntityFlag::IS_ALIVE is NOT set.
	virtual bool is_dead() const { return !is_alive(); }

	// Checks if EntityFlag::IS_HIDDEN is set.
	virtual bool is_hidden() const { return m_flags & EntityFlag::IS_HIDDEN; }
	// Checks if EntityFlag::IS_FLASHING is set.
	virtual bool is_flashing() const { return m_flags& EntityFlag::IS_FLASHING; }

	// Checks if EntityFlag::IS_CREATURE is set.
	virtual bool is_creature() const { return m_flags & EntityFlag::IS_CREATURE; }
	// Checks if EntityFlag::IS_ITEM is set.
	virtual bool is_item() const { return m_flags & EntityFlag::IS_ITEM; }
	// Checks if EntityFlag::IS_FURNITURE is set.
	virtual bool is_furniture() const { return m_flags & EntityFlag::IS_FURNITURE; }

	// Checks if EntityFlag::CAN_DO_ACTIONS is set.
	virtual bool can_do_actions() const { return m_flags & EntityFlag::CAN_DO_ACTIONS; }

	// Checks if EntityFlag::CAN_INTERACT_WITH_OTHERS is set.
	virtual bool can_interact_with_others() const { return m_flags & EntityFlag::CAN_INTERACT_WITH_OTHERS; }
	// Checks if EntityFlag::CAN_INTERACT_WITH_OTHERS is NOT set.
	virtual bool cannot_interact_with_others() const { return !can_interact_with_others(); }

public:
	// - Field of View -- //

	bool can_see(const Coord& x, const Coord& y, const Coord& z) const;
	bool can_see(const Position& pos) const { return can_see(pos.x, pos.y, pos.z); }
	bool can_see(const Entity* entity) const;

	bool has_seen(const Coord& x, const Coord& y, const Coord& z) const;
	bool has_seen(const Position& pos) const { return has_seen(pos.x, pos.y, pos.z); }

public:
	// -- Utilities -- //

	bool operator==(const Entity& other) const;

	virtual Position escape_wall(const Position& pos) const;

	virtual bool is_player() const;
	virtual bool is_visible_to_player() const;

	virtual void on_flags_updated();
	virtual bool is_on_floor() const;
	virtual bool is_being_held() const;

	virtual bool should_flash();
	virtual void game_message(GameMessage msg);

public:
	// -- Game Loop -- //

	virtual void step(const TimeUnit& time_delta);
};



#endif // !LEMONADE_GAME_SRC_ENTITY_H