#include "consumable.h"

std::shared_ptr<RawObjectChecker> ConsumableEffect::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();
	checker->set_type_name("consumable_effect");

	checker->add_property("modified_stat", RawValueType::RAW_String, true);
	checker->add_property("amount", RawValueType::RAW_Number, true);
	checker->add_property("taste", RawValueType::RAW_String, false);

	return checker;
}

ConsumableEffect ConsumableEffect::from_json(const json& j)
{
	auto checker = ConsumableEffect::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	ConsumableEffect effect;
	
	effect.modified_stat = j.at("modified_stat").get<StatID>();
	effect.amount = j.at("amount").get<StatInt_t>();

	if (checker->is_valid_property("taste", j))
	{
		effect.taste = j.at("taste").get<std::string>();
		effect.taste = trim(effect.taste);
	}

	return effect;
}