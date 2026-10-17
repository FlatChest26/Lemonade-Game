#pragma once

#ifndef LEMONADE_GAME_SRC_ENTITY_BUILDER_H
#define LEMONADE_GAME_SRC_ENTITY_BUILDER_H

#include "entity.h"
#include "creature_soul.h"

#include "raw_master.h"

namespace EntityBuilder
{
	struct CreatureInitializer
	{
		CreatureSoul* soul{ nullptr };
		RawMaster::Ref<Species> species{ nullptr };
		Age lifespan{ 80 };
		Age current_age{ 18 };

		std::vector<RawMaster::Ref<BodyMod>> body_mods;
	};

	Entity* create_creature(CreatureInitializer init);


	struct ItemInitializer
	{
		
	};

	Entity* create_item(ItemInitializer init);
}


#endif // !LEMONADE_GAME_SRC_ENTITY_BUILDER_H
