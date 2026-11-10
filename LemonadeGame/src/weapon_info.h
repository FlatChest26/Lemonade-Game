#pragma once

#ifndef LEMONADE_GAME_SRC_WEAPON_INFO_H
#define LEMONADE_GAME_SRC_WEAPON_INFO_H

#include <vector>

#include "dice_rolls.h"
#include "time_units.h"
#include "snowy_grammar.h"
#include "stat_values.h"

#include "raw_object_checker.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

enum class AttackRange : uint8_t
{
	MELEE,
	RANGED
};

inline std::unordered_map<std::string, AttackRange> AllAttackRanges
{
	{ "melee",		AttackRange::MELEE },
	{ "ranged",		AttackRange::RANGED },
};

enum class DamageType : uint8_t
{
	EDGE,
	PIERCE
};

inline std::unordered_map<std::string, DamageType> AllDamageTypes
{
	{ "edge",		DamageType::EDGE },
	{ "pierce",		DamageType::PIERCE },
};

enum class AttackFrequency : uint8_t
{
	MAIN,
	SECONDARY
};

inline std::unordered_map<std::string, AttackFrequency> AllAttackFrequencies
{
	{ "main",			AttackFrequency::MAIN },
	{ "secondary",		AttackFrequency::SECONDARY },
};

struct Damage
{
	StatInt_t amount;
	DamageType type;
};

struct Attack
{
	std::string name;

	AttackRange range;
	
	DiceRoll damage_dice;
	DamageType damage_type;

	TimeUnit prepare;
	TimeUnit recover;

	AttackFrequency frequency;

	Verb attack_verb;

	static std::shared_ptr<RawObjectChecker> get_checker();
	static Attack from_json(const json& j);
};

struct WeaponInfo
{
	std::vector<Attack> attacks;

	static std::shared_ptr<RawObjectChecker> get_checker();
	static WeaponInfo from_json(const json& j);
};

#endif // !LEMONADE_GAME_SRC_WEAPON_INFO_H
