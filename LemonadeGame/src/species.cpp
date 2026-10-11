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

	if (j.contains("default_body_plan"))
	{
		if (!j.at("default_body_plan").is_string())
		{
			CERR("Invalid 'default_body_plan' field in species JSON object: " << j);
			s.default_body_plan = body_plan::GenericBody;
		}
		else
		{
			s.default_body_plan = RawMaster::get_body_plan(j.at("default_body_plan").get<std::string>());
		}
	}

	if (j.contains("body_mods"))
	{
		if (!j.at("body_mods").is_array())
		{
			CERR("Invalid 'body_mods' field in species JSON object: " << j);
			s.body_mods = {};
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

				s.body_mods.push_back(RawMaster::get_body_mod(json_str.get<std::string>()));
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

	return s;
}