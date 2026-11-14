#include "gait.h"

RawObjectChecker* GaitDef::get_checker()
{
	static RawObjectChecker checker = RawObjectChecker();

	checker.set_type_name("gait");

	checker.add_property("type", RawValueType::RAW_String, true);

	checker.add_property("name", RawValueType::RAW_String, true);
	checker.add_property("speed", RawValueType::RAW_Number, true);

	checker.add_property("is_default", RawValueType::RAW_Bool, false);

	return &checker;
}

GaitDef GaitDef::from_json(const json& j)
{
	auto checker = GaitDef::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	GaitDef gait;

	gait.name = j.at("name").get<std::string>();
	gait.speed = j.at("speed").get<double>();

	auto it = AllPostureTypes.find(j.at("type").get<std::string>());
	if (it != AllPostureTypes.end())
	{
		gait.posture_type = it->second;
	}
	else
	{
		CERR("Invalid posture type name: " << j.at("type"));
		gait.posture_type = PostureType::STAND;
	}

	if (checker->is_valid_property("is_default", j))
	{
		gait.is_default = j.at("is_default").get<bool>();
	}

	return gait;
}