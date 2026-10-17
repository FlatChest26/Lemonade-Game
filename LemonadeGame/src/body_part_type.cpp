#include "body_part_type.h"
#include "snowy_macros.h"
#include "snowy_utilities.h"

BodyPartType BodyPartType::from_json(const json& j)
{
	BodyPartType bp;

	bp.ID = j.at("ID").get<std::string>();

	if (j.contains("noun"))
	{
		bp.noun = Noun::from_json(j);
	}

	if (j.contains("default_relative_size"))
	{
		if (!j.at("default_relative_size").is_number())
		{
			CERR("Invalid 'default_relative_size' field in species JSON object: " << j);
			bp.default_rel_size = 0.25;
		}
		else
		{
			bp.default_rel_size = j.at("default_relative_size").get<float>();
		}
	}

	if (j.contains("category"))
	{
		if (!j.at("category").is_string())
		{
			CERR("Invalid 'category' field in species JSON object: " << j);
		}
		else
		{
			auto cat_name = j.at("category").get<std::string>();

			if (!AllBodyPartCategories.contains(cat_name))
			{
				CERR(cat_name << " is not a valid body part category!");
				bp.category = BodyPartType::Category::OTHER;
			}
			else
			{
				bp.category = AllBodyPartCategories[cat_name];
			}
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
					CERR("Invalid body part flag: " << flag_name);
					continue;
				}

				if (!AllBodyPartFlags.contains(flag_name))
				{
					CERR(flag_name << " is not a valid body part flag!");
					continue;
				}

				bp.flags |= AllBodyPartFlags[flag_name];
			}
		}
	}

	return bp;
}