#pragma once

#ifndef LEMONADE_GAME_SRC_SNOWY_UNITS_H
#define LEMONADE_GAME_SRC_SNOWY_UNITS_H

#include <units.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace units
{
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

	using Mass = mass::gram_t;

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

	using Length = length::centimeter_t;

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

	using Temperature = temperature::kelvin_t;
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

	UNIT_ADD(
		specific_heat_capacity, joules_per_kilogram_kelvin, joules_per_kilogram_kelvin, j_per_kg_k,
		compound_unit<energy::joules, inverse<mass::kilograms>, inverse<temperature::kelvin>>
	)

	using SpecificHeatCapacity = specific_heat_capacity::joules_per_kilogram_kelvin_t;

	namespace specific_heat_capacity
	{
		inline SpecificHeatCapacity from_json(const json& j)
		{
			SpecificHeatCapacity s = SpecificHeatCapacity(0);

			if (j.is_number())
			{
				s += SpecificHeatCapacity(j.get<double>());
			}
			else if (j.is_object())
			{
				if (j.contains("joules_per_kilogram_kelvin"))
				{
					s += specific_heat_capacity::joules_per_kilogram_kelvin_t(j.at("joules_per_kilogram_kelvin").get<double>());
				}
			}

			return s;
		}
	}

	using Density = density::kilograms_per_cubic_meter_t;

	namespace density
	{
		inline Density from_json(const json& j)
		{
			Density d = Density(0);

			if (j.is_number())
			{
				d += Density(j.get<double>());
			}
			else if (j.is_object())
			{
				if (j.contains("kilograms_per_cubic_meter"))
				{
					d += density::kilograms_per_cubic_meter_t(j.at("kilograms_per_cubic_meter").get<double>());
				}

				if (j.contains("grams_per_milliliter"))
				{
					d += density::grams_per_milliliter_t(j.at("grams_per_milliliter").get<double>());
				}
			}

			return d;
		}
	}

	UNIT_ADD(
		molar_mass, grams_per_mole, grams_per_mole, g_per_mol,
		compound_unit<mass::grams, inverse<substance::moles>>
	)

	UNIT_ADD(
		molar_mass, kilograms_per_mole, kilograms_per_mole, kg_per_mol,
		compound_unit<mass::kilograms, inverse<substance::moles>>
	)

	using MolarMass = molar_mass::grams_per_mole_t;

	namespace molar_mass
	{
		inline MolarMass from_json(const json& j)
		{
			MolarMass m = MolarMass(0);

			if (j.is_number())
			{
				m += MolarMass(j.get<double>());
			}
			else if (j.is_object())
			{
				if (j.contains("grams_per_mole"))
				{
					m += molar_mass::grams_per_mole_t(j.at("grams_per_mole").get<double>());
				}

				if (j.contains("kilograms_per_mole"))
				{
					m += molar_mass::kilograms_per_mole_t(j.at("kilograms_per_mole").get<double>());
				}
			}

			return m;
		}
	}

	UNIT_ADD(
		thermal_mass, joules_per_kelvin, joules_per_kelvin, j_per_K,
		compound_unit<energy::joules, inverse<temperature::kelvin>>
	)

	using ThermalMass = thermal_mass::joules_per_kelvin_t;

	using Volume = volume::cubic_millimeter_t;
}

#endif // !LEMONADE_GAME_SRC_SNOWY_UNITS_H