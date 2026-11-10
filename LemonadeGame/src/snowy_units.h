#pragma once

#ifndef LEMONADE_GAME_SRC_SNOWY_UNITS_H
#define LEMONADE_GAME_SRC_SNOWY_UNITS_H

#include <units.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

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

	namespace mass
	{
		inline Mass from_json(const json& j)
		{
			Mass m = Mass(0);

			if (j.is_number())
			{
				m += Mass(j.get<double>());
			}
			else if (j.is_object())
			{
				if (j.contains("milligrams"))
				{
					m += mass::milligram_t(j.at("milligrams").get<double>());
				}

				if (j.contains("grams"))
				{
					m += mass::gram_t(j.at("grams").get<double>());
				}

				if (j.contains("kilograms"))
				{
					m += mass::kilogram_t(j.at("kilograms").get<double>());
				}

				if (j.contains("metric_tons"))
				{
					m += mass::metric_ton_t(j.at("metric_tons").get<double>());
				}

				if (j.contains("ounces"))
				{
					m += mass::ounce_t(j.at("ounces").get<double>());
				}

				if (j.contains("pounds"))
				{
					m += mass::pound_t(j.at("pounds").get<double>());
				}
			}

			return m;
		}
	}

	namespace length
	{
		inline Length from_json(const json& j)
		{
			Length l = Length(0);

			if (j.is_number())
			{
				l += Length(j.get<double>());
			}
			else if (j.is_object())
			{
				if (j.contains("centimeters"))
				{
					l += length::centimeter_t(j.at("centimeters").get<double>());
				}

				if (j.contains("meters"))
				{
					l += length::meter_t(j.at("meters").get<double>());
				}

				if (j.contains("millimeters"))
				{
					l += length::millimeter_t(j.at("millimeters").get<double>());
				}

				if (j.contains("kilometers"))
				{
					l += length::kilometer_t(j.at("kilometers").get<double>());
				}

				if (j.contains("inches"))
				{
					l += length::inch_t(j.at("inches").get<double>());
				}

				if (j.contains("feet"))
				{
					l += length::foot_t(j.at("feet").get<double>());
				}

				if (j.contains("yard"))
				{
					l += length::yard_t(j.at("yard").get<double>());
				}
			}

			return l;
		}
	}


	namespace temperature
	{
		inline Temperature from_json(const json& j)
		{
			Temperature t = Temperature(0);

			if (j.is_number())
			{
				t += Temperature(j.get<double>());
			}
			else if (j.is_object())
			{
				if (j.contains("kelvin"))
				{
					t += temperature::kelvin_t(j.at("kelvin").get<double>());
				}

				if (j.contains("celsius"))
				{
					t += temperature::celsius_t(j.at("celsius").get<double>());
				}

				if (j.contains("fahrenheit"))
				{
					t += temperature::fahrenheit_t(j.at("fahrenheit").get<double>());
				}
			}

			return t;
		}
	}
}

#endif // !LEMONADE_GAME_SRC_SNOWY_UNITS_H