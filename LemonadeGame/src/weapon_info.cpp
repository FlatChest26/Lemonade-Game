#include "weapon_info.h"

std::shared_ptr<RawObjectChecker> Attack::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("attack");

	checker->add_property("name", RawValueType::RAW_String, true);
	checker->add_property("range", RawValueType::RAW_String, true);

	checker->add_property("damage_dice", RawValueType::RAW_String, true);

	checker->add_property("damage_type", RawValueType::RAW_String, true);

	checker->add_property("prepare", RawValueType::RAW_Custom, true);
	checker->add_property("recover", RawValueType::RAW_Custom, true);

	checker->add_property("frequency", RawValueType::RAW_String, false);

	checker->add_property("attack_verb", RawValueType::RAW_Object, true, Verb::get_checker());

	return checker;
}

Attack Attack::from_json(const json& j)
{
	auto checker = Attack::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	Attack a;

	a.name = j.at("name").get<std::string>();

	auto attack_range = AllAttackRanges.find(j.at("range").get<std::string>());
	if (attack_range != AllAttackRanges.end())
	{
		a.range = attack_range->second;
	}
	else
	{
		CERR("Invalid attack range name: " << j.at("range"));
	}

	auto damage_type = AllDamageTypes.find(j.at("damage_type").get<std::string>());
	if (damage_type != AllDamageTypes.end())
	{
		a.damage_type = damage_type->second;
	}
	else
	{
		CERR("Invalid damage type name: " << j.at("damage_type"));
	}

	a.damage_dice = DiceRoll::from_string(j.at("damage_dice").get<std::string>());

	a.prepare = TimeUnit::from_json(j.at("prepare"));
	a.recover = TimeUnit::from_json(j.at("recover"));

	a.attack_verb = Verb::from_json(j.at("attack_verb"));

	if (checker->is_valid_property("frequency", j))
	{
		auto frequency = AllAttackFrequencies.find(j.at("frequency").get<std::string>());
		if (frequency != AllAttackFrequencies.end())
		{
			a.frequency = frequency->second;
		}
		else
		{
			CERR("Invalid attack frequency name: " << j.at("frequency"));
		}
	}
	else
	{
		a.frequency = AttackFrequency::SECONDARY;
	}

	return a;
}

std::shared_ptr<RawObjectChecker> WeaponInfo::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("weapon_info");

	checker->add_property("attacks", RawValueType::RAW_Array, true);

	return checker;
}

WeaponInfo WeaponInfo::from_json(const json& j)
{
	auto checker = WeaponInfo::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	WeaponInfo w;

	for (auto attack_json : j.at("attacks"))
	{
		if (!Attack::get_checker()->check_json(attack_json))
		{
			CERR("Invalid attack json! " << attack_json);
			continue;
		}

		w.attacks.push_back(Attack::from_json(attack_json));
	}

	return w;
}