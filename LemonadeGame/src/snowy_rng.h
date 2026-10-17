#pragma once

#ifndef LEMONADE_GAME_SRC_SNOWY_RNG_H
#define LEMONADE_GAME_SRC_SNOWY_RNG_H

#include <random>

namespace snowy::rng
{
	inline std::mt19937 generator;

	inline void initialize_rng()
	{
		std::random_device rd;
		generator = std::mt19937(rd());
	}

	inline int random_int_range(int min, int max)
	{
		std::uniform_int_distribution<int> distrib(min, max);
		return distrib(generator);
	}
}

#endif // !LEMONADE_GAME_SRC_SNOWY_RNG_H
