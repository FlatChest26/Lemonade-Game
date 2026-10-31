#pragma once

#ifndef LEMONADE_ENGINE_SRC_RAW_OBJECT_CHECKER_H
#define LEMONADE_ENGINE_SRC_RAW_OBJECT_CHECKER_H

#include "snowy_macros.h"

#include <string>
#include <unordered_map>
#include <memory>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

enum class RawValueType
{
	RAW_Bool,
	RAW_Char,
	RAW_Int,
	RAW_Float,
	RAW_Number,
	RAW_String,
	RAW_Array,
	RAW_Object,
	RAW_Structured,
	RAW_Custom
};

class RawObjectChecker
{
protected:
	std::unordered_map<std::string, std::tuple<RawValueType, bool, std::shared_ptr<RawObjectChecker>>> m_values;
	std::string m_type_name;

public:
	RawObjectChecker() :
		m_type_name("raw value")
	{
	}

	void set_type_name(std::string type_name)
	{
		m_type_name = type_name;
	}

	void add_property(std::string property_name, RawValueType value_type, bool is_required = false, std::shared_ptr<RawObjectChecker> checker = nullptr)
	{
		if (m_values.find(property_name) != m_values.end())
			return;

		m_values[property_name] = std::tuple(value_type, is_required, checker);
	}

	bool check_value_type(const json& j, RawValueType value_type, std::shared_ptr<RawObjectChecker> checker = nullptr, bool print_errors = true)
	{
		if (checker)
		{
			return checker->check_json(j, print_errors);
		}

		switch (value_type)
		{
		case RawValueType::RAW_Bool:
			return j.is_boolean();
		case RawValueType::RAW_Char:
			return j.is_number() || j.is_string();
		case RawValueType::RAW_Int:
			return j.is_number_integer();
		case RawValueType::RAW_Float:
			return j.is_number_float();
		case RawValueType::RAW_Number:
			return j.is_number();
		case RawValueType::RAW_String:
			return j.is_string();
		case RawValueType::RAW_Array:
			return j.is_array();
		case RawValueType::RAW_Object:
			return j.is_object();
		case RawValueType::RAW_Structured:
			return j.is_structured();
		case RawValueType::RAW_Custom:
			return true;
		}

		return false;
	}

	bool is_valid_property(std::string value_name, const json& j)
	{
		const auto& value_tuple = m_values[value_name];

		RawValueType value_type = std::get<0>(value_tuple);
		bool mandatory = std::get<1>(value_tuple);
		std::shared_ptr<RawObjectChecker> checker = std::get<2>(value_tuple);

		if (!j.contains(value_name))
		{
			return false;
		}

		if (!check_value_type(j.at(value_name), value_type, checker, false))
		{
			return false;
		}

		return true;
	}

	bool check_json(const json& j, bool print_errors = true)
	{
		for (const auto& [value_name, value_tuple] : m_values)
		{
			RawValueType value_type = std::get<0>(value_tuple);
			bool mandatory = std::get<1>(value_tuple);
			std::shared_ptr<RawObjectChecker> checker = std::get<2>(value_tuple);

			bool contains = j.contains(value_name);
			bool value_checked = contains ? check_value_type(j.at(value_name), value_type, checker, print_errors) : false;

			if (contains && !value_checked && print_errors)
			{
				CERR("Invalid '" << value_name << "' field in " << m_type_name << " JSON object: " << j);
			}

			if (mandatory && (!contains || !value_checked))
			{
				if (print_errors)
				{
					CERR("Not all required values found in " << m_type_name << " definition!");
				}

				return false;
			}
		}

		return true;
	}
};

#endif // !LEMONADE_ENGINE_SRC_RAW_OBJECT_CHECKER_H