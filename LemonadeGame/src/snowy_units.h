#pragma once

#ifndef LEMONADE_GAME_SRC_SNOWY_UNITS_H
#define LEMONADE_GAME_SRC_SNOWY_UNITS_H

#include <units.h>

namespace units
{
	using Temperature = temperature::kelvin_t;
	using Length = length::centimeter_t;
	using Mass = mass::gram_t;

	template<typename T, typename U>
	inline constexpr double in(U units)
	{
		return units.convert<T>().value();
	}

	template<typename T, typename U>
	inline constexpr T as(U units)
	{
		return units.convert<T>();
	}
}

#endif // !LEMONADE_GAME_SRC_SNOWY_UNITS_H