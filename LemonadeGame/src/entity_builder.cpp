#include "entity_builder.h"
#include "anatomy.h"
#include "gait.h"

#include "world.h"
#include "entity_filter.h"

void EntityBuilder::default_entity_render_overrides(Entity* entity, std::vector<RenderOverride>& render_overrides)
{
	// Falling bg
	if (entity->can_fall())
	{
		render_overrides.push_back({
			.priority = 1,
			.renderable = Renderable{ .bg = color::get("cyan") },
			.override_bg = true,
			.frames_on = 30,
			.active = [](const Entity* entity)
				{
					return !entity->is_on_floor();
				}
		});
	}


	if (entity->has_living())
	{
		// Dead bg
		render_overrides.push_back({
			.priority = 2,
			.renderable = Renderable{ .bg = color::get("red") },
			.override_bg = true,
			.active = [](const Entity* entity)
				{
					return entity->get_living()->is_dead();
				}
		});

		// Sleeping renderable
		render_overrides.push_back({
			.priority = 3,
			.renderable = Renderable{ .glyph = 'Z', .fg = color::get("white"), .bg = color::get("blue") },
			.override_glyph = true,
			.override_fg = true,
			.override_bg = true,
			.frames_on = 50,
			.active = [](const Entity* entity)
				{
					return entity->get_living()->is_asleep();
				}
		});

	}

	if (entity->has_anatomy())
	{
		// Crawling bg
		render_overrides.push_back({
			.priority = 4,
			.renderable = Renderable{ .bg = color::get("orange") },
			.override_bg = true,
			.active = [](const Entity* entity)
				{
					return entity->get_anatomy()->posture().get_current_posture()->type == PostureType::CRAWL;
				}
			});

	}

	// Grapple bg

	render_overrides.push_back({
		.priority = 6,
		.renderable = Renderable{ .bg = color::get("violet") },
		.override_bg = true,
		.active = [](const Entity* entity)
			{
				return entity->has_grapple();
			}
	});


	std::sort(render_overrides.begin(), render_overrides.end(),
		[](const RenderOverride& a, const RenderOverride& b)
		{
			return a.priority < b.priority;
		}
	);
}

Entity EntityBuilder::create_creature(EntityID ID, CreatureInitializer init)
{
	auto new_creature = Entity(ID);

	// Soul
	new_creature.add_soul(init.soul);

	// Flags
	new_creature.add_flag(EntityFlag::IS_CREATURE);
	new_creature.add_flag(EntityFlag::FREE_TO_MOVE);
	// new_creature.add_flag(EntityFlag::IS_BLOCKING);
	new_creature.add_flag(EntityFlag::CAN_COLLIDE);
	new_creature.add_flag(EntityFlag::CAN_FALL);
	new_creature.add_flag(EntityFlag::HAS_FOV);
	new_creature.add_flag(EntityFlag::CAN_HOLD_ITEMS);
	new_creature.add_flag(EntityFlag::CAN_BE_HELD);
	new_creature.add_flag(EntityFlag::ANIMATE);
	new_creature.add_flag(EntityFlag::CAN_DO_ACTIONS);
	new_creature.add_flag(EntityFlag::CAN_INTERACT_WITH_OTHERS);
	new_creature.add_flag(EntityFlag::CAN_BE_GRAPPLED);

	// Species
	if (init.species)
	{
		auto species = init.species;
		new_creature.set_species(species);

		// Body Plan
		auto combined_body_mods = species->body_mods;

		combined_body_mods.reserve(init.body_mods.size() + species->body_mods.size());
		combined_body_mods.insert(combined_body_mods.end(), init.body_mods.begin(), init.body_mods.end());

		new_creature.set_anatomy(new Anatomy(species->default_body_plan, combined_body_mods));

		// Gaits
		new_creature.set_gait(new Gait(species->gaits));

		// Living
		new_creature.set_living(new Living(&new_creature, init.current_age, init.lifespan));
		new_creature.get_living()->add_defaults();

		for (auto& [ID, init_amount] : init.initial_stats)
		{
			if (!new_creature.get_living()->has_stat(ID))
			{
				CERR("Invalid stat ID: " << ID);
				continue;
			}

			new_creature.get_living()->get_stat(ID).set_value(init_amount);
		}

		// Person
		if (species->is_part_of_society())
			new_creature.set_person(new Person(&new_creature, init.starting_cash));

		// Inventory
		new_creature.get_inventory()->set_max_slots(new_creature.default_inventory_slots());
	}

	if (!init.species)
	{
		CERR("Creature: " << ID << " has no species assigned!");
	}

	default_entity_render_overrides(&new_creature, new_creature.render_overrides);

	return new_creature;
}

Entity EntityBuilder::create_item(EntityID ID, ItemInitializer init)
{
	auto new_item = Entity(ID);

	new_item.add_flag(EntityFlag::IS_ITEM);
	new_item.add_flag(EntityFlag::FREE_TO_MOVE);
	new_item.add_flag(EntityFlag::CAN_COLLIDE);
	new_item.add_flag(EntityFlag::INANIMATE);
	new_item.add_flag(EntityFlag::CAN_FALL);
	new_item.add_flag(EntityFlag::CAN_BE_HELD);

	//new_item.add_flag(EntityFlag::CAN_HOLD_ITEMS);

	new_item.set_item_def(init.item_definition);

	default_entity_render_overrides(&new_item, new_item.render_overrides);

	return new_item;
}

Entity EntityBuilder::create_furniture(EntityID ID, FurnitureInitializer init)
{
	auto new_furniture = Entity(ID);

	new_furniture.add_flag(EntityFlag::IS_FURNITURE);
	new_furniture.add_flag(EntityFlag::FIXED_IN_PLACE);
	new_furniture.add_flag(EntityFlag::CAN_COLLIDE);
	new_furniture.add_flag(EntityFlag::INANIMATE);
	new_furniture.add_flag(EntityFlag::CAN_FALL);

	//new_furniture.add_flag(EntityFlag::CAN_HOLD_ITEMS);
	new_furniture.add_flag(EntityFlag::CAN_BE_HELD);

	if (init.furniture_definition->is_blocking())
		new_furniture.add_flag(EntityFlag::IS_BLOCKING);

	if (init.furniture_definition->is_opaque())
		new_furniture.add_flag(EntityFlag::IS_OPAQUE);

	new_furniture.set_furniture_def(init.furniture_definition);
	new_furniture.set_item_def(init.furniture_definition->item_definition);

	new_furniture.set_movable(new TileMovable(TileTransform{ .size = init.furniture_definition->size }));

	default_entity_render_overrides(&new_furniture, new_furniture.render_overrides);

	return new_furniture;
}