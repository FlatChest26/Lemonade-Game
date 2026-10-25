#include "body_part_type.h"
#include "snowy_macros.h"
#include "snowy_utilities.h"
#include "raw_master.h"

std::shared_ptr<RawObjectChecker> BodyPartType::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("body_part_type");

	checker->add_property("ID", RawValueType::RAW_String, true);
	checker->add_property("noun", RawValueType::RAW_Custom, true, Noun::get_checker());

	checker->add_property("default_relative_size", RawValueType::RAW_Number, false);
	checker->add_property("category", RawValueType::RAW_String, false);

	checker->add_property("flags", RawValueType::RAW_Array, false);
	checker->add_property("plural", RawValueType::RAW_Bool, false);

	checker->add_property("copy_from", RawValueType::RAW_String, false);
	checker->add_property("add_flags", RawValueType::RAW_Array, false);
	checker->add_property("remove_flags", RawValueType::RAW_Array, false);

	return checker;
}

BodyPartType BodyPartType::from_json(const json& j)
{
	auto checker = BodyPartType::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	BodyPartType bp;

	if (checker->is_valid_property("copy_from", j))
	{
		auto copy_bp = RawMaster::get<BodyPartType>(j.at("copy_from").get<std::string>());

		if (!copy_bp)
		{
			CERR("Unrecognized 'copy_from' ID: " << j.at("copy_from"));
		}
		else
		{
			bp = *copy_bp;
		}
	}

	bp.ID = j.at("ID").get<std::string>();
	bp.noun = Noun::from_json(j);

	if (checker->is_valid_property("default_relative_size", j))
	{
		bp.default_rel_size = j.at("default_relative_size").get<double>();
	}

	if (checker->is_valid_property("plural", j))
	{
		bp.plural = j.at("plural").get<bool>();
	}

	if (checker->is_valid_property("category", j))
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

	auto flag_func = [&](json flag_name, bool add)
		{
			if (!flag_name.is_string())
			{
				CERR("Invalid body part flag: " << flag_name);
				return false;
			}

			if (!AllBodyPartFlags.contains(flag_name))
			{
				CERR(flag_name << " is not a valid body part flag!");
				return false;
			}

			if (add)
			{
				bp.flags |= AllBodyPartFlags[flag_name];
			}
			else
			{
				bp.flags &= ~AllBodyPartFlags[flag_name];
			}

			return true;
		};

	if (checker->is_valid_property("flags", j))
	{
		bp.flags = {};

		for (const auto& flag_name : j.at("flags"))
		{
			flag_func(flag_name, true);
		}
	}

	if (checker->is_valid_property("add_flags", j))
	{
		for (const auto& flag_name : j.at("add_flags"))
		{
			flag_func(flag_name, true);
		}
	}

	if (checker->is_valid_property("remove_flags", j))
	{
		for (const auto& flag_name : j.at("remove_flags"))
		{
			flag_func(flag_name, false);
		}
	}

	return bp;
}