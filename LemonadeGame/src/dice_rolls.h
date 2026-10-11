#pragma once

#ifndef LEMONADE_GAME_SRC_DICE_ROLLS_H
#define LEMONADE_GAME_SRC_DICE_ROLLS_H

#include <random>
#include "snowy_rng.h"

struct dice_roll
{
	// -- Variables -- //

	unsigned short rolls = 1;
	unsigned short sides = 4;

	int modifer = 0;
	double multiplier = 1.0;

	// -- Operators -- //

	constexpr dice_roll operator+(const int& other) const
	{
		return dice_roll{ rolls, sides, modifer + other };
	}

	constexpr dice_roll operator-(const int& other) const
	{
		return dice_roll{ rolls, sides, modifer - other };
	}

	constexpr dice_roll operator+() const
	{
		return dice_roll{ rolls, sides, modifer, +multiplier };
	}

	constexpr dice_roll operator-() const
	{
		return dice_roll{ rolls, sides, modifer, -multiplier };
	}

	constexpr dice_roll operator*(const double& other) const
	{
		return dice_roll{ rolls, sides, modifer, other };
	}

	constexpr dice_roll operator/(const double& other) const
	{
		return dice_roll{ rolls, sides, modifer, 1 / other };
	}
};

inline int roll(const dice_roll& dice)
{
	int total = 0;

	std::uniform_int_distribution<int> distrib(1, dice.sides);

	for (int roll_count = 0; roll_count < static_cast<int>(dice.rolls); roll_count++)
	{
		total += distrib(snowy::rng::generator);
	}

	total = static_cast<int>(round(total * dice.multiplier));
	total += dice.modifer;

	return total;
}

#pragma warning( push )
#pragma warning( disable : 4455 ) // Diable the warning for user-defined literals not starting with an underscore

inline constexpr dice_roll operator "" d4(unsigned long long int amount)
{
	return dice_roll{ static_cast<unsigned short>(amount), 4 };
}

inline constexpr dice_roll operator "" d6(unsigned long long int amount)
{
	return dice_roll{ static_cast<unsigned short>(amount), 6 };
}

inline constexpr dice_roll operator "" d8(unsigned long long int amount)
{
	return dice_roll{ static_cast<unsigned short>(amount), 8 };
}

inline constexpr dice_roll operator "" d12(unsigned long long int amount)
{
	return dice_roll{ static_cast<unsigned short>(amount), 12 };
}

inline constexpr dice_roll operator "" d20(unsigned long long int amount)
{
	return dice_roll{ static_cast<unsigned short>(amount), 20 };
}

#pragma warning( pop )

#endif // !LEMONADE_GAME_SRC_DICE_ROLLS_H