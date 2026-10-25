#pragma once

#ifndef LEMONADE_GAME_SRC_THING_H
#define LEMONADE_GAME_SRC_THING_H

#include "lemonade_game.h"
#include "snowy_grammar.h"
#include "renderable.h"

using ThingID = std::string;

struct CreatureSoul;
class TileType;
class BodyPart;

/* An abstraction of a "thing" that can be represented in the game. A grammar object with some visual presence. */
class Thing : public GrammarObject
{
public:

	Thing() = default;
	virtual ~Thing() {}

public:
	// -- Getters -- //

	virtual ThingID ID() const { return "THING"; }
	virtual std::string description() const { return std::string("This is ") + snowy::grammar::a(get_noun().singular); }

	// Renderable //

	virtual Renderable get_renderable() const { return Renderable(); }

	virtual glyph_t glyph() const { return get_renderable().glyph; }
	virtual color_t fg() const { return get_renderable().fg; }
	virtual color_t bg() const { return get_renderable().bg; }

	// -- Type Checks -- //

	virtual constexpr bool is_creature_soul() const { return false; }
	virtual constexpr CreatureSoul* as_creature_soul() { return nullptr; }

	virtual constexpr bool is_tile_type() const { return false; }
	virtual constexpr TileType* as_tile_type() { return nullptr; }

	virtual constexpr bool is_body_part() const { return false; }
	virtual constexpr BodyPart* as_body_part() { return nullptr; }

	// Misc //

	virtual std::string get_display_name() const { return full_name(); }
	virtual operator std::string() const { return get_display_name(); }
};

inline std::ostream& operator<<(std::ostream& os, const Thing& thing)
{
	return os << thing.get_display_name();
};

inline std::wostream& operator<<(std::wostream& os, const Thing& thing)
{
	return os << thing.get_display_name().c_str();
};

namespace std
{
	inline string to_string(const Thing& thing)
	{
		return thing.get_display_name();
	}

	inline string to_string(Thing* thing)
	{
		return thing->get_display_name();
	}
}

namespace fmt
{
	template <typename T>
	struct formatter<T, std::enable_if_t<std::is_base_of<Thing, T>::value, char>> : formatter<std::string>
	{
		auto format(const Thing& thing, format_context& ctx) const
		{
			return formatter<std::string>::format(thing.nickname(), ctx);
		}
	};
}

#endif // !LEMONADE_GAME_SRC_THING_H