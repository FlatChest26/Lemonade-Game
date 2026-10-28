#pragma once

#ifndef LEMONADE_GAME_SRC_ENTITY_MEMORY_H
#define LEMONADE_GAME_SRC_ENTITY_MEMORY_H

#include "entity_id.h"

#include "transforms.h"

struct EntityMemory
{
	EntityID entity;
	Position position;
};

#endif // !LEMONADE_GAME_SRC_ENTITY_MEMORY_H
