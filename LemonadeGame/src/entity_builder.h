#pragma once

#ifndef LEMONADE_GAME_SRC_ENTITY_BUILDER_H
#define LEMONADE_GAME_SRC_ENTITY_BUILDER_H

#include "entity.h"

#include "raw_master.h"

namespace EntityBuilder
{
	using StatInitializer = std::tuple<StatID, StatInt_t>;

	struct CreatureInitializer
	{
		CreatureSoul* soul{ nullptr };
		RawMaster::Ref<Species> species{ nullptr };
		Age lifespan{ 80 };
		Age current_age{ 18 };

		std::vector<RawMaster::Ref<BodyMod>> body_mods;
		Currency starting_cash{ Currency::none() };
		std::vector<StatInitializer> initial_stats;
	};

	Entity* create_creature(EntityID ID, CreatureInitializer init);

	struct ItemInitializer
	{
		RawMaster::Ref<ItemDefinition> item_definition{ nullptr };
	};

	Entity* create_item(EntityID ID, ItemInitializer init);

	struct FurnitureInitializer
	{
		RawMaster::Ref<FurnitureDefinition> furniture_definition{ nullptr };
	};

	Entity* create_furniture(EntityID ID, FurnitureInitializer init);
}

#endif // !LEMONADE_GAME_SRC_ENTITY_BUILDER_H
