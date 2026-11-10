#include "item_def.h"

std::shared_ptr<RawObjectChecker> ItemDefinition::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("item_definition");

	checker->add_property("ID", RawValueType::RAW_String, true);
	checker->add_property("noun", RawValueType::RAW_Custom, true, Noun::get_checker());
	checker->add_property("renderable", RawValueType::RAW_Custom, true, Renderable::get_checker());

	checker->add_property("description", RawValueType::RAW_String, false);
	checker->add_property("price", RawValueType::RAW_Custom, false);

	checker->add_property("flags", RawValueType::RAW_Array, false);

	checker->add_property("category", RawValueType::RAW_String, false);

	checker->add_property("consumable_effects", RawValueType::RAW_Array, false);

	checker->add_property("carry_slots", RawValueType::RAW_Int, false);

	checker->add_property("weapon_info", RawValueType::RAW_Custom, false, WeaponInfo::get_checker());

	return checker;
}

ItemDefinition ItemDefinition::from_json(const json& j)
{
	auto checker = ItemDefinition::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	ItemDefinition i;

	i.ID = j.at("ID").get<std::string>();
	i.noun = Noun::from_json(j.at("noun"));
	i.renderable = Renderable::from_json(j.at("renderable"));

	if (checker->is_valid_property("description", j))
	{
		i.description = j.at("description").get<std::string>();
		i.description = grammar::trim(i.description);
	}

	if (checker->is_valid_property("price", j))
	{
		i.price = Currency::from_json(j.at("price"));
	}

	if (checker->is_valid_property("flags", j))
	{
		i.flags = ItemFlags::NONE;

		for (const auto& flag : j.at("flags"))
		{
			auto it = AllItemFlags.find(flag.get<std::string>());
			if (it != AllItemFlags.end())
			{
				i.flags |= it->second;
			}
			else
			{
				CERR("Invalid item flag name: " << flag);
			}
		}
	}

	if (checker->is_valid_property("category", j))
	{
		auto it = AllItemCategories.find(j.at("category").get<std::string>());
		if (it != AllItemCategories.end())
		{
			i.category = it->second;
		}
		else
		{
			CERR("Invalid item category: " << j.at("category"));
		}
	}

	if (checker->is_valid_property("consumable_effects", j))
	{
		for (const auto& effect_json : j.at("consumable_effects"))
		{
			if (!ConsumableEffect::get_checker()->check_json(effect_json, true))
			{
				CERR("Invalid consumable effect JSON: " << effect_json);
				continue;
			}

			i.consumable_effects.push_back(ConsumableEffect::from_json(effect_json));
		}

		if (!i.consumable_effects.empty())
		{
			i.flags |= ItemFlags::IS_CONSUMABLE;
		}
	}

	if (checker->is_valid_property("carry_slots", j))
	{
		i.carry_slots = j.at("carry_slots").get<size_t>();
	}

	if (checker->is_valid_property("weapon_info", j))
	{
		i.weapon_info = WeaponInfo::from_json(j.at("weapon_info"));
		i.flags |= ItemFlags::HAS_WEAPON_INFO;
	}

	return i;
}