#pragma once

#ifndef LEMONADE_GAME_SRC_BODY_PART_COMPONENTS_H
#define LEMONADE_GAME_SRC_BODY_PART_COMPONENTS_H

#include "stat_values.h"
#include "enum_traits.h"
#include "snowy_grammar.h"

using AllureStat = StatValue<0, 100>;

struct AllureType
{
	double lewdness;
	double suggestiveness;
	double kinkiness;
	double beauty;

	std::vector<Noun> lewd_names;
	std::vector<Noun> suggestive_names;
	std::vector<Noun> beauty_names;
	std::vector<Noun> kinky_names;
};

struct Allure
{
	AllureStat lewdness;
	AllureStat suggestiveness;
	AllureStat kinkiness;
	AllureStat beauty;

	Allure(AllureStat lewdness = 0, AllureStat suggestiveness = 0, AllureStat kinkiness = 0, AllureStat beauty = 0) :
		lewdness(lewdness), suggestiveness(suggestiveness), kinkiness(kinkiness), beauty(beauty)
	{
	}
};

#endif // !LEMONADE_GAME_SRC_BODY_PART_COMPONENTS_H
