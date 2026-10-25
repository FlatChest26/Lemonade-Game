#include "item_def.h"

std::shared_ptr<RawObjectChecker> ItemDefinition::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("item_definition");

	checker->add_property("ID", RawValueType::RAW_String, true);
	checker->add_property("noun", RawValueType::RAW_Custom, true, Noun::get_checker());
	checker->add_property("renderable", RawValueType::RAW_Custom, true, Renderable::get_checker());

	checker->add_property("description", RawValueType::RAW_String, false);

	return checker;
}

ItemDefinition ItemDefinition::from_json(const json& j)
{
	auto checker = ItemDefinition::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	ItemDefinition i;

	i.ID = j.at("ID").get<std::string>();
	i.noun = Noun::from_json(j.at("noun"));
	i.renderable = Renderable::from_json(j.at("renderable"));

	if (checker->is_valid_property("description", j))
	{
		i.description = j.at("description").get<std::string>();
	}

	return i;
}
