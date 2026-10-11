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

#define DEFAULT_FG_COLOR	color::get("white")
#define DEFAULT_BG_COLOR	color::get("black")

constexpr glyph_t DEFAULT_GLPYH = ' ';

namespace color
{
	inline constexpr size_t COLORS_AMOUNT = 23;

	inline snowy::StaticDatabase<color_t, COLORS_AMOUNT> colors_db
	{
		{ "black",			{ 0x00, 0x00, 0x00 } },
		{ "white",			{ 0xFF, 0xFF, 0xFF } },
		{ "gray",			{ 0x80, 0x80, 0x80 } },
		{ "dark_gray",		{ 0x34, 0x34, 0x34 } },
		{ "light_gray",		{ 0xB6, 0xB6, 0xB6 } },
		{ "red",			{ 0xFF, 0x00, 0x00 } },
		{ "dark_red",		{ 0x80, 0x00, 0x00 } },
		{ "light_red",		{ 0xFF, 0x80, 0x80 } },
		{ "green",			{ 0x00, 0xFF, 0x00 } },
		{ "dark_green",		{ 0x00, 0x80, 0x00 } },
		{ "light_green",	{ 0x80, 0xFF, 0x80 } },
		{ "blue",			{ 0x00, 0x00, 0xFF } },
		{ "dark_blue",		{ 0x00, 0x00, 0x80 } },
		{ "light_blue",		{ 0x80, 0x80, 0xFF } },
		{ "yellow",			{ 0xFF, 0xFF, 0x00 } },
		{ "brown",			{ 0x80, 0x80, 0x00 } },
		{ "light_yellow",	{ 0xFF, 0xFF, 0x80 } },
		{ "magenta",		{ 0xFF, 0x00, 0xFF } },
		{ "dark_magenta",	{ 0x80, 0x00, 0x80 } },
		{ "light_magenta",	{ 0xFF, 0x80, 0xFF } },
		{ "cyan",			{ 0x00, 0xFF, 0xFF } },
		{ "dark_cyan",		{ 0x00, 0x80, 0x80 } },
		{ "light_cyan",		{ 0x80, 0xFF, 0xFF } },
	};

	inline constexpr color_t get(const snowy::StringID& color_name)
	{
		if (!colors_db.contains(color_name))
		{
			CERR("Unrecognized color: " << color_name);
			return DEFAULT_FG_COLOR;
		}

		return colors_db[color_name];
	}

	inline constexpr std::string get_name_of_color(const color_t& color)
	{
		if (!colors_db.contains(color))
		{
			CERR("Invalid color " << color);
			return "<undefined color>";
		}

		return colors_db.find_key(color);
	}

	inline constexpr color_t from_hex_code(const std::string& hex_code)
	{
		if (!hex_code.starts_with("0x"))
		{
			CERR("Hex code must start with '0x': " << hex_code);
			return DEFAULT_FG_COLOR;
		}

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

	inline constexpr color_t from_rgb(uint8_t r, uint8_t g, uint8_t b)
	{
		return { r, g, b };
	}

	inline constexpr color_t from_json(const json& j)
	{
		if (j.is_string())
		{
			if (j.get<std::string>().starts_with("0x"))
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