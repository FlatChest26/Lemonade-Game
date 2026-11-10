#pragma once

#ifndef LEMONADE_GAME_SRC_ITEM_DEF_H
#define LEMONADE_GAME_SRC_ITEM_DEF_H

#include "renderable.h"
#include "snowy_grammar.h"
#include "currency.h"
#include "consumable.h"
#include "weapon_info.h"

#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

enum class ItemFlags : int32_t
{
	NONE = 0,

	IS_CONSUMABLE = 1 << 0,
	HAS_WEAPON_INFO = 1 << 1,
};


IS_FLAG_ENUM(ItemFlags)

inline std::unordered_map<std::string, ItemFlags> AllItemFlags
{
	{ "consumable",		ItemFlags::IS_CONSUMABLE },
};

enum class ItemCategory : uint32_t
{
	GENERAL,
	WEAPON,
	CLOTHING,
	FOOD,
	DRINK,
};

inline std::unordered_map<std::string, ItemCategory> AllItemCategories
{
	{ "general",	ItemCategory::GENERAL },
	{ "weapon",		ItemCategory::WEAPON },
	{ "clothing",	ItemCategory::CLOTHING },
	{ "food",		ItemCategory::FOOD },
	{ "drink",		ItemCategory::DRINK },
};

struct ItemDefinition
{
	std::string ID;
	Noun noun;
	Renderable renderable;

	std::string description; 
	ItemFlags flags;

	ItemCategory category{ ItemCategory::GENERAL };

	Currency price{ Currency::from_dollars(1.0) };
	size_t carry_slots{ 1 };

	std::vector<ConsumableEffect> consumable_effects;

	WeaponInfo weapon_info;

	static std::shared_ptr<RawObjectChecker> get_checker();
	static ItemDefinition from_json(const json& j);

public:
	// -- Flags -- //
	bool is_consumable() const { return flags & ItemFlags::IS_CONSUMABLE; }
	bool has_weapon_info() const { return flags & ItemFlags::HAS_WEAPON_INFO; }
};

#endif // !LEMONADE_GAME_SRC_ITEM_DEF_H
