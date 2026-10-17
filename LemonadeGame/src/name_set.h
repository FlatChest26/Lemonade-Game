#pragma once

#ifndef LEMONADE_GAME_SRC_NAME_SET_H
#define LEMONADE_GAME_SRC_NAME_SET_H

#include "snowy_grammar.h"
#include "snowy_database.h"

struct NameSet
{
	std::string ID;
	json names_json;

	static NameSet from_json(const json& j);


	std::string get_name(size_t idx) const
	{
		if (0 < idx && idx < names_json.size())
		{
			return names_json.at(idx).get<std::string>();
		}

		return "";
	}
};


#endif // !LEMONADE_GAME_SRC_NAME_SET_H