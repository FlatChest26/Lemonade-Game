#pragma once

#ifndef LEMONADE_GAME_SRC_DICE_ROLLS_H
#define LEMONADE_GAME_SRC_DICE_ROLLS_H

#include <random>
#include <cctype>
#include <string>

using DiceInt = uint16_t;

struct DiceRoll
{
	// -- Variables -- //

	DiceInt rolls = 1;
	DiceInt sides = 4;

	int modifer = 0;
	double multiplier = 1.0;

	static DiceRoll from_string(std::string_view str)
	{
		DiceRoll d;

		if (str.empty())
			return d;

		std::string rolls_str;
		std::string sides_str;

		std::string modifier_str;

		bool hit_d = false;
		bool hit_mod = false;

		bool subtract_mod = false;

		for (auto& chr : str)
		{
			if (!hit_d)
			{
				if (chr == 'd')
				{
					hit_d = true;
				}
				else
				{
					rolls_str.push_back(chr);
				}
			}
			else if (!hit_mod)
			{
				if (chr == '-' || chr == '+')
				{
					hit_mod = true;

					subtract_mod = chr == '-';
				}
				else
				{
					sides_str.push_back(chr);
				}
			}
			else
			{
				modifier_str.push_back(chr);
			}
		}

		if (rolls_str.empty() || sides_str.empty())
		{
			return d;
		}


		d.rolls = static_cast<DiceInt>(std::stoi(rolls_str));
		d.sides = static_cast<DiceInt>(std::stoi(sides_str));

		if (!modifier_str.empty())
		{
			d.modifer = subtract_mod ? -static_cast<DiceInt>(std::stoi(modifier_str)) : static_cast<DiceInt>(std::stoi(modifier_str));
		}

		return d;
	}

	// -- Operators -- //

	constexpr DiceRoll operator+(const int& other) const
	{
		return DiceRoll{ rolls, sides, modifer + other };
	}

	constexpr DiceRoll operator-(const int& other) const
	{
		return DiceRoll{ rolls, sides, modifer - other };
	}

	constexpr DiceRoll operator+() const
	{
		return DiceRoll{ rolls, sides, modifer, +multiplier };
	}

	constexpr DiceRoll operator-() const
	{
		return DiceRoll{ rolls, sides, modifer, -multiplier };
	}

	constexpr DiceRoll operator*(const double& other) const
	{
		return DiceRoll{ rolls, sides, modifer, other };
	}

	constexpr DiceRoll operator/(const double& other) const
	{
		return DiceRoll{ rolls, sides, modifer, 1 / other };
	}
};

template <typename T>
inline int roll(const DiceRoll& dice, T& generator)
{
	int total = 0;

	std::uniform_int_distribution<int> dist(1, dice.sides);

	for (int roll_count = 0; roll_count < static_cast<int>(dice.rolls); roll_count++)
	{
		total += dist(generator);
	}

	total = static_cast<int>(round(total * dice.multiplier));
	total += dice.modifer;

	return total;
}



inline constexpr DiceRoll operator "" _d4(unsigned long long int amount)
{
	return DiceRoll{ static_cast<DiceInt>(amount), 4 };
}

inline constexpr DiceRoll operator "" _d6(unsigned long long int amount)
{
	return DiceRoll{ static_cast<DiceInt>(amount), 6 };
}

inline constexpr DiceRoll operator "" _d8(unsigned long long int amount)
{
	return DiceRoll{ static_cast<DiceInt>(amount), 8 };
}

inline constexpr DiceRoll operator "" _d10(unsigned long long int amount)
{
	return DiceRoll{ static_cast<DiceInt>(amount), 10 };
}

inline constexpr DiceRoll operator "" _d12(unsigned long long int amount)
{
	return DiceRoll{ static_cast<DiceInt>(amount), 12 };
}

inline constexpr DiceRoll operator "" _d20(unsigned long long int amount)
{
	return DiceRoll{ static_cast<DiceInt>(amount), 20 };
}

#endif // !LEMONADE_GAME_SRC_DICE_ROLLS_H