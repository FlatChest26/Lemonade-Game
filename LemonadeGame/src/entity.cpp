#include "entity.h"

#include "game.h"
#include "world_state.h"
#include "world.h"

#include "game_log.h"

#include "action.h"

#include "snowy_utilities.h"
#include "action.h"


void Entity::copy_components_from_entity(const Entity& other)
{
	if (other.has_soul())
	{
		add_soul(new CreatureSoul(*other.soul()));
	}

	if (other.has_movable())
	{
		set_movable(new TileMovable(*other.movable()));
	}

	if (other.has_anatomy())
	{
		set_anatomy(new Anatomy(*other.anatomy()));
	}

	if (other.has_inventory())
	{
		if (!has_inventory())
		{
			setup_inventory();
		}
		else
		{
			inventory()->clear_items(true);
		}

		for (const auto& item : other.inventory()->get_items())
		{
			add_to_inventory(item);
		}
	}

	if (other.has_gait())
	{
		set_gait(new Gait(*other.gait()));
	}

	if (other.has_living())
	{
		set_living(new Living(*other.living()));
	}

	if (other.is_person())
	{
		set_person(new Person(*other.person()));
	}

	if (other.has_brain())
	{
		set_brain(new AIBrain(*other.ai_brain()));
	}
}

void Entity::move_components_from_entity(Entity& other)
{
	if (other.has_soul())
	{
		add_soul(other.release_soul());
	}

	if (other.has_movable())
	{
		set_movable(other.release_component<TileMovable>());
	}

	if (other.has_inventory())
	{
		set_inventory(other.release_component<Inventory>());
	}

	if (other.has_anatomy())
	{
		set_anatomy(other.release_component<Anatomy>());
	}

	if (other.has_gait())
	{
		set_gait(other.release_component<Gait>());
	}

	if (other.has_living())
	{
		set_living(other.release_component<Living>());
	}

	if (other.is_person())
	{
		set_person(other.release_component<Person>());
	}

	if (other.has_brain())
	{
		set_brain(other.release_component<AIBrain>());
	}
}

void Entity::fix_ownership()
{
	if (has_anatomy())
	{
		anatomy()->set_owner(this);
	}

	if (has_inventory())
	{
		inventory()->set_owner(this);

		for (auto& item : inventory()->get_items())
		{
			item->set_location(this);
		}
	}

	if (has_living())
	{
		living()->set_owner(this);
	}

	if (is_person())
	{
		person()->set_owner(this);
	}

	if (has_brain())
	{
		ai_brain()->set_entity(this);
	}

	for (auto& action : m_action_queue)
	{
		action->set_actor(this);
	}
}


Entity::Entity(EntityID ID, EntityFlag entity_flags) :
	m_ID(ID), m_flags(entity_flags)
{
	on_flags_updated();
	m_action_queue.reserve(max_actions_queued());
}

Entity::~Entity()
{
	
}

Entity::Entity(const Entity& other)
{
	m_ID = other.m_ID;
	m_flags = other.m_flags;

	m_location = other.m_location;

	m_raw_refs = other.m_raw_refs;
	copy_components_from_entity(other);

	render_overrides = other.render_overrides;

	on_flags_updated();
	m_action_queue.reserve(max_actions_queued());
}

Entity::Entity(Entity&& other)
{
	m_ID = other.m_ID;
	m_flags = other.m_flags;

	m_location = other.m_location;
	m_action_queue = std::move(other.m_action_queue);

	m_raw_refs = other.m_raw_refs;
	move_components_from_entity(other);

	render_overrides = std::move(other.render_overrides);

	on_flags_updated();
	m_action_queue.reserve(max_actions_queued());
}

double Entity::move_speed() const
{
	if (has_gait())
	{
		return gait()->get_current_speed();
	}
	else if (is_player())
	{
		return 0.7;
	}
	else if (is_creature())
	{
		return 1.2;
	}

	return 1.0f;
}

double Entity::interact_speed() const
{
	return 0.5f;
}

int Entity::view_range() const
{
	if (has_field_of_view())
	{
		if (has_anatomy() && !anatomy()->has_body_parts({ .required_flags = BodyPartFlag::SIGHT }))
		{
			return 1;
		}

		return 32;
	}

	return 0;
}

size_t Entity::default_inventory_slots() const
{
	if (has_anatomy())
	{
		return anatomy()->count_body_parts({ .required_flags = BodyPartFlag::GRASP });
	}

	return 1;
}

size_t Entity::carry_slots() const
{
	if (has_item_def())
	{
		return item_def()->carry_slots;
	}

	if (is_creature())
	{
		return 2;
	}

	return 1;
}

size_t Entity::max_actions_queued() const
{
	return 2;
}

Inventory* Entity::get_holder_inventory() const
{
	if (auto holder = get_entity_holder())
	{
		return holder->inventory();
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

WorldGameState* Entity::get_world_state() const
{
	return get_world()->get_world_state();
}

Game* Entity::get_game() const
{
	return get_world()->get_game();
}

Entity* Entity::get_player() const
{
	return get_world_state()->get_player();
}

bool Entity::does_player_exist() const
{
	return get_world_state()->does_player_exist();
}

TileMap* Entity::get_tile_map() const
{
	return get_world()->get_tile_map();
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
			auto inventory = holder->inventory();
			if (inventory && holder->inventory()->has_item(this))
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
			auto inventory = holder->inventory();
			if (inventory && holder->inventory()->has_item(this))
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
		desc += soul()->description;
	}

	if (has_item_def())
	{
		desc += " " + item_def()->description;
	}

	if (has_species())
	{
		desc += " " + species()->description;
	}

	return grammar::trim(desc);
}

std::string Entity::display_name() const
{
	return grammar::a(display_noun(), false);
}

GrammarNoun Entity::display_noun() const
{
	GrammarNoun display_noun = GrammarNoun::from_noun(noun());

	if (is_player() && GameSettings::USE_SECOND_PERSON_PRONOUNS)
	{
		display_noun = GrammarNoun::from_string("you", true);
		display_noun.noun.always_plural = true;
	}
	else if (is_name_known())
	{
		display_noun = GrammarNoun::from_string(nickname(), true);
	}

	else if (has_soul() && soul()->gender->uses_noun)
	{
		auto& gender = soul()->gender;

		Noun gender_noun = gender->mature_noun;

		bool species_mention = false;

		if (has_living())
		{
			if (living()->is_young())
			{
				if (has_species() && species()->gendered_nouns.has_immature_gendered_noun(gender->gender_type))
				{
					gender_noun = species()->gendered_nouns.get_immature_gendered_noun(gender->gender_type);
					species_mention = true;
				}
				else
				{
					gender_noun = gender->immature_noun;
				}
			}
			else
			{
				if (has_species() && species()->gendered_nouns.has_mature_gendered_noun(gender->gender_type))
				{
					gender_noun = species()->gendered_nouns.get_mature_gendered_noun(gender->gender_type);
					species_mention = true;
				}
				else
				{
					gender_noun = gender->mature_noun;
				}
			}
		}

		if (species_mention)
		{
			display_noun = gender_noun;
		}
		else
		{
			display_noun.noun.singular = display_noun.noun.singular + " " + gender_noun.singular;
			display_noun.noun.plural = display_noun.noun.singular + " " + gender_noun.plural;
		}
	}

	if (has_living() && living()->is_dead())
	{
		display_noun = grammar::poss(display_noun) + " corpse";
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

Renderable Entity::renderable_at(Coord offset_x, Coord offset_y, Coord offset_z) const
{
	if (has_furniture_def())
	{
		return furniture_def()->renderable_at(offset_x, offset_y, offset_z);
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
		renderable = soul()->renderable();
	}

	if (has_item_def())
	{
		renderable = item_def()->renderable;
	}

	if (has_species())
	{
		renderable = species()->renderable;
	}

	if (is_named() && is_name_known())
	{
		renderable.glyph = nickname()[0];

		if (has_living() && !living()->is_adult())
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
		renderable.fg = soul()->color;
	}

	return renderable;
}

Noun Entity::noun() const
{
	if (has_species())
	{
		return species()->noun;
	}

	if (has_item_def())
	{
		return item_def()->noun;
	}

	if (has_soul())
	{
		return soul()->noun();
	}

	return Noun();
}

Name Entity::name() const
{
	if (has_soul())
	{
		return soul()->name;
	}

	return Name();
}

RawMaster::Ref<Pronouns> Entity::pronouns() const
{
	if (is_player() && GameSettings::USE_SECOND_PERSON_PRONOUNS)
	{
		return RawMaster::get<Pronouns>("you/your");
	}

	if (has_soul())
	{
		return soul()->gender->pronouns;
	}

	return is_animate() ? RawMaster::get<Pronouns>("it/its") : RawMaster::get<Pronouns>("they/them");
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

	if (does_player_exist())
	{
		return get_player()->can_see(this);
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

	auto tile = world->get_tile(movable()->pos());

	if (tile->has_floor() || tile->can_descend())
	{
		return true;
	}

	auto tile_below = world->get_tile(movable()->pos() + Position(0, 0, -1));

	if (tile_below->has_ceiling())
	{
		return true;
	}

	return world->is_blocked(movable()->posx(), movable()->posy(), movable()->posz() - 1, this, -1);
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
		if (inventory()->has_item(subject))
		{
			return true;
		}

		bool is_adjacent = true;

		if (has_movable())
		{
			if (!subject->has_movable())
			{
				is_adjacent = false;
			}
			else
			{
				is_adjacent = transforms::is_adjacent(movable()->pos(), subject->movable()->pos());
			}
		}

		if (!inventory()->has_item(subject) && inventory()->can_carry(subject) && is_adjacent)
		{
			return true;
		}
	}

	if (subject->has_item_def() && subject->item_def()->is_consumable())
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
		if (inventory()->has_item(subject))
		{
			potential_actions.push_back({
				.name = "drop",
				.input_action = Keybind::Drop,
				.action = [](Entity* actor, Entity* subject) { return new DropAction(actor, subject); }
			});
		}

		bool is_adjacent = true;

		if (has_movable())
		{
			if (!subject->has_movable())
			{
				is_adjacent = false;
			}
			else
			{
				is_adjacent = transforms::is_adjacent(movable()->pos(), subject->movable()->pos());
			}
		}

		if (!inventory()->has_item(subject) && inventory()->can_carry(subject) && is_adjacent)
		{
			potential_actions.push_back({
				.name = "grab",
				.input_action = Keybind::Grab,
				.action = [](Entity* actor, Entity* subject) { return new GrabAction(actor, subject); }
			});
		}
	}

	if (subject->has_item_def() && subject->item_def()->is_consumable())
	{
		potential_actions.push_back({
			.name = "consume",
			.input_action = Keybind::Consume,
			.action = [](Entity* actor, Entity* subject) { return new ConsumeAction(actor, subject); }
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
		inventory()->set_max_slots(default_inventory_slots());
	}

	return inventory();
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

	if (!inventory()->add_item(item))
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
		inventory()->remove_item(item);
	}

	item->set_location(get_world(), false);
}

bool Entity::can_queue_action(Action* action, bool print_errors) const
{
	if (!action)
	{
		if (print_errors) CERR("Failed to start action! Action was null. " << ID());
		return false;
	}

	if (!can_do_actions())
	{
		if (print_errors) CERR("Failed to start action! Entity cannot do actions. " << ID());
		return false;
	}

	if (!can_queue_action())
	{
		if (print_errors) CERR("Failed to start action! Too many actions queued. " << ID());
		return false;
	}

	if (has_action(action))
	{
		return false;
	}

	if (!action->check_action())
	{
		return false;
	}

	if (action->has_subject() && cannot_interact_with_others())
	{
		if (print_errors) CERR("Failed to start action! Entity cannot interact with others. " << ID());
		return false;
	}

	return true;
}

bool Entity::start_action(Action* action)
{
	return start_action(QueuedAction(action));
}

bool Entity::start_action(QueuedAction action)
{
	if (!can_queue_action(action.get(), true))
	{
		return false;
	}

	m_action_queue.push_back(std::move(action));

	if (get_current_action()->progress(TimeUnit::none()))
		complete_action();

	return true;
}

bool Entity::has_action(Action* action) const
{
	for (auto& other_action : m_action_queue)
	{
		if (
			other_action->action_name() == action->action_name() && 
			other_action->get_actor() == action->get_actor() &&
			other_action->get_subject() == action->get_subject()
			)
			return true;
	}

	return false;
}

void Entity::reset_actions(Action* action)
{
	reset_actions(QueuedAction(action));
}

void Entity::reset_actions(QueuedAction action)
{
	if (action && !can_queue_action(action.get(), true))
		return;

	m_action_queue.clear();

	if (action)
	{
		start_action(std::move(action));
	}
}

void Entity::continue_action(const TimeUnit& delta)
{
	if (!has_current_action())
		return;

	if (get_current_action()->progress(delta))
	{
		complete_action();
	}
}

void Entity::complete_action()
{
	if (!has_current_action())
		return;

	auto next_action = std::move(m_action_queue.back());
	m_action_queue.pop_back();

	if (auto world_state = get_world_state())
	{
		world_state->handle_action(std::move(next_action));
	}
}

void Entity::grapple_other(Entity* other)
{
	if (!other->can_be_grappled())
		return;

	set_component<Grapple>(new Grapple(this, other));
	other->set_component<Grapple>(new Grapple(this, other));
}

void Entity::release_grapple()
{
	if (!is_grappling_other())
		return;

	if (has_grapple())
	{
		get_grapple()->target->remove_component<Grapple>();
		remove_component<Grapple>();
	}
}

void Entity::escape_grapple()
{
	if (!is_being_grappled())
		return;

	if (has_grapple())
	{
		get_grapple()->owner->remove_component<Grapple>();
		remove_component<Grapple>();
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
	else if (has_movable())
	{
		if ((movable()->pos() - Position(x, y, z)).length_squared() > (view_range() * view_range()))
		{
			return false;
		}

		auto result = los::raycast3D(
			get_world(),
			movable()->pos(),
			Position(x, y, z),
			los::RaycastParams
			{
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

	for (const auto& [px, py, pz] : entity->movable()->get_points())
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

bool Entity::can_move_on_own() const
{
	if (!has_movable())
		return false;

	if (is_being_grappled())
		return false;

	if (is_being_held())
		return false;

	return true;
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

	if (!can_do_actions())
	{
		m_action_queue.clear();
	}
}

bool Entity::should_flash() const
{
	if (is_being_held() || is_hidden() || !has_movable())
	{
		return false;
	}

	if (auto world = get_world())
	{
		auto entities_in_tile = world->get_entities_at(
			movable()->pos(), 
			EntityFilter
			{
				.ignore_flags = EntityFlag::IS_HIDDEN,
				.required_flags = type_flag(),
			}
		);

		if (entities_in_tile.size() < 2)
		{
			return false;
		}

		auto i = std::distance(
			entities_in_tile.begin(),
			std::find_if(
				entities_in_tile.begin(), entities_in_tile.end(),
				[&](Entity* entity)
				{
					return entity == this;
				}
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
	if (auto world_state = get_world_state())
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

	for (const auto& check_pos : transforms::get_surrounding_locations(pos))
	{
		if (!check_block(check_pos))
		{
			return check_pos;
		}
	}

	return pos;
}

void Entity::step(const TimeUnit& time_delta)
{
	if (has_movable())
	{
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
				movable()->move(0, 0, -1);
				*get_component<FallTimer>() -= TimeUnit::from_seconds(1.0); // Reset the fall timer
			}
		}

		if (is_being_held() && get_entity_holder()->has_movable())
		{
			movable()->pos() = get_entity_holder()->movable()->pos() - movable()->get_center();
			movable()->direction() = get_entity_holder()->movable()->direction();
		}

		if (is_being_grappled() && get_grapple_owner()->has_movable())
		{
			auto grapple_owner = get_grapple_owner();
			movable()->pos() = escape_wall(transforms::get_closest_adjacent(movable()->pos(), grapple_owner->movable()->pos()));
		}

		// Try to escape wall
		if (!is_being_held() && can_collide() && free_to_move())
		{
			movable()->pos() = escape_wall(movable()->pos());
		}
	}

	// Handle current action
	if (can_do_actions())
	{
		continue_action(time_delta);
	}

	if (has_anatomy())
	{
		anatomy()->step(time_delta);
	}
	
	if (has_living())
	{
		living()->step(time_delta);
	}

	if (has_brain())
	{
		ai_brain()->step(time_delta);
	}

	if (has_brain() && has_living() && living()->is_dead())
	{
		remove_brain();
	}
}