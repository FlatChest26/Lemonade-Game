#include "posture.h"


std::shared_ptr<RawObjectChecker> PostureDef::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("posture");

	checker->add_property("ID", RawValueType::RAW_String, true);
	checker->add_property("verb", RawValueType::RAW_Custom, true, Verb::get_checker());
	checker->add_property("type", RawValueType::RAW_String, true);
	checker->add_property("height", RawValueType::RAW_Number, true);

	checker->add_property("is_default", RawValueType::RAW_Bool, false);
	checker->add_property("directional_verb", RawValueType::RAW_Bool, false);

	checker->add_property("on_bp", RawValueType::RAW_String, false);
	checker->add_property("on_noun", RawValueType::RAW_Custom, false, Noun::get_checker());

	return checker;
}

PostureDef PostureDef::from_json(const json& j)
{
	auto checker = PostureDef::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	PostureDef p;

	p.ID = j.at("ID").get<std::string>();
	p.verb = Verb::from_json(j.at("verb"));

	auto it = AllPostureTypes.find(j.at("type").get<std::string>());
	if (it != AllPostureTypes.end())
	{
		p.type = it->second;
	}
	else
	{
		CERR("Invalid posture type name: " << j.at("type"));
		p.type = PostureType::STAND;
	}

	p.height = j.at("height").get<double>();

	if (checker->is_valid_property("is_default", j))
	{
		p.is_default = j.at("is_default").get<bool>();
	}

	if (checker->is_valid_property("directional_verb", j))
	{
		p.directional_verb = j.at("directional_verb").get<bool>();
	}

	if (checker->is_valid_property("on_bp", j))
	{
		p.on_bp = j.at("on_bp").get<BodyPartID>();
	}

	if (checker->is_valid_property("on_noun", j))
	{
		p.on_noun = Noun::from_json(j.at("on_noun"));
	}

	return p;
}