#include "gait.h"

std::shared_ptr<RawObjectChecker> GaitDef::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("gait");

	checker->add_property("name", RawValueType::RAW_String, true);
	checker->add_property("speed", RawValueType::RAW_Number, true);

	checker->add_property("is_default", RawValueType::RAW_Bool, false);

	return checker;
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

	if (checker->is_valid_property("is_default", j))
	{
		gait.is_default = j.at("is_default").get<bool>();
	}

	return gait;
}