#include "material_def.h"
#include "entity.h"

std::shared_ptr<RawObjectChecker> MaterialStateDefinition::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("material_state_definition");

	checker->add_property("noun", RawValueType::RAW_Custom, true, Noun::get_checker());
	checker->add_property("color", RawValueType::RAW_Custom, true);

	checker->add_property("description", RawValueType::RAW_String, false);
	checker->add_property("flags", RawValueType::RAW_Array, false);
	checker->add_property("consumable_effects", RawValueType::RAW_Array, false);
	checker->add_property("value", RawValueType::RAW_Number, false);

	return checker;
}

MaterialStateDefinition MaterialStateDefinition::from_json(const json& j)
{
	auto checker = MaterialStateDefinition::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	MaterialStateDefinition ms;
	
	ms.noun = Noun::from_json(j.at("noun"));
	ms.color = color::from_json(j.at("color"));

	if (checker->is_valid_property("description", j))
	{
		ms.description = j.at("description").get<std::string>();
	}

	if (checker->is_valid_property("value", j))
	{
		ms.value = Currency::from_dollars(j.at("value").get<double>());;
	}

	if (checker->is_valid_property("flags", j))
	{
		ms.flags = MaterialFlag::NONE;

		for (const auto& flag : j.at("flags"))
		{
			auto it = AllMaterialFlags.find(flag.get<std::string>());
			
			if (it != AllMaterialFlags.end())
			{
				ms.flags |= it->second;
			}
			else
			{
				CERR("Invalid item flag name : " << flag);
			}
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

			ms.consumable_effects.push_back(ConsumableEffect::from_json(effect_json));
		}

		if (!ms.consumable_effects.empty())
		{
			ms.flags |= MaterialFlag::IS_CONSUMABLE;
		}
	}

	return ms;
}

std::shared_ptr<RawObjectChecker> MaterialType::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("material_type");

	checker->add_property("ID", RawValueType::RAW_String, true);

	checker->add_property("boiling_point", RawValueType::RAW_Object, true);
	checker->add_property("melting_point", RawValueType::RAW_Object, true);

	checker->add_property("states", RawValueType::RAW_Object, true);

	return checker;
}

MaterialType MaterialType::from_json(const json& j)
{
	auto checker = MaterialType::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	MaterialType mt;

	mt.ID = j.at("ID").get<std::string>();

	if (j.at("boiling_point").contains("celsius"))
	{
		mt.boiling_point = units::temperature::celsius_t(j.at("boiling_point").at("celsius").get<double>());
	}
	else if (j.at("boiling_point").contains("kelvin"))
	{
		mt.boiling_point = units::temperature::kelvin_t(j.at("boiling_point").at("kelvin").get<double>());
	}
	else if (j.at("boiling_point").contains("fahrenheit"))
	{
		mt.boiling_point = units::temperature::fahrenheit_t(j.at("boiling_point").at("fahrenheit").get<double>());
	}
	else
	{
		CERR("Invalid melting point " << j.at("boiling_point") << " in json: " << j);
	}

	if (j.at("melting_point").contains("celsius"))
	{
		mt.melting_point = units::temperature::celsius_t(j.at("melting_point").at("celsius").get<double>());
	}
	else if (j.at("melting_point").contains("kelvin"))
	{
		mt.melting_point = units::temperature::kelvin_t(j.at("melting_point").at("kelvin").get<double>());
	}
	else if (j.at("melting_point").contains("fahrenheit"))
	{
		mt.melting_point = units::temperature::fahrenheit_t(j.at("melting_point").at("fahrenheit").get<double>());
	}
	else
	{
		CERR("Invalid melting point " << j.at("melting_point") << " in json: " << j);
	}

	for (auto& [state_name, state_json] : j.at("states").get<std::unordered_map<std::string, json>>())
	{
		if (!AllMaterialStateTypes.contains(state_name))
		{
			CERR("Invalid state name " << state_name << " in json: " << j);
			continue;
		}

		if (!MaterialStateDefinition::get_checker()->check_json(state_json))
			continue;

		auto state_type = AllMaterialStateTypes[state_name];
		mt.states[state_type] = MaterialStateDefinition::from_json(state_json);
	}

	return mt;
}
