#pragma once

#ifndef SAI_SRC_COLORS_H
#define SAI_SRC_COLORS_H

#include <string>
#include <libtcod/color.h>

#include "snowy_macros.h"

#include "snowy_database.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

typedef TCOD_color_t	color_t;
typedef int				glyph_t;

#define DEFAULT_FG_COLOR	color_t{ 0xFF, 0xFF, 0xFF }
#define DEFAULT_BG_COLOR	color_t{ 0x00, 0x00, 0x00 }

constexpr glyph_t DEFAULT_GLPYH = ' ';

namespace color
{
	inline bool _colors_initialized = false;

	using ColorsDB = snowy::SingletonDatabase<color_t>;
	static inline void initialize_db()
	{
		ColorsDB::init();
		ColorsDB::add({
			{ "black",{ 0x00, 0x00, 0x00 } },
			{ "white",{ 0xFF, 0xFF, 0xFF } },
			});

		_colors_initialized = true;
	}

	inline constexpr color_t get(const snowy::StringID& color_name)
	{
		if (!_colors_initialized)
		{
			initialize_db();
		}

		if (ColorsDB::has(color_name))
		{
			return ColorsDB::get(color_name);
		}

		CERR("Unrecognized color: " << color_name);
		return DEFAULT_FG_COLOR;
	}

	inline constexpr color_t from_hex_code(const std::string& hex_code)
	{
		if (hex_code.starts_with("0x"))
		{
			if (hex_code.size() != 8)
			{
				CERR("Hex code must be 8 characters long (including '0x'): " << hex_code);
				return DEFAULT_FG_COLOR;
			}

			uint8_t r = std::stoi(hex_code.substr(2, 2), nullptr, 16);
			uint8_t g = std::stoi(hex_code.substr(4, 2), nullptr, 16);
			uint8_t b = std::stoi(hex_code.substr(6, 2), nullptr, 16);

			return { r, g, b };
		}

		if (hex_code.starts_with("#"))
		{
			if (hex_code.size() != 7)
			{
				CERR("Hex code must be 7 characters long (including '#'): " << hex_code);
				return DEFAULT_FG_COLOR;
			}

			uint8_t r = std::stoi(hex_code.substr(1, 2), nullptr, 16);
			uint8_t g = std::stoi(hex_code.substr(3, 2), nullptr, 16);
			uint8_t b = std::stoi(hex_code.substr(5, 2), nullptr, 16);

			return { r, g, b };
		}

		CERR("Hex code must start with '0x' or '#': " << hex_code);
		return DEFAULT_FG_COLOR;
	}

	inline constexpr color_t from_rgb(uint8_t r, uint8_t g, uint8_t b)
	{
		return { r, g, b };
	}

	inline constexpr color_t from_json(const json& j)
	{
		if (j.is_string())
		{
			if (j.get<std::string>().starts_with("0x") || j.get<std::string>().starts_with("#"))
			{
				return from_hex_code(j.get<std::string>());
			}
			else
			{
				return get(j.get<std::string>());
			}
		}
		else if (j.is_array() && j.size() == 3 && j.at(0).is_number_integer() && j.at(1).is_number_integer() && j.at(2).is_number_integer())
		{
			uint8_t r = j.at(0).get<int>();
			uint8_t g = j.at(1).get<int>();
			uint8_t b = j.at(2).get<int>();
			return { r, g, b };
		}

		CERR("Invalid color JSON: " << j);
		return DEFAULT_FG_COLOR;
	}
}

#endif // !SAI_SRC_COLORS_H