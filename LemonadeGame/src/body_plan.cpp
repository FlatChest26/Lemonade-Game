#include "body_plan.h"

BodyMod BodyMod::from_json(const json& j)
{
	BodyMod b;

	b.ID = j.at("ID").get<std::string>();

	if (j.contains("add_parts"))
	{
		if (!j.at("add_parts").is_array())
		{
			CERR("Invalid 'add_parts' field in species JSON object: " << j);
		}
		else
		{
			for (auto& json_body_plan : j.at("add_parts"))
			{
				if (!json_body_plan.contains("ID"))
				{
					CERR("JSON body plan requires ID: " << json_body_plan);
					continue;
				}

				b.add_parts.push_back(BodyPartPlan::from_json(json_body_plan));
			}
		}
	}


	return b;
}

BodyPartPlan BodyPartPlan::from_json(const json& j)
{
	BodyPartPlan bpp;

	bpp.ID = j.at("ID").get<std::string>();

	if (j.contains("body_part_type"))
	{
		if (!j.at("body_part_type").is_string())
		{
			CERR("Invalid 'rel_size' field in species JSON object: " << j);
		}
		else
		{
			bpp.type = j.at("body_part_type").get<std::string>();
		}
	}

	if (j.contains("rel_size"))
	{
		if (!j.at("rel_size").is_number())
		{
			CERR("Invalid 'rel_size' field in species JSON object: " << j);
			bpp.rel_size = 1.0f;
		}
		else
		{
			bpp.rel_size = j.at("rel_size").get<double>();
		}
	}

	if (j.contains("body_position"))
	{
		if (!j.at("body_position").is_string())
		{
			CERR("Invalid 'body_position' field in species JSON object: " << j);
		}
		else
		{
			auto pos_name = j.at("body_position").get<std::string>();

			if (!AllBodyPartPositions.contains(pos_name))
			{
				CERR(pos_name << " is not a valid body part position!");
			}
			else
			{
				bpp.position = AllBodyPartPositions[pos_name];
			}
		}
	}

	return bpp;
}

BodyConnectionPlan BodyConnectionPlan::from_json(const json& j)
{
	BodyConnectionPlan bcp;

	if (j.contains("parent"))
	{
		if (!j.at("parent").is_string())
		{
			CERR("Invalid 'parent' field in species JSON object: " << j);
		}
		else
		{
			bcp.parent = j.at("parent").get<std::string>();
		}
	}

	if (j.contains("child"))
	{
		if (!j.at("child").is_string())
		{
			CERR("Invalid 'child' field in species JSON object: " << j);
		}
		else
		{
			bcp.child = j.at("child").get<std::string>();
		}
	}

	if (j.contains("relative_position"))
	{
		if (!j.at("relative_position").is_string())
		{
			CERR("Invalid 'relative_position' field in species JSON object: " << j);
		}
		else
		{
			auto pos_name = j.at("relative_position").get<std::string>();

			if (!AllBodyPartRelativePosition.contains(pos_name))
			{
				CERR(pos_name << " is not a valid body part position!");
			}
			else
			{
				bcp.relative_position = AllBodyPartRelativePosition[pos_name];
			}
		}
	}

	return bcp;
}

BodyPlan BodyPlan::from_json(const json& j)
{
	BodyPlan bp;

	bp.ID = j.at("ID").get<std::string>();

	if (j.contains("body_parts"))
	{
		if (!j.at("body_parts").is_array())
		{
			CERR("Invalid 'body_parts' field in species JSON object: " << j);
		}
		else
		{
			for (auto& json_body_plan : j.at("body_parts"))
			{
				if (!json_body_plan.contains("ID"))
				{
					CERR("JSON body plan requires ID: " << json_body_plan);
					continue;
				}

				bp.body_parts.push_back(BodyPartPlan::from_json(json_body_plan));
			}
		}
	}

	if (j.contains("body_connections"))
	{
		if (!j.at("body_connections").is_array())
		{
			CERR("Invalid 'body_connections' field in species JSON object: " << j);
		}
		else
		{
			for (auto& json_body_plan : j.at("body_connections"))
			{
				if (!json_body_plan.contains("ID"))
				{
					CERR("JSON body plan requires ID: " << json_body_plan);
					continue;
				}

				bp.body_connections.push_back(BodyConnectionPlan::from_json(json_body_plan));
			}
		}
	}

	return bp;
}
