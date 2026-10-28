#include "entity_builder.h"
#include "anatomy.h"
#include "gait.h"

Entity* EntityBuilder::create_creature(EntityID ID, CreatureInitializer init)
{
	auto new_creature = new Entity(ID);

	// Soul
	new_creature->add_soul(init.soul);

	// Flags
	new_creature->add_flag(EntityFlag::IS_CREATURE); 
	new_creature->add_flag(EntityFlag::FREE_TO_MOVE);
	// new_creature->add_flag(EntityFlag::IS_BLOCKING);
	new_creature->add_flag(EntityFlag::CAN_COLLIDE);
	new_creature->add_flag(EntityFlag::CAN_FALL);
	new_creature->add_flag(EntityFlag::HAS_FOV);
	new_creature->add_flag(EntityFlag::CAN_HOLD_ITEMS);
	new_creature->add_flag(EntityFlag::CAN_BE_HELD);
	new_creature->add_flag(EntityFlag::ANIMATE);
	new_creature->add_flag(EntityFlag::IS_ALIVE);
	new_creature->add_flag(EntityFlag::CAN_DO_ACTIONS);

	// Species
	if (init.species)
	{
		new_creature->set_species(init.species);

		// Body Plan
		auto combined_body_mods = init.species->body_mods;

		combined_body_mods.reserve(init.body_mods.size() + init.species->body_mods.size());
		combined_body_mods.insert(combined_body_mods.end(), init.body_mods.begin(), init.body_mods.end());

		new_creature->set_anatomy(new Anatomy(init.species->default_body_plan, combined_body_mods));

		// Gaits
		new_creature->set_gait(new Gait(*init.species->default_gaits));
	}

	if (!init.species)
	{
		CERR("Creature: " << ID << " has no species assigned!");
	}

	// Lifespan
	new_creature->set_living(new Living(nullptr, init.current_age, init.lifespan));

	return new_creature;
}

Entity* EntityBuilder::create_item(EntityID ID, ItemInitializer init)
{
	auto new_item = new Entity(ID);

	new_item->add_flag(EntityFlag::IS_ITEM);
	new_item->add_flag(EntityFlag::FREE_TO_MOVE);
	new_item->add_flag(EntityFlag::CAN_COLLIDE);
	new_item->add_flag(EntityFlag::INANIMATE);
	new_item->add_flag(EntityFlag::CAN_FALL);
	//new_item->add_flag(EntityFlag::CAN_HOLD_ITEMS);
	new_item->add_flag(EntityFlag::CAN_BE_HELD);

	new_item->set_item_def(init.item_definition);

	return new_item;
}
