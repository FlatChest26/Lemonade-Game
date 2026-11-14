#pragma once

#ifndef LEMONADE_GAME_SRC_SNOWY_GRAMMAR_H
#define LEMONADE_GAME_SRC_SNOWY_GRAMMAR_H

#include <string>
#include <string_view>
#include <vector>

#define FMT_HEADER_ONLY
#include <fmt/core.h>

#include "snowy_macros.h"
#include "snowy_database.h"
#include "raw_master.h"

#include "raw_object_checker.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "enum_traits.h"

struct Noun
{
	std::string singular = "";
	std::string plural = "";
	bool always_plural = false;

	constexpr operator const char* () const { return singular.c_str(); }
	constexpr operator std::string() const { return singular; }

	constexpr bool empty() const { return singular[0] == '\0' && plural[0] == '\0'; }

	static RawObjectChecker* get_checker();
	static Noun from_json(const json& j);
};

inline constexpr Noun operator+(const std::string& str, const Noun& noun)
{
	return Noun{ str + noun.singular, str + noun.plural, noun.always_plural };
}

inline constexpr Noun operator+(const Noun& noun, const std::string& other)
{
	return Noun{ noun.singular + other, noun.plural + other, noun.always_plural };
}

struct Name
{
	std::string nickname = "";
	std::string full_name{ nickname };

	constexpr operator const char* () const { return nickname.c_str(); }
	constexpr operator std::string() const { return nickname; }

	constexpr bool empty() const { return nickname[0] == '\0' && full_name[0] == '\0'; }

	static Name from_json(const json& j);
};

struct GrammarNoun
{
	Noun noun;
	bool is_proper = false;

	static GrammarNoun from_string(const std::string& str, bool is_proper = true)
	{
		GrammarNoun n;

		n.noun = Noun{ str, str + "s", false };
		n.is_proper = is_proper;

		return n;
	}

	static GrammarNoun from_noun(const Noun& noun, bool is_proper = false)
	{
		GrammarNoun n;

		n.noun = noun;
		n.is_proper = is_proper;

		return n;
	}

	static GrammarNoun from_name(const Name& name, bool is_proper = true)
	{
		GrammarNoun n;

		n.noun = Noun{ name.nickname, name.nickname };
		n.is_proper = is_proper;

		return n;
	}

	std::string as_string() const { return noun.singular; }
	Noun as_noun() const { return noun; }

	GrammarNoun& operator=(const std::string& str)
	{
		noun = Noun{ str, str + "s", false };
		return *this;
	}

	GrammarNoun& operator=(const Noun& other_noun)
	{
		noun = other_noun;
		return *this;
	}

	GrammarNoun& operator=(const Name& name)
	{
		noun = Noun{ name.nickname, name.nickname };
		return *this;
	}

	GrammarNoun& operator+=(const std::string& str)
	{
		noun = Noun{ noun.singular + str, noun.plural + str, noun.always_plural };
		return *this;
	}
};

inline GrammarNoun operator+(const std::string& str, const GrammarNoun& g_noun)
{
	return GrammarNoun::from_noun(Noun{ str + g_noun.noun.singular, str + g_noun.noun.plural, g_noun.noun.always_plural }, g_noun.is_proper);
}

inline GrammarNoun operator+(const GrammarNoun& g_noun, const std::string& str)
{
	return GrammarNoun::from_noun(Noun{ g_noun.noun.singular + str, g_noun.noun.plural + str, g_noun.noun.always_plural }, g_noun.is_proper);
}

struct Verb
{
	std::string singular;
	std::string plural;

	static RawObjectChecker* get_checker();
	static Verb from_json(const json& j);
};

struct WordReplacement
{
	std::string old_str;
	std::string new_str;

	bool empty() const { return old_str.empty() && new_str.empty(); }

	static RawObjectChecker* get_checker();
	static WordReplacement from_json(const json& j);
};

struct NounReplacement
{
	WordReplacement singular;
	WordReplacement plural;

	static RawObjectChecker* get_checker();
	static NounReplacement from_json(const json& j);
};

namespace grammar
{
	inline std::string english_list(const std::ranges::input_range auto& list)
	{
		std::string final_list = "";

		size_t i = 0;

		for (const auto& item : list)
		{
			final_list += item;

			if (i < list.size() - 1)
			{
				if (list.size() > 2)
				{
					if (i == list.size() - 2)
					{
						final_list += ", and ";
					}
					else
					{
						final_list += ", ";
					}
				}
				else if (i == list.size() - 2)
				{
					final_list += " and ";
				}
			}

			i++;
		}

		return final_list;
	}
	extern constexpr std::string capitalize(std::string_view str);
	extern std::string trim(std::string_view s);

	extern bool has_starting_vowel_sound(const std::string& noun);

	extern std::string a(std::string_view noun);
	inline std::string A(std::string_view noun) { return capitalize(a(noun)); }

	inline std::string an(std::string_view noun) { return a(noun); }
	inline std::string An(std::string_view noun) { return A(noun); }

	extern std::string the(std::string_view noun);
	inline std::string The(std::string_view noun) { return capitalize(the(noun)); }

	extern std::string poss(std::string_view noun);
	inline std::string Poss(std::string_view noun) { return capitalize(poss(noun)); }

	extern std::string poss(const Noun& noun, bool plural = false);
	inline std::string Poss(const Noun& noun, bool plural = false) { return capitalize(poss(noun, plural)); }

	extern std::string a(const Noun& noun, bool plural = false);
	inline std::string A(const Noun& noun, bool plural = false) { return capitalize(a(noun, plural)); }

	inline std::string an(const Noun& noun, bool plural = false) { return a(noun, plural); }
	inline std::string An(const Noun& noun, bool plural = false) { return A(noun, plural); }

	extern std::string the(const Noun& noun, bool plural = false);
	inline std::string The(const Noun& noun, bool plural = false) { return capitalize(the(noun, plural)); }

	extern bool is_proper(const GrammarNoun& noun);
	extern bool is_plural(const GrammarNoun& noun);

	extern std::string to_string(const GrammarNoun& noun, bool plural = false);

	extern std::string a(const GrammarNoun& noun, bool plural = false);
	inline std::string A(const GrammarNoun& noun, bool plural = false) { return capitalize(a(noun, plural)); }

	inline std::string an(const GrammarNoun& noun, bool plural = false) { return a(noun, plural); }
	inline std::string An(const GrammarNoun& noun, bool plural = false) { return A(noun, plural); }

	extern std::string the(const GrammarNoun& noun, bool plural = false);
	inline std::string The(const GrammarNoun& noun, bool plural = false) { return capitalize(the(noun, plural)); }

	extern std::string poss(const GrammarNoun& noun, bool plural = false);
	inline std::string Poss(const GrammarNoun& noun, bool plural = false) { return capitalize(poss(noun, plural)); }

	extern std::string say_regarding(const Verb& verb, const Noun& noun, bool plural = false);
	extern std::string say_regarding(const Verb& verb, const GrammarNoun& g_noun, bool plural = false);
	extern std::string say_regarding(const Verb& verb, int num);

	extern std::string word_replace(std::string_view str, const WordReplacement& replacement);
	extern Noun noun_replace(const Noun& noun, const NounReplacement& replacement);
}

namespace std
{
	inline constexpr string to_string(Noun noun) { return noun.singular; }
	inline constexpr string to_string(Name name) { return name.full_name; }
}

using Age_t = uint16_t;

struct Age
{
	Age_t physical_age{ 18 };
	Age_t chronological_age{ physical_age };

	constexpr bool operator==(Age other) const { return physical_age == other.physical_age && chronological_age == other.chronological_age; }
	constexpr operator Age_t() const { return physical_age; }

	static Age from_json(const json& j);
};

struct Pronouns
{
	std::string subjective;
	std::string objective;
	std::string determiner;
	std::string independent_possessive;
	std::string reflexive;
	std::string shortened;

	bool acts_plural = false;

	constexpr operator const char* () const { return shortened.c_str(); }
	constexpr operator std::string() const { return shortened; }

	static RawObjectChecker* get_checker();
	static Pronouns from_json(const json& j);
};

inline Pronouns DEFAULT_NEUTER_PRONOUNS{ "it", "it", "its", "its", "itself", "it/its", false };
inline Pronouns DEFAULT_EPICENE_PRONOUNS{ "they", "them", "their", "theirs", "themself", "they/them", true };

enum class GenderType : uint8_t
{
	NONE = 0,

	NEUTER = 1 << 0,
	EPICENE = 1 << 1,
	MASCULINE = 1 << 2,
	FEMININE = 1 << 3,
};

IS_FLAG_ENUM(GenderType)

inline std::map<std::string, GenderType> AllGenderTypes
{
	{ "NEUTER",			GenderType::NEUTER },
	{ "EPICENE",		GenderType::EPICENE },
	{ "MASCULINE",		GenderType::MASCULINE },
	{ "FEMININE",		GenderType::FEMININE },
};

struct Gender
{
	std::string name;

	RawMaster::Ref<Pronouns> pronouns;
	GenderType gender_type{ GenderType::NONE };

	Noun mature_noun;
	Noun immature_noun;

	bool is_plural = false;
	bool uses_noun = true;

	bool is_neuter() const { return gender_type & GenderType::NEUTER; }
	bool is_epicene() const { return gender_type & GenderType::EPICENE; }
	bool is_masculine() const { return gender_type & GenderType::MASCULINE; }
	bool is_feminine() const { return gender_type & GenderType::FEMININE; }

	static RawObjectChecker* get_checker();
	static Gender from_json(const json& j);
};

struct GrammarObject
{
	virtual Noun noun() const { return Noun{ "grammar object", "grammar objects" }; }
	virtual Name name() const { return Name(); }

	virtual RawMaster::Ref<Pronouns> pronouns() const { return RawMaster::get<Pronouns>("OBJECT"); }

	virtual std::string singular_noun() const { return noun().singular; }
	virtual std::string plural_noun() const { return noun().plural; }

	bool is_named() const { return !name().empty(); }

	virtual std::string nickname() const
	{
		if (is_named())
		{
			return prefix() + name().nickname + suffix();
		}

		return prefix() + singular_noun() + suffix();
	}

	virtual std::string full_name() const
	{
		if (is_named())
		{
			return prefix() + name().full_name + suffix();
		}

		return prefix() + singular_noun() + suffix();
	}

	virtual std::string they() const { return pronouns()->subjective; }
	virtual std::string them() const { return pronouns()->objective; }
	virtual std::string their() const { return pronouns()->determiner; }
	virtual std::string theirs() const { return pronouns()->independent_possessive; }
	virtual std::string themselves() const { return pronouns()->reflexive; }

	virtual bool is_plural() const { return noun().always_plural; }
	virtual bool is_pronouns_plural() const { return pronouns()->acts_plural; }

	virtual std::string prefix() const { return ""; }
	virtual std::string suffix() const { return ""; }
};

namespace grammar
{
	inline constexpr std::string they(GrammarObject* T) { return T->pronouns()->subjective; }
	inline constexpr std::string They(GrammarObject* T) { return capitalize(they(T)); }

	inline constexpr std::string them(GrammarObject* T) { return T->pronouns()->objective; }
	inline constexpr std::string Them(GrammarObject* T) { return capitalize(them(T)); }

	inline constexpr std::string their(GrammarObject* T) { return T->pronouns()->determiner; }
	inline constexpr std::string Their(GrammarObject* T) { return capitalize(their(T)); }

	inline constexpr std::string theirs(GrammarObject* T) { return T->pronouns()->independent_possessive; }
	inline constexpr std::string Theirs(GrammarObject* T) { return capitalize(theirs(T)); }

	inline constexpr std::string themselves(GrammarObject* T) { return T->pronouns()->reflexive; }
	inline constexpr std::string Themselves(GrammarObject* T) { return capitalize(themselves(T)); }
}

#endif // !LEMONADE_GAME_SRC_SNOWY_GRAMMAR_H
