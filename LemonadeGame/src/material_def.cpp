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
	checker->add_property("value", RawValueType::RAW_Custom, false);

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
		ms.value = Currency::from_json(j.at("value"));
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
				CERR("Invalid material flag name : " << flag);
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

	checker->add_property("boiling_point", RawValueType::RAW_Custom, true);
	checker->add_property("melting_point", RawValueType::RAW_Custom, true);

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

	mt.boiling_point = units::temperature::from_json(j.at("boiling_point"));
	mt.melting_point = units::temperature::from_json(j.at("melting_point"));

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

Renderable Material::renderable() const
{
	Renderable renderable = { ' ', color::get("white"), color::get("black") };

	switch (current_state())
	{
	case MaterialStateType::SOLID:
		renderable.glyph = 219; // █
		break;
	case MaterialStateType::LIQUID:
		renderable.glyph = 247; // ≈
		break;
	case MaterialStateType::GAS:
		renderable.glyph = 177; // ▒
		break;
	case MaterialStateType::POWDER:
		renderable.glyph = 247; // ≈
		break;
	}

	if (auto state_def = current_state_definition())
	{
		renderable.fg = state_def->color;
	}

	return renderable;
}

GrammarNoun Material::display_noun() const
{
	std::string noun = singular_noun();

	switch (current_state())
	{
	case MaterialStateType::LIQUID:
	{
		if (amount() > 6)
		{
			noun = "wall of " + singular_noun();
		}
		else if (amount() > 3)
		{
			noun = "pool of " + singular_noun();

		}
		else if (amount() > 1)
		{
			noun = "puddle of " + singular_noun();
		}
		else if (amount() > 0)
		{
			if (should_stain())
			{
				noun = singular_noun() + " stain";
			}
			else
			{
				noun = "spattering of " + singular_noun();
			}
		}
		break;
	}
	case MaterialStateType::SOLID:
	{
		if (amount() > 6)
		{
			noun = "wall of " + singular_noun();
		}
		else if (amount() > 3)
		{
			noun = "block of " + singular_noun();
		}
		else if (amount() > 1)
		{
			noun = "chunk of " + singular_noun();
		}
		else if (amount() > 0)
		{
			noun = "sheet of " + singular_noun();
		}
		break;
	}
	case MaterialStateType::GAS:
	{
		if (amount() > 6)
		{
			noun = "wall of " + singular_noun();
		}
		else if (amount() > 2)
		{
			noun = "cloud of " + singular_noun();
		}
		else if (amount() > 0)
		{
			noun = "puff of " + singular_noun();
		}
		break;
	}
	case MaterialStateType::POWDER:
	{
		if (amount() > 6)
		{
			noun = "wall of " + singular_noun();
		}
		else if (amount() > 3)
		{
			noun = "pile of " + singular_noun();
		}
		else if (amount() > 0)
		{
			noun = "sheet of " + singular_noun();
		}
		break;
	}
	}

	return grammar::a(GrammarNoun(noun));
}

void Material::update_state()
{
	if (!m_mat_type || m_mat_type->states.empty())
		return;

	if (m_mat_type->states.contains(MaterialStateType::GAS) && m_temperature >= m_mat_type->boiling_point)
	{
		m_state = MaterialStateType::GAS;
	}
	else if (m_mat_type->states.contains(MaterialStateType::LIQUID) && m_temperature >= m_mat_type->melting_point)
	{
		m_state = MaterialStateType::LIQUID;
	}
	else if (m_mat_type->states.contains(MaterialStateType::SOLID) && m_state != MaterialStateType::POWDER)
	{
		m_state = MaterialStateType::SOLID;
	}
}