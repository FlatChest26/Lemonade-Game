#include "species.h"
#include "raw_master.h"

Species Species::from_json(const json& j)
{
	Species s;

	s.ID = j.at("ID").get<std::string>();

	if (j.contains("noun"))
	{
		s.noun = Noun::from_json(j.at("noun"));
	}

	if (j.contains("renderable"))
	{
		s.renderable = Renderable::from_json(j.at("renderable"));
	}

	if (j.contains("adjective"))
	{
		if (!j.at("adjective").is_string())
		{
			CERR("Invalid 'adjective' field in species JSON object: " << j);
			s.adjective = "<invalid adjective>";
		}
		else
		{
			s.adjective = j.at("adjective").get<std::string>();
		}
	}

	if (j.contains("gendered_nouns"))
	{
		if (!j.at("gendered_nouns").is_structured())
		{
			CERR("Invalid 'gendered_nouns' field in species JSON object: " << j);
		}
		else
		{
			auto gendered_nouns_json = j.at("gendered_nouns");

			if (gendered_nouns_json.contains("immature"))
			{
				for (auto& [json_key, json_noun] : gendered_nouns_json.at("immature").get<std::map<std::string, json>>())
				{
					s.gendered_nouns.immature[AllGenderTypes[json_key]] = Noun::from_json(json_noun);
				}
			}

			if (gendered_nouns_json.contains("mature"))
			{
				for (auto& [json_key, json_noun] : gendered_nouns_json.at("mature").get<std::map<std::string, json>>())
				{
					s.gendered_nouns.mature[AllGenderTypes[json_key]] = Noun::from_json(json_noun);
				}
			}
		}
	}

	if (j.contains("default_body_plan"))
	{
		if (!j.at("default_body_plan").is_string())
		{
			CERR("Invalid 'default_body_plan' field in species JSON object: " << j);
			//s.default_body_plan = RawMaster::get<BodyPlan>("GenericBody");
		}
		else
		{
			s.default_body_plan = RawMaster::get<BodyPlan>(j.at("default_body_plan").get<std::string>());
		}
	}

	if (j.contains("body_mods"))
	{
		if (!j.at("body_mods").is_array())
		{
			CERR("Invalid 'body_mods' field in species JSON object: " << j);
			//s.body_mods = {};
		}
		else
		{
			for (auto& json_str : j.at("body_mods"))
			{
				if (!json_str.is_string())
				{
					CERR("Unrecognized body mod in: " << j);
					continue;
				}

				s.body_mods.push_back(RawMaster::get<BodyMod>(json_str.get<std::string>()));
			}
		}
	}

	if (j.contains("description"))
	{
		if (!j.at("description").is_string())
		{
			CERR("Invalid 'description' field in species JSON object: " << j);
			s.description = "<invalid description>";
		}
		else
		{
			s.description = j.at("description").get<std::string>();
		}
	}

	if (j.contains("lifespan_min"))
	{
		s.lifespan_min = Age::from_json(j.at("lifespan_min"));
	}

	if (j.contains("lifespan_max"))
	{
		s.lifespan_max = Age::from_json(j.at("lifespan_max"));
	}

	if (j.contains("lifespan"))
	{
		if (!j.at("lifespan").is_structured())
		{
			CERR("Invalid 'lifespan' field in species JSON object: " << j);
		}

		else
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
	}

	if (j.contains("child_age"))
	{
		s.child_age = Age::from_json(j.at("child_age"));
	}

	if (j.contains("adult_age"))
	{
		s.adult_age = Age::from_json(j.at("adult_age"));
	}

	if (j.contains("mature_age"))
	{
		s.mature_age = Age::from_json(j.at("mature_age"));
	}

	if (j.contains("frequency"))
	{
		if (!j.at("frequency").is_number())
		{
			CERR("Invalid 'frequency' field in species JSON object: " << j);
		}
		else
		{
			s.frequency = j.at("frequency").get<double>();
		}
	}

	if (j.contains("flags"))
	{
		if (!j.at("flags").is_array())
		{
			CERR("Invalid 'flags' field in species JSON object: " << j);
		}
		else
		{
			for (const auto& flag_name : j.at("flags"))
			{
				if (!flag_name.is_string())
				{
					CERR("Invalid tile flag name in species JSON object: " << flag_name);
					continue;
				}

				if (!AllSpeciesFlags.contains(flag_name))
				{
					CERR(flag_name << " is not a valid tile flag name!");
					continue;
				}

				if (static_cast<int>(AllSpeciesFlags[flag_name]) > 0)
				{
					s.flags |= AllSpeciesFlags[flag_name];
				}
				else
				{
					s.flags &= AllSpeciesFlags[flag_name];
				}
			}
		}
	}

	return s;
}