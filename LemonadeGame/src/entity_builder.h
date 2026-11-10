#pragma once

#ifndef LEMONADE_GAME_SRC_ENTITY_BUILDER_H
#define LEMONADE_GAME_SRC_ENTITY_BUILDER_H

#include "entity.h"

#include "raw_master.h"

namespace EntityBuilder
{
	extern void default_entity_render_overrides(Entity* entity, std::vector<RenderOverride>& render_overrides);

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

	extern Entity create_creature(EntityID ID, CreatureInitializer init);

	struct ItemInitializer
	{
		RawMaster::Ref<ItemDefinition> item_definition{ nullptr };
	};

	extern Entity create_item(EntityID ID, ItemInitializer init);

	struct FurnitureInitializer
	{
		RawMaster::Ref<FurnitureDefinition> furniture_definition{ nullptr };
	};

	extern Entity create_furniture(EntityID ID, FurnitureInitializer init);
}

#endif // !LEMONADE_GAME_SRC_ENTITY_BUILDER_H
