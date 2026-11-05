#pragma once

#ifndef LEMONADE_GAME_SRC_ENTITY_H
#define LEMONADE_GAME_SRC_ENTITY_H

#include "render_params.h"
#include "thing.h"
#include "time_units.h"
#include "inventory.h"

#include "action_option.h"

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
#include "furniture_def.h"
#include "ai_brain.h"
#include "entity_id.h"
#include "person.h"
#include "game_settings.h"
#include "line_of_sight.h"

#include <variant>
#include <vector>
#include <map>
#include <functional>

class TileMap;
class World;
class Game;
class WorldGameState;
struct GameMessage;
class Entity;

using EntityLocation = std::variant<World*, Entity*>;

enum class EntityFlag : int64_t
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

	IS_HIDDEN = 1 << 9,											// The entity cannot be seen.
	IS_FLASHING = 1 << 10,										// The entity is briefly not being rendered (has no effect on the world)

	IS_CREATURE = 1 << 11,										// The entity is a creature (whatever that means).
	IS_ITEM = 1 << 12,											// The entity is an item (whatever that means).
	IS_FURNITURE = 1 << 13,										// The entity is furniture (whatever that means).

	CAN_DO_ACTIONS = 1 << 14,									// The entity can queue up and perform actions.

	CAN_INTERACT_WITH_OTHERS = 1 << 15,							// The entity can interact with other things during actions.
	CANNOT_INTERACT_WITH_OTHERS = ~CAN_INTERACT_WITH_OTHERS,	// The entity can interact with other things during actions.

	NAME_IS_KNOWN = 1 << 16,									// This entity's name is known by the player.
	SHOULD_REMOVE = 1 << 17,									// This entity is queued for removal.

	IS_ENTITY_SPAWN = 1 << 18,									// NPCs generate from this entity.
	IS_ENTITY_CLEARER = 1 << 19,								// NPCs are removed once they reach this entity.
};

IS_FLAG_ENUM(EntityFlag);

struct RenderOverride
{
	size_t priority{ 0 };

	Renderable renderable{ ' ', color::get("white"), color::get("black") };

	bool override_glyph{ false };
	bool override_fg{ false };
	bool override_bg{ false };

	FrameInt flash_frames_on{ 0 };
	FrameInt flash_frames_off{ flash_frames_on };

	std::function<bool(const Entity*)> active;
};

template<typename T>
using ComponentRef = std::unique_ptr<T>;

using FallTimer = TimeUnit;

using EntityRawRefTuple = std::tuple<
	RawMaster::Ref<Species>,
	RawMaster::Ref<ItemDefinition>,
	RawMaster::Ref<FurnitureDefinition>
>;

using EntityComponentTuple = std::tuple<
	ComponentRef<CreatureSoul>,
	ComponentRef<TileMovable>,
	ComponentRef<Inventory>,
	ComponentRef<Anatomy>,
	ComponentRef<Gait>,
	ComponentRef<Living>,
	ComponentRef<AIBrain>,
	ComponentRef<Person>,
	ComponentRef<FallTimer>
>;

using EntityRef = Entity&;
using EntityPtr = Entity*;

class Action;

/* An entity represents anything in the world. */
class Entity : public Thing
{
protected:

	EntityID m_ID;
	EntityFlag m_flags;
	EntityLocation m_location; // What stores the entity. Usually the world but can also be another entity.

	EntityComponentTuple m_components;
	EntityRawRefTuple m_raw_refs;

	ActionQueue m_action_queue;

public:
	// -- Virtual Variables -- //

	std::vector<RenderOverride> render_overrides;

	virtual double move_speed() const; // Modifies how long it takes this entity to move a single tile.
	virtual double interact_speed() const; // Modifies how long it takes this entity to interact with something.
	virtual int view_range() const; // How far this entity can see

	virtual size_t default_inventory_slots() const; // How many inventory slots this entity has by default
	virtual size_t carry_slots() const; // How many inventory slots this entity takes up

	virtual size_t max_actions_queued() const; // How many actions this entity can queue at once

public:
	Entity(EntityID ID, EntityFlag entity_flags = EntityFlag::NONE);
	~Entity();

	Entity(const Entity& other);
	Entity(Entity&& other);

public:

	// -- Getters & Setters --

	virtual EntityID ID() const { return m_ID; }

	WorldGameState* get_world_state() const;
	Game* get_game() const;

	Entity* get_player() const;
	EntityID get_player_id() const;
	bool does_player_exist() const;

	TileMap* get_tile_map() const;
	World* get_world() const;

	EntityLocation get_location() const { return m_location; } // Return variant of this entity's location. Can be either a pointer to the world or a another entity.
	Inventory* get_holder_inventory() const;

	Entity* get_entity_holder() const; // Returns a pointer to the entity holding this entity (if it exists)
	World* get_world_holder() const; // Returns a pointer to the world holding this entity (if it exists)

public:

	// -- Components -- //

	template<typename T>
	T* get_component() const { return std::get<ComponentRef<T>>(m_components).get(); }

	template<typename T>
	bool has_component() const { return std::get<ComponentRef<T>>(m_components) != nullptr; }

	template<typename T>
	T* set_component(T* new_component)
	{
		std::get<ComponentRef<T>>(m_components).reset(new_component);
		return std::get<ComponentRef<T>>(m_components).get();
	}

	template<typename T>
	T* release_component()
	{
		ComponentRef<T> removed_component = std::move(std::get<ComponentRef<T>>(m_components));
		return removed_component.release();
	}

	template<typename T>
	void remove_component()
	{
		ComponentRef<T> removed_component = std::move(std::get<ComponentRef<T>>(m_components));
		removed_component.reset();
	}

	template<typename T>
	const T* get_raw_ref() const { return std::get<RawMaster::Ref<T>>(m_raw_refs).get(); }

	template<typename T>
	bool has_raw_ref() const { return std::get<RawMaster::Ref<T>>(m_raw_refs) != nullptr; }

	template<typename T>
	const T* set_raw_ref(const T* new_component)
	{
		std::get<RawMaster::Ref<T>>(m_raw_refs).reset(new_component);
		return std::get<RawMaster::Ref<T>>(m_raw_refs).get();
	}

	template<typename T>
	const T* set_raw_ref(RawMaster::Ref<T> new_component)
	{
		std::get<RawMaster::Ref<T>>(m_raw_refs) = new_component;
		return std::get<RawMaster::Ref<T>>(m_raw_refs).get();
	}

public:
	// Actions //

	virtual bool can_queue_action(QueuedAction action, bool print_errors = false) const;
	virtual void reset_actions(QueuedAction action = nullptr);
	virtual bool start_action(QueuedAction action);

	virtual void continue_action(const TimeUnit& delta);
	virtual void complete_action();

	virtual bool can_queue_action() const
	{
		return m_action_queue.size() < max_actions_queued();
	}

	virtual bool has_any_action() const
	{
		return has_queued_action() || has_current_action();
	}

	virtual bool has_queued_action() const
	{
		return m_action_queue.size() > 1;
	}

	virtual bool has_current_action() const
	{
		return !m_action_queue.empty();
	}

	virtual bool has_action_name(std::string action_name) const;

	virtual double get_action_speed(Action* action) const;

	virtual const Action* get_current_action() const { return m_action_queue.back(); }
	virtual Action* get_current_action() { return m_action_queue.back(); }

	virtual bool can_interact_with(Entity* subject) const;
	virtual std::vector<InteractionOption> get_interactions_with(Entity* subject);

	// Inventory  //

	// Adds an `Inventory` component with the set amount of max slots.
	Inventory* setup_inventory();

	// Adds an item to this entity's `Inventory` component. Returns true if adding the item was successful.
	bool add_to_inventory(Entity* item);
	// Removes an item from this entity's `Inventory` component.
	void remove_from_inventory(Entity* item, bool update_inventory = true);

	// Sets this entity's `Inventory` component to the input. Automatically sets the owner of the inventory to this entity.
	virtual void set_inventory(Inventory* inventory) { set_component<Inventory>(inventory)->set_owner(this); }
	// Returns this entity's `Inventory` component, if it exists.
	virtual Inventory* get_inventory() { return get_component<Inventory>(); }
	virtual const Inventory* get_inventory() const { return get_component<Inventory>(); }
	// Returns true if this entity has an `Inventory` component.
	virtual bool has_inventory() const { return has_component<Inventory>(); }

	// Location //

	// Sets this entity's holder to the input. Automatically removes them from another's inventory if necessary.
	bool set_location(Entity* entity, bool change_holder = true);
	// Sets this entity's holder to the input. Automatically removes them from another's inventory if necessary.
	bool set_location(World* world, bool change_holder = true);

	// Movable //

	// Returns this entity's `Movable` component, if it exists.
	virtual TileMovable* get_movable() { return get_component<TileMovable>(); }
	virtual const TileMovable* get_movable() const { return get_component<TileMovable>(); }
	// Returns true if this entity has a `Movable` component.
	virtual bool has_movable() const { return has_component<TileMovable>(); }
	// Sets this entity's `Movable` component to the input.
	virtual void set_movable(const TileTransform& transform) { set_component<TileMovable>(new TileMovable(transform)); }
	virtual void set_movable(const TileMovable& movable) { set_component<TileMovable>(new TileMovable(movable)); }

	virtual void set_movable(TileMovable* movable) { set_component<TileMovable>(movable); }

	// Species //

	// Returns true if this entity has a `Species` component.
	virtual bool has_species() const { return has_raw_ref<Species>(); }
	// Returns this entity's `Species` component, if it exists.
	virtual const Species* get_species() const { return get_raw_ref<Species>(); }

	// Sets this entity's `Species` component to the input.
	virtual void set_species(const Species* species) { set_raw_ref<Species>(species); }
	// Sets this entity's `Species` component to the input.
	virtual void set_species(RawMaster::Ref<Species> species) { set_raw_ref<Species>(species); }
	// Sets this entity's `Species` component to the input (this time using a species ID).
	virtual void set_species(RawMaster::RawID species_id) { set_raw_ref<Species>(RawMaster::get<Species>(species_id)); }

	// Item Definition //

	// Returns this entity's `ItemDefinition` component, if it exists.
	virtual const ItemDefinition* get_item_def() const { return get_raw_ref<ItemDefinition>(); }
	// Sets this entity's `ItemDefinition` component to the input.
	virtual void set_item_def(RawMaster::Ref<ItemDefinition> item_def) { set_raw_ref<ItemDefinition>(item_def); }
	// Returns true if this entity has a `ItemDefinition` component.
	virtual bool has_item_def() const { return has_raw_ref<ItemDefinition>(); }

	// Furniture Definition //

	// Returns this entity's `ItemDefinition` component, if it exists.
	virtual const FurnitureDefinition* get_furniture_def() const { return get_raw_ref<FurnitureDefinition>(); }
	// Sets this entity's `ItemDefinition` component to the input.
	virtual void set_furniture_def(RawMaster::Ref<FurnitureDefinition> furniture_def) { set_raw_ref<FurnitureDefinition>(furniture_def); }
	// Returns true if this entity has a `FurnitureDefinition` component.
	virtual bool has_furniture_def() const { return has_raw_ref<FurnitureDefinition>(); }

	// Anatomy //

	// Returns this entity's `Anatomy` component, if it exists.
	virtual Anatomy* get_anatomy() { return get_component<Anatomy>(); }
	virtual const Anatomy* get_anatomy() const { return get_component<Anatomy>(); }
	// Sets this entity's `Anatomy` component to the input.
	virtual void set_anatomy(Anatomy* anatomy) { set_component<Anatomy>(anatomy)->set_owner(this); }
	// Returns true if this entity has a `Anatomy` component.
	virtual bool has_anatomy() const { return has_component<Anatomy>(); }
	

	// Gait //

	virtual bool has_gait() const { return has_component<Gait>(); }

	// Returns this entity's `Gait` component, if it exists.
	virtual Gait* get_gait() { return get_component<Gait>(); }
	virtual const Gait* get_gait() const { return get_component<Gait>(); }
	// Sets this entity's `Gait` component to the input.
	virtual void set_gait(Gait* gait) { set_component<Gait>(gait); }

	// Changes this entity's gait type only if they have a `Gait` component.
	virtual void change_gait_type(std::string gait_id)
	{
		if (has_component<Gait>())
		{
			get_component<Gait>()->set_current_gait(gait_id);
		}
	}

	// Living //

	// Returns true if this entity has a `Living` component.
	virtual bool has_living() const { return has_component<Living>(); }
	// Returns this entity's `Living` component, if it exists.
	virtual Living* get_living() { return get_component<Living>(); }
	virtual const Living* get_living() const { return get_component<Living>(); }
	// Sets this entity's `Living` component to the input.
	virtual void set_living(Living* living) { set_component<Living>(living)->set_owner(this); }

	// Soul //

	// Sets this entity's `CreatureSoul` component to the input.
	virtual void add_soul(CreatureSoul* soul)
	{
		set_component<CreatureSoul>(soul);
		add_flag(EntityFlag::ANIMATE);
	}

	// Removes this entity's `CreatureSoul` component and returns its unique_ptr.
	virtual CreatureSoul* release_soul()
	{
		remove_flag(EntityFlag::ANIMATE);
		return release_component<CreatureSoul>();
	}

	// Returns true if this entity has a `CreatureSoul` component.
	virtual bool has_soul() const { return has_component<CreatureSoul>(); }
	// Returns this entity's `CreatureSoul` component, if it exists.
	virtual CreatureSoul* get_soul() const { return get_component<CreatureSoul>(); }

	// Brain //

	virtual AIBrain* set_brain(AIBrain* brain)
	{
		set_component<AIBrain>(brain)->set_entity(this);
		return get_component<AIBrain>();
	}

	virtual AIBrain* remove_brain() { return release_component<AIBrain>(); }

	virtual AIBrain* get_brain() { return get_component<AIBrain>(); }
	virtual const AIBrain* get_brain() const { return get_component<AIBrain>(); }

	virtual bool has_brain() const { return has_component<AIBrain>(); }

	// Person //

	virtual Person* get_person() { return get_component<Person>(); }
	virtual const Person* get_person() const { return get_component<Person>(); }
	virtual bool is_person() const { return has_component<Person>(); }
	virtual void set_person(Person* person) { set_component<Person>(person)->set_owner(this); }

public:
	// Thing //

	// For entities larger than 1x1x1, return the renderable at that coordinate, relative to the entity's position.
	virtual Renderable get_renderable_at(Coord offset_x, Coord offset_y, Coord offset_z) const;
	// For entities larger than 1x1x1, return the renderable at that coordinate, relative to the entity's position.
	virtual Renderable get_renderable_at(Position offset) const { return get_renderable_at(offset.x, offset.y, offset.z); }

	virtual Renderable renderable() const override;

	virtual Noun noun() const override;
	virtual Name name() const override;
	virtual RawMaster::Ref<Pronouns> pronouns() const override;

	virtual bool is_plural() const;

	virtual std::string prefix() const;
	virtual std::string suffix() const;

	virtual std::string description() const;

	virtual std::string display_name() const;
	virtual GrammarNoun display_noun() const;

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

	// Checks if EntityFlag::IS_CREATURE is set.
	virtual bool is_creature() const { return m_flags & EntityFlag::IS_CREATURE; }
	// Checks if EntityFlag::IS_ITEM is set.
	virtual bool is_item() const { return m_flags & EntityFlag::IS_ITEM; }
	// Checks if EntityFlag::IS_FURNITURE is set.
	virtual bool is_furniture() const { return m_flags & EntityFlag::IS_FURNITURE; }

	virtual EntityFlag type_flag() const
	{
		if (is_creature())
			return EntityFlag::IS_CREATURE;
		if (is_item())
			return EntityFlag::IS_ITEM;
		if (is_furniture())
			return EntityFlag::IS_FURNITURE;
		return EntityFlag::NONE;
	}

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

	// Checks if EntityFlag::IS_HIDDEN is set.
	virtual bool is_hidden() const { return m_flags & EntityFlag::IS_HIDDEN; }
	// Checks if EntityFlag::IS_FLASHING is set.
	virtual bool is_flashing() const { return m_flags & EntityFlag::IS_FLASHING; }

	// Checks if EntityFlag::CAN_DO_ACTIONS is set.
	virtual bool can_do_actions() const { return m_flags & EntityFlag::CAN_DO_ACTIONS; }

	// Checks if EntityFlag::CAN_INTERACT_WITH_OTHERS is set.
	virtual bool can_interact_with_others() const { return m_flags & EntityFlag::CAN_INTERACT_WITH_OTHERS; }
	// Checks if EntityFlag::CAN_INTERACT_WITH_OTHERS is NOT set.
	virtual bool cannot_interact_with_others() const { return !can_interact_with_others(); }

	// Checks if EntityFlag::SHOULD_REMOVE is set.
	virtual bool should_remove() const { return m_flags & EntityFlag::SHOULD_REMOVE; }
	// Queues an entity to be removed on the next `step()` call.
	virtual void queue_remove() { add_flag(EntityFlag::SHOULD_REMOVE); }

	// Checks if EntityFlag::NAME_IS_KNOWN is set.
	virtual bool is_name_known() const { return m_flags & EntityFlag::NAME_IS_KNOWN; }
	virtual bool is_name_unknown() const { return !is_name_known(); }

	// The player learns this entity's name.
	virtual void learn_name() { add_flag(EntityFlag::NAME_IS_KNOWN); }
	// The player forgets this entity's name.
	virtual void forget_name() { remove_flag(EntityFlag::NAME_IS_KNOWN); }

	// Checks if EntityFlag::IS_ENTITY_SPAWN is set.
	virtual bool is_entity_spawn() const { return m_flags & EntityFlag::IS_ENTITY_SPAWN; }

	// Checks if EntityFlag::IS_ENTITY_CLEARER is set.
	virtual bool is_entity_clearer() const { return m_flags & EntityFlag::IS_ENTITY_CLEARER; }

public:
	// - Field of View -- //

	bool can_see(Coord x, Coord y, Coord z) const;
	bool can_see(const Position& pos) const { return can_see(pos.x, pos.y, pos.z); }
	bool can_see(const Entity* entity) const;

	bool has_seen(Coord x, Coord y, Coord z) const;
	bool has_seen(const Position& pos) const { return has_seen(pos.x, pos.y, pos.z); }

public:
	// -- Utilities -- //

	bool operator==(const Entity& other) const;
	Entity& operator=(Entity& other);
	Entity& operator=(const Entity& other);

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

public:

	void copy_components_from_entity(const Entity& other);
	void move_components_from_entity(Entity& other);
	void fix_ownership();
};




#endif // !LEMONADE_GAME_SRC_ENTITY_H