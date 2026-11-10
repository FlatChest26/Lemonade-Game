#include "body_plan.h"

std::shared_ptr<RawObjectChecker> BodyPlan::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();
	checker->set_type_name("body_plan");

	checker->add_property("ID", RawValueType::RAW_String, true);
	checker->add_property("body_parts", RawValueType::RAW_Array, true);

	checker->add_property("default_height", RawValueType::RAW_Number, false);
	checker->add_property("default_weight", RawValueType::RAW_Number, false);

	checker->add_property("connections", RawValueType::RAW_Array, false);

	return checker;
}

BodyPlan BodyPlan::from_json(const json& j)
{
	auto checker = BodyPlan::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	BodyPlan bp;

	bp.ID = j.at("ID").get<std::string>();

	if (checker->is_valid_property("default_height", j))
	{
		bp.height = units::length::from_json(j.at("default_height"));
	}

	if (checker->is_valid_property("default_weight", j))
	{
		bp.weight = units::mass::from_json(j.at("default_weight"));
	}

	for (auto& json_body_plan : j.at("body_parts"))
	{
		if (!json_body_plan.is_object())
			continue;

		if (!BodyPartPlan::get_checker()->check_json(json_body_plan, true))
		{
			continue;
		}

		bp.body_parts.push_back(BodyPartPlan::from_json(json_body_plan));
	}

	if (checker->is_valid_property("connections", j))
	{
		for (auto& json_connection : j.at("connections"))
		{
			if (!json_connection.is_object())
				continue;

			if (!BodyConnectionPlan::get_checker()->check_json(json_connection, true))
			{
				continue;
			}

			bp.body_connections.push_back(BodyConnectionPlan::from_json(json_connection));
		}
	}

	if (checker->is_valid_property("body_relations", j))
	{
		for (auto& json_connection : j.at("body_relations"))
		{
			if (!BodyConnectionPlan::get_checker()->check_json(json_connection, true))
			{
				continue;
			}

			bp.body_relations.push_back(BodyConnectionPlan::from_json(json_connection));
		}
	}

	return bp;
}

std::shared_ptr<RawObjectChecker> BodyMod::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("body_mod");

	checker->add_property("ID", RawValueType::RAW_String, false);

	checker->add_property("modify_size", RawValueType::RAW_Number, false);

	checker->add_property("set_height", RawValueType::RAW_Number, false);
	checker->add_property("add_height", RawValueType::RAW_Number, false);
	checker->add_property("modify_height", RawValueType::RAW_Number, false);

	checker->add_property("set_weight", RawValueType::RAW_Number, false);
	checker->add_property("add_weight", RawValueType::RAW_Number, false);
	checker->add_property("modify_weight", RawValueType::RAW_Number, false);

	checker->add_property("add_parts", RawValueType::RAW_Array, false);
	checker->add_property("remove_parts", RawValueType::RAW_Array, false);
	checker->add_property("replace_parts", RawValueType::RAW_Object, false);

	checker->add_property("add_connections", RawValueType::RAW_Array, false);
	checker->add_property("add_relations", RawValueType::RAW_Array, false);

	checker->add_property("add_body_gloss", RawValueType::RAW_Array, false);
	checker->add_property("remove_body_gloss", RawValueType::RAW_Array, false);

	return checker;
}

BodyMod BodyMod::from_json(const json& j)
{
	auto checker = BodyMod::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	BodyMod b;

	if (checker->is_valid_property("ID", j))
	{
		b.ID = j.at("ID").get<std::string>();
	}

	if (checker->is_valid_property("modify_size", j))
	{
		b.modify_size = j.at("modify_size").get<double>();
	}

	if (checker->is_valid_property("set_height", j))
	{
		b.set_height = units::length::from_json(j.at("set_height"));
	}

	if (checker->is_valid_property("add_height", j))
	{
		b.add_height = units::length::from_json(j.at("add_height"));
	}

	if (checker->is_valid_property("modify_height", j))
	{
		b.multiply_height = j.at("modify_height").get<double>();
	}

	if (checker->is_valid_property("set_weight", j))
	{
		b.set_weight = units::mass::from_json(j.at("set_weight"));
	}

	if (checker->is_valid_property("add_weight", j))
	{
		b.add_weight = units::mass::from_json(j.at("add_weight"));
	}

	if (checker->is_valid_property("modify_weight", j))
	{
		b.multiply_weight = j.at("modify_weight").get<double>();
	}

	if (checker->is_valid_property("add_parts", j))
	{
		for (auto& json_body_plan : j.at("add_parts"))
		{
			if (!BodyPartPlan::get_checker()->check_json(json_body_plan, true))
			{
				CERR("Invalid body part plan json! " << json_body_plan);
				continue;
			}

			b.add_parts.push_back(BodyPartPlan::from_json(json_body_plan));
		}
	}

	if (checker->is_valid_property("remove_parts", j))
	{
		for (auto& json_body_plan_id : j.at("remove_parts"))
		{
			if (!json_body_plan_id.is_string())
			{
				CERR("Invalid body plan ID: " << json_body_plan_id << " in JSON: " << j);
				continue;
			}

			b.remove_parts.push_back(json_body_plan_id.get<std::string>());
		}
	}

	if (checker->is_valid_property("replace_parts", j))
	{
		for (auto& [replace_id, json_new_body_plan] : j.at("replace_parts").get<std::map<std::string, json>>())
		{
			if (!BodyPartPlan::get_checker()->check_json(json_new_body_plan, true))
			{
				CERR("Invalid body part plan json! " << json_new_body_plan);
				continue;
			}

			b.replace_parts[replace_id] = BodyPartPlan::from_json((json_new_body_plan));
		}
	}

	if (checker->is_valid_property("add_connections", j))
	{
		for (auto& json_connection : j.at("add_connections"))
		{
			if (!BodyConnectionPlan::get_checker()->check_json(json_connection, true))
			{
				CERR("Invalid body connection plan json! " << json_connection);
				continue;
			}

			b.add_connections.push_back(BodyConnectionPlan::from_json(json_connection));
		}
	}

	if (checker->is_valid_property("add_relations", j))
	{
		for (auto& json_connection : j.at("add_relations"))
		{
			if (!BodyConnectionPlan::get_checker()->check_json(json_connection, true))
			{
				CERR("Invalid body connection plan json! " << json_connection);
				continue;
			}

			b.add_relations.push_back(BodyConnectionPlan::from_json(json_connection));
		}
	}

	if (checker->is_valid_property("add_body_gloss", j))
	{
		for (auto& json_body_gloss : j.at("add_body_gloss"))
		{
			if (json_body_gloss.is_string())
			{
				b.add_body_gloss.push_back(RawMaster::get<BodyGloss>(json_body_gloss.get<std::string>()));
			}
			else if (json_body_gloss.is_object())
			{
				if (!BodyGloss::get_checker()->check_json(json_body_gloss, true))
				{
					CERR("Invalid body gloss json! " << json_body_gloss);
					continue;
				}

				b.add_body_gloss.push_back(std::make_shared<BodyGloss>(BodyGloss::from_json(json_body_gloss)));
			}
			else
			{
				CERR("Unrecognized body gloss: " << json_body_gloss);
			}
		}
	}

	if (checker->is_valid_property("remove_body_gloss", j))
	{
		for (auto& json_body_gloss : j.at("remove_body_gloss"))
		{
			if (!json_body_gloss.is_string())
			{
				CERR("Invalid body gloss ID: " << json_body_gloss);
				continue;
			}

			b.remove_body_gloss.push_back(json_body_gloss.get<std::string>());
		}
	}

	return b;
}

std::shared_ptr<RawObjectChecker> BodyPartPlan::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("body_part_plan");

	checker->add_property("bp_type", RawValueType::RAW_String, true);

	checker->add_property("ID", RawValueType::RAW_String, false);

	checker->add_property("rel_size", RawValueType::RAW_Number, false);
	checker->add_property("position", RawValueType::RAW_String, false);
	checker->add_property("prefix", RawValueType::RAW_String, false);

	checker->add_property("is_root", RawValueType::RAW_Bool, false);

	return checker;
}

BodyPartPlan BodyPartPlan::from_json(const json& j)
{
	auto checker = BodyPartPlan::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	BodyPartPlan bpp;

	bpp.type = j.at("bp_type").get<std::string>();

	if (checker->is_valid_property("ID", j))
	{
		bpp.ID = j.at("ID").get<std::string>();
	}
	else
	{
		bpp.ID = bpp.type;
	}

	if (checker->is_valid_property("rel_size", j))
	{
		bpp.rel_size = j.at("rel_size").get<double>();
	}
	else
	{
		bpp.rel_size = -1.0;
	}

	if (checker->is_valid_property("position", j))
	{
		auto pos_name = j.at("position").get<std::string>();

		if (!AllBodyPartPositions.contains(pos_name))
		{
			CERR(pos_name << " is not a valid body part position!");
		}
		else
		{
			bpp.position = AllBodyPartPositions[pos_name];
		}
	}

	if (checker->is_valid_property("prefix", j))
	{
		bpp.prefix = j.at("prefix").get<std::string>();
	}

	if (checker->is_valid_property("is_root", j))
	{
		bpp.is_root = j.at("is_root").get<bool>();
	}
	else
	{
		bpp.is_root = false;
	}

	return bpp;
}

std::shared_ptr<RawObjectChecker> BodyConnectionPlan::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("body_connection_plan");

	checker->add_property("parent", RawValueType::RAW_String, true);
	checker->add_property("child", RawValueType::RAW_String, true);
	checker->add_property("relation", RawValueType::RAW_String, false);

	return checker;
}

BodyConnectionPlan BodyConnectionPlan::from_json(const json& j)
{
	auto checker = BodyConnectionPlan::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	BodyConnectionPlan bcp;

	bcp.parent = j.at("parent").get<std::string>();
	bcp.child = j.at("child").get<std::string>();

	if (checker->is_valid_property("relation", j))
	{
		auto pos_name = j.at("relation").get<std::string>();

		if (!AllBodyPartRelativePosition.contains(pos_name))
		{
			CERR(pos_name << " is not a valid body part position!");
		}
		else
		{
			bcp.relative_position = AllBodyPartRelativePosition[pos_name];
		}
	}

	return bcp;
}

std::shared_ptr<RawObjectChecker> BodyGloss::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("body_gloss");

	checker->add_property("ID", RawValueType::RAW_String, true);
	checker->add_property("replacements", RawValueType::RAW_Array, true);

	return checker;
}

BodyGloss BodyGloss::from_json(const json& j)
{
	auto checker = BodyGloss::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	BodyGloss bdg;

	bdg.ID = j.at("ID").get<std::string>();

	for (auto& replacement_json : j.at("replacements"))
	{
		if (!NounReplacement::get_checker()->check_json(replacement_json))
		{
			CERR("Invalid noun replacement json! " << replacement_json);
			continue;
		}

		bdg.replacements.push_back(NounReplacement::from_json(replacement_json));
	}

	return bdg;
}