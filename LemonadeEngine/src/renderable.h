#pragma once

#ifndef LEMONADE_GAME_SRC_RENDERABLE_H
#define LEMONADE_GAME_SRC_RENDERABLE_H

#include <iostream>
#include <tuple>

#include "color.h"
#include "raw_object_checker.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

inline constexpr glyph_t DISTANT_DOT = 250;
inline constexpr glyph_t PLAYER_CHAR = '@';

inline const color_t falling_bg = color::get("light_cyan");
inline const color_t explored_fg = { 50, 45, 40 };
inline const color_t unexplored_fg = { 0, 0, 0 };

typedef	std::tuple<glyph_t, color_t, color_t> TupleRenderable;

struct Renderable
{
	// -- Variables -- //

	glyph_t glyph{ '?' };
	color_t fg{ DEFAULT_FG_COLOR };
	color_t bg{ DEFAULT_BG_COLOR };

	// -- Operators -- //

	constexpr bool operator==(const Renderable& other) const
	{
		return glyph == other.glyph && fg == other.fg && bg == other.bg;
	}
	constexpr bool operator==(const glyph_t& other) const
	{
		return glyph == other;
	}
	constexpr bool operator==(const color_t& other) const
	{
		return fg == other;
	}

	constexpr Renderable& operator=(const Renderable& other)
	{
		glyph = other.glyph;
		fg = other.fg;
		bg = other.bg;

		return *this;
	}

	constexpr Renderable& operator=(const glyph_t& other)
	{
		glyph = other;
		return *this;
	}

	constexpr Renderable& operator=(const color_t& other)
	{
		fg = other;
		return *this;
	}

	constexpr Renderable& operator=(const TupleRenderable& other)
	{
		glyph = std::get<0>(other);
		fg = std::get<1>(other);
		bg = std::get<2>(other);

		return *this;
	}

	constexpr TupleRenderable as_tuple() const
	{
		return TupleRenderable(glyph, fg, bg);
	}
	constexpr operator TupleRenderable() const
	{
		return as_tuple();
	}

	static std::shared_ptr<RawObjectChecker> get_checker()
	{
		static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

		checker->set_type_name("renderable");

		checker->add_property("glyph",	RawValueType::RAW_Char,		true);
		checker->add_property("fg",		RawValueType::RAW_Custom,	false);
		checker->add_property("bg",		RawValueType::RAW_Custom,	false);

		return checker;
	}

	static Renderable from_json(const json& j)
	{
		auto checker = Renderable::get_checker();

		if (!checker->check_json(j, true))
		{
			return {'?', DEFAULT_FG_COLOR, DEFAULT_BG_COLOR };
		}

		Renderable r;

		if (j.contains("glyph"))
		{
			if (j.at("glyph").is_number())
			{
				r.glyph = j.at("glyph").get<glyph_t>();
			}
			else if (j.at("glyph").is_string())
			{
				std::string glyph_str = j.at("glyph").get<std::string>();
				if (!glyph_str.empty())
				{
					r.glyph = static_cast<glyph_t>(glyph_str[0]);
				}
			}
			else
			{
				CERR("Invalid glyph type in JSON: " << j.at("glyph"));
			}
		}

		if (j.contains("fg"))
		{
			r.fg = color::from_json(j.at("fg"));
		}

		if (j.contains("bg"))
		{
			r.bg = color::from_json(j.at("bg"));
		}

		return r;
	}
};

inline std::ostream& operator<<(std::ostream& os, const Renderable& renderable)
{
	return os << static_cast<char>(renderable.glyph);
}
inline std::wostream& operator<<(std::wostream& os, const Renderable& renderable)
{
	return os << static_cast<wchar_t>(renderable.glyph);
}

inline Renderable SHROUD(DEFAULT_GLPYH, DEFAULT_FG_COLOR, DEFAULT_BG_COLOR);

#endif //  !LEMONADE_GAME_SRC_RENDERABLE_H