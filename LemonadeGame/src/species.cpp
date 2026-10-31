#include "species.h"
#include "raw_master.h"

std::shared_ptr<RawObjectChecker> Species::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("species");

	checker->add_property("ID", RawValueType::RAW_String, true);
	checker->add_property("noun", RawValueType::RAW_Custom, true, Noun::get_checker());
	checker->add_property("renderable", RawValueType::RAW_Custom, true, Renderable::get_checker());

	checker->add_property("adjective", RawValueType::RAW_String, false);
	checker->add_property("gendered_nouns", RawValueType::RAW_Object, false);
	checker->add_property("default_body_plan", RawValueType::RAW_String, false);
	checker->add_property("body_mods", RawValueType::RAW_Array, false);
	checker->add_property("description", RawValueType::RAW_String, false);

	checker->add_property("lifespan_min", RawValueType::RAW_Custom, false);
	checker->add_property("lifespan_max", RawValueType::RAW_Custom, false);

	checker->add_property("lifespan", RawValueType::RAW_Object, false);

	checker->add_property("child_age", RawValueType::RAW_Custom, false);
	checker->add_property("adult_age", RawValueType::RAW_Custom, false);
	checker->add_property("mature_age", RawValueType::RAW_Custom, false);

	checker->add_property("life_stages", RawValueType::RAW_Object, false);

	checker->add_property("frequency", RawValueType::RAW_Number, false);
	checker->add_property("flags", RawValueType::RAW_Array, false);

	checker->add_property("copy_from", RawValueType::RAW_String, false);

	checker->add_property("remove_flags", RawValueType::RAW_Array, false);
	checker->add_property("add_flags", RawValueType::RAW_Array, false);

	checker->add_property("common_genders", RawValueType::RAW_Array, false);
	checker->add_property("add_common_genders", RawValueType::RAW_Array, false);
	checker->add_property("remove_common_genders", RawValueType::RAW_Array, false);

	return checker;
}

Species Species::from_json(const json& j)
{
	auto checker = Species::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	Species s;

	if (checker->is_valid_property("copy_from", j))
	{
		auto copy_s = RawMaster::get<Species>(j.at("copy_from").get<std::string>());

		if (!copy_s)
		{
			CERR("Unrecognized 'copy_from' ID: " << j.at("copy_from"));
		}
		else
		{
			s = *copy_s;
		}
	}

	s.ID = j.at("ID").get<std::string>();
	s.noun = Noun::from_json(j.at("noun"));
	s.renderable = Renderable::from_json(j.at("renderable"));

	if (checker->is_valid_property("adjective", j))
	{
		s.adjective = j.at("adjective").get<std::string>();
	}

	if (checker->is_valid_property("gendered_nouns", j))
	{
		auto gendered_nouns_json = j.at("gendered_nouns");

		if (gendered_nouns_json.contains("immature"))
		{
			if (!gendered_nouns_json.at("immature").is_object())
			{
				CERR("Invalid 'immature' field in species JSON object: " << j);
			}
			else
			{
				for (auto& [json_key, json_noun] : gendered_nouns_json.at("immature").get<std::map<std::string, json>>())
				{
					s.gendered_nouns.immature[AllGenderTypes[json_key]] = Noun::from_json(json_noun);
				}
			}
		}

		if (gendered_nouns_json.contains("mature"))
		{
			if (!gendered_nouns_json.at("mature").is_object())
			{
				CERR("Invalid 'mature' field in species JSON object: " << j);
			}
			else
			{
				for (auto& [json_key, json_noun] : gendered_nouns_json.at("mature").get<std::map<std::string, json>>())
				{
					s.gendered_nouns.mature[AllGenderTypes[json_key]] = Noun::from_json(json_noun);
				}
			}
		}
	}

	if (checker->is_valid_property("default_body_plan", j))
	{
		s.default_body_plan = RawMaster::get<BodyPlan>(j.at("default_body_plan").get<std::string>());
	}

	if (checker->is_valid_property("body_mods", j))
	{
		for (auto& json_body_mod : j.at("body_mods"))
		{
			if (json_body_mod.is_string())
			{
				s.body_mods.push_back(RawMaster::get<BodyMod>(json_body_mod.get<std::string>()));
			}
			else if (json_body_mod.is_object())
			{
				s.body_mods.push_back(std::make_shared<BodyMod>(BodyMod::from_json(json_body_mod)));
			}
			else
			{
				CERR("Unrecognized body mod in: " << j);
			}
		}
	}

	if (checker->is_valid_property("description", j))
	{
		s.description = j.at("description").get<std::string>();
	}

	if (checker->is_valid_property("lifespan_min", j))
	{
		s.lifespan_min = Age::from_json(j.at("lifespan_min"));
	}

	if (checker->is_valid_property("lifespan_max", j))
	{
		s.lifespan_max = Age::from_json(j.at("lifespan_max"));
	}

	if (checker->is_valid_property("lifespan", j))
	{
		if (j.at("lifespan").contains("min"))
		{
			s.lifespan_min = Age::from_json(j.at("lifespan").at("min"));
		}

		if (j.at("lifespan").contains("max"))
		{
			s.lifespan_max = Age::from_json(j.at("lifespan").at("max"));
		}
	}

	if (checker->is_valid_property("child_age", j))
	{
		s.child_age = Age::from_json(j.at("child_age"));
	}

	if (checker->is_valid_property("adult_age", j))
	{
		s.adult_age = Age::from_json(j.at("adult_age"));
	}

	if (checker->is_valid_property("mature_age", j))
	{
		s.mature_age = Age::from_json(j.at("mature_age"));
	}

	if (checker->is_valid_property("life_stages", j))
	{
		auto life_stages_json = j.at("life_stages");

		if (life_stages_json.contains("child"))
		{
			s.child_age = Age::from_json(life_stages_json.at("child"));
		}

		if (life_stages_json.contains("adult"))
		{
			s.adult_age = Age::from_json(life_stages_json.at("adult"));
		}

		if (life_stages_json.contains("mature"))
		{
			s.mature_age = Age::from_json(life_stages_json.at("mature"));
		}
	}


	if (checker->is_valid_property("frequency", j))
	{
		s.frequency = j.at("frequency").get<double>();
	}

	auto flag_func = [&](json flag_name, bool add)
		{
			if (!flag_name.is_string())
			{
				CERR("Invalid tile flag name in species JSON object: " << flag_name);
				return false;
			}

			if (!AllSpeciesFlags.contains(flag_name))
			{
				CERR(flag_name << " is not a valid tile flag name!");
				return false;
			}

			if (add)
			{
				if (static_cast<int>(AllSpeciesFlags[flag_name]) > 0)
				{
					s.flags |= AllSpeciesFlags[flag_name];
				}
				else
				{
					s.flags &= AllSpeciesFlags[flag_name];
				}
			}
			else
			{
				if (static_cast<int>(AllSpeciesFlags[flag_name]) > 0)
				{
					s.flags &= ~AllSpeciesFlags[flag_name];
				}
				else
				{
					s.flags |= ~AllSpeciesFlags[flag_name];
				}
			}

			return true;
		};

	if (checker->is_valid_property("flags", j))
	{
		s.flags = {};

		for (const auto& flag_name : j.at("flags"))
		{
			flag_func(flag_name, true);
		}
	}

	if (checker->is_valid_property("add_flags", j))
	{
		for (const auto& flag_name : j.at("add_flags"))
		{
			flag_func(flag_name, true);
		}
	}

	if (checker->is_valid_property("remove_flags", j))
	{
		for (const auto& flag_name : j.at("remove_flags"))
		{
			flag_func(flag_name, false);
		}
	}

	auto gender_func = [&](json gender_json, bool add)
		{
			if (add)
			{
				if (gender_json.is_string())
				{
					auto gender_type = gender_json.get<std::string>();
					if (AllGenderTypes.find(gender_type) == AllGenderTypes.end())
					{
						CERR(gender_type << " is not a valid gender type!");
						return false;
					}

					s.common_genders[AllGenderTypes.at(gender_type)] = 1.0;
				}

				if (gender_json.is_object())
				{
					auto gender_type = gender_json.at("type").get<std::string>();
					auto frequency = gender_json.at("frequency").get<double>();

					if (AllGenderTypes.find(gender_type) == AllGenderTypes.end())
					{
						CERR(gender_type << " is not a valid gender type!");
						return false;
					}

					s.common_genders[AllGenderTypes.at(gender_type)] = frequency;
				}
			}
			else
			{
				if (gender_json.is_string())
				{
					auto gender_type = gender_json.get<std::string>();

					if (AllGenderTypes.find(gender_type) == AllGenderTypes.end())
					{
						CERR(gender_type << " is not a valid gender type!");
						return false;
					}

					s.common_genders.erase(AllGenderTypes.at(gender_type));
				}
			}

			return true;
		};

	if (checker->is_valid_property("common_genders", j))
	{
		s.common_genders.clear();

		for (const auto& gender_json : j.at("common_genders"))
		{
			gender_func(gender_json, true);
		}
	}

	if (checker->is_valid_property("add_common_genders", j))
	{
		for (const auto& gender_json : j.at("add_common_genders"))
		{
			gender_func(gender_json, true);
		}
	}

	if (checker->is_valid_property("remove_common_genders", j))
	{
		for (const auto& gender_json : j.at("remove_common_genders"))
		{
			gender_func(gender_json, false);
		}
	}

	if (s.adjective.empty())
	{
		s.adjective = s.noun.singular;
	}

	return s;
}