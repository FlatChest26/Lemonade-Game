#pragma once

#ifndef LEMONADE_GAME_SRC_THING_H
#define LEMONADE_GAME_SRC_THING_H

#include "lemonade_game.h"
#include "snowy_grammar.h"
#include "renderable.h"

using ThingID = std::string;

/* An abstraction of a "thing" that can be represented in the game. A grammar object with some visual presence. */
class Thing : public GrammarObject
{
public:
	// -- Getters -- //

	virtual std::string description() const { return std::string("This is ") + display_name(); }

	// Renderable //

	virtual Renderable renderable() const { return Renderable(); }

	virtual glyph_t glyph() const { return renderable().glyph; }
	virtual color_t fg() const { return renderable().fg; }
	virtual color_t bg() const { return renderable().bg; }

	// Misc //

	virtual GrammarNoun display_noun() const { return is_named() ? full_name() : singular_noun(); }
	virtual std::string display_name() const { return grammar::a(display_noun()); }

	virtual operator std::string() const { return display_name(); }
};

inline std::ostream& operator<<(std::ostream& os, const Thing& thing)
{
	return os << thing.display_name();
};

inline std::wostream& operator<<(std::wostream& os, const Thing& thing)
{
	return os << thing.display_name().c_str();
};

namespace std
{
	inline string to_string(const Thing& thing)
	{
		return thing.display_name();
	}

	inline string to_string(const Thing* thing)
	{
		return thing->display_name();
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