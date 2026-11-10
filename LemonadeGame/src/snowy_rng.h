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

	inline bool flip_coin()
	{
		std::bernoulli_distribution distrib(0.5);
		return distrib(generator);
	}

	template<typename T>
	using WeightedVector = std::vector<std::tuple<double, T>>;

	template<typename T, typename K>
	static T get_weighted_random(const WeightedVector<T>& vector, K& generator)
	{
		std::vector<double> weights;

		for (const auto& [key, val] : vector)
		{
			weights.push_back(key);
		}

		std::discrete_distribution<size_t> weighted_distribution(weights.begin(), weights.end());
		return std::get<1>(vector.at(weighted_distribution(generator)));
	}
}

#endif // !LEMONADE_GAME_SRC_SNOWY_RNG_H
