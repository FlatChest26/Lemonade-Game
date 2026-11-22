#pragma once

#ifndef LEMONADE_GAME_SRC_BODY_PART_COMPONENTS_H
#define LEMONADE_GAME_SRC_BODY_PART_COMPONENTS_H

#include "stat_values.h"
#include "enum_traits.h"
#include "snowy_grammar.h"


struct InterestingNames
{
	std::vector<Noun> lewd_names;
	std::vector<Noun> suggestive_names;
	std::vector<Noun> beauty_names;
	std::vector<Noun> kinky_names;
};

struct Health
{
	StatInt_t current;
	StatInt_t max;
};

#endif // !LEMONADE_GAME_SRC_BODY_PART_COMPONENTS_H
