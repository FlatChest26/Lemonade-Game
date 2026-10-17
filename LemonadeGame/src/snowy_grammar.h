#pragma once

#ifndef LEMONADE_GAME_SRC_SNOWY_GRAMMAR_H
#define LEMONADE_GAME_SRC_SNOWY_GRAMMAR_H

#include <string>
#include <vector>

#include "snowy_macros.h"
#include "snowy_database.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "enum_traits.h"

template<typename T, typename Predicate>
inline std::string english_list(T list, Predicate to_str)
{
	std::string final_list = "";

	size_t i = 0;

	for (const auto& item : list)
	{
		final_list += to_str(item);

		if (i == list.size() - 1)
		{
		}
		else if (list.size() > 2)
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

		i++;
	}

	return final_list;
}

inline constexpr std::string capitalize(std::string str)
{
	if (str.empty())
	{
		return str;
	}

	str[0] = std::toupper(str[0]);

	return str;
}

namespace snowy::grammar
{
	inline std::vector<char> vowels { 'a', 'e', 'i', 'o', 'u' };

	extern std::string a(std::string noun);
	inline std::string A(std::string noun) { return capitalize(a(noun)); }

	inline std::string an(std::string noun) { return a(noun); }
	inline std::string An(std::string noun) { return A(noun); }

	
}

struct Noun
{
	std::string singular = "";
	std::string plural = "";
	bool always_plural = false;

	constexpr operator const char* () const { return singular.c_str(); }
	constexpr operator std::string() const { return singular; }

	constexpr bool empty() const { return singular[0] == '\0' && plural[0] == '\0'; }

	static Noun from_json(const json& j)
	{
		Noun n;

		if (j.contains("singular"))
		{
			if (!j.at("singular").is_string())
			{
				CERR("Invalid 'singular' field in noun JSON object: " << j);
				n.singular = "<invalid noun>";
			}
			else
			{
				n.singular = j.at("singular").get<std::string>();
			}
		}

		if (j.contains("plural"))
		{
			if (!j.at("plural").is_string())
			{
				CERR("Invalid 'plural' field in noun JSON object: " << j);
				n.plural = "<invalid noun>";
			}
			else
			{
				n.plural = j.at("plural").get<std::string>();
			}
		}

		if (j.contains("always_plural"))
		{
			if (!j.at("always_plural").is_boolean())
			{
				CERR("Invalid 'always_plural' field in noun JSON object: " << j);
				n.always_plural = false;
			}
			else
			{
				n.always_plural = j.at("always_plural").get<bool>();
			}
		}

		return n;
	}
};

namespace snowy::grammar
{
	extern std::string a(Noun noun, bool plural = false);
	inline std::string A(Noun noun, bool plural = false) { return capitalize(a(noun, plural)); }

	inline std::string an(Noun noun, bool plural = false) { return a(noun, plural); }
	inline std::string An(Noun noun, bool plural = false) { return A(noun, plural); }
}

namespace std
{
	inline constexpr string to_string(Noun noun) { return noun.singular; }
}

struct Name
{
	std::string nickname = "";
	std::string full_name{ nickname };

	constexpr operator const char* () const { return nickname.c_str(); }
	constexpr operator std::string() const { return nickname; }

	constexpr bool empty() const { return nickname[0] == '\0' && full_name[0] == '\0'; }

	static Name from_json(const json& j)
	{
		Name n;

		if (j.is_string())
		{
			n.nickname = j.get<std::string>();
			n.full_name = j.get<std::string>();
		}
		else if (j.is_array() && j.size() > 0)
		{
			if (j.at(0).is_string())
			{
				n.nickname = j.at(0).get<std::string>();
			}

			if (j.size() > 1 && j.at(1).is_string())
			{
				n.full_name = j.at(1).get<std::string>();
			}
			else
			{
				n.full_name = n.nickname;
			}
		}
		else if (j.is_structured())
		{
			if (j.contains("nickname"))
			{
				if (!j.at("nickname").is_string())
				{
					CERR("Invalid 'nickname' field in noun JSON object: " << j);
					n.nickname = "<invalid nickname>";
				}
				else
				{
					n.nickname = j.at("nickname").get<std::string>();
				}
			}

			if (j.contains("full_name"))
			{
				if (!j.at("full_name").is_string())
				{
					CERR("Invalid 'full_name' field in noun JSON object: " << j);
					n.full_name = "<invalid full name>";
				}
				else
				{
					n.full_name = j.at("full_name").get<std::string>();
				}
			}
		}
		return n;
	}

};

namespace std
{
	inline constexpr string to_string(Name name) { return name.full_name; }
}

struct Age
{
	using age_t = short;

	age_t physical_age{ 18 };
	age_t chronological_age{ physical_age };

	constexpr bool operator==(Age other) const { return physical_age == other.physical_age && chronological_age == other.chronological_age; }
	constexpr operator age_t() const { return physical_age; }

	static Age from_json(const json& j)
	{
		Age a;

		if (j.is_number())
		{
			a.physical_age = j.get<age_t>();
			a.chronological_age = j.get<age_t>();
		}
		else if (j.is_array() && j.size() > 0)
		{
			if (j.at(0).is_number())
			{
				a.physical_age = j.get<age_t>();
			}

			if (j.size() > 1 && j.at(1).is_number())
			{
				a.chronological_age = j.at(1).get<age_t>();
			}
			else
			{
				a.chronological_age = a.physical_age;
			}
		}
		else if (j.is_structured())
		{
			if (j.contains("physical_age"))
			{
				if (!j.at("physical_age").is_number())
				{
					CERR("Invalid 'physical_age' field in noun JSON object: " << j);
					a.physical_age = 18;
				}
				else
				{
					a.physical_age = j.at("physical_age").get<age_t>();
				}
			}

			if (j.contains("chronological_age"))
			{
				if (!j.at("chronological_age").is_number())
				{
					CERR("Invalid 'chronological_age' field in noun JSON object: " << j);
					a.chronological_age = 18;
				}
				else
				{
					a.chronological_age = j.at("chronological_age").get<age_t>();
				}
			}
		}

		return a;
	}
};

struct Pronouns
{
	const char* subjective;
	const char* objective;
	const char* determiner;
	const char* independent_possessive;
	const char* reflexive;
	const char* shortened;

	bool acts_plural = false;

	constexpr operator const char* () const { return shortened; }
	constexpr operator std::string() const { return shortened; }
};

namespace pronouns
{
	inline Pronouns IT_ITS
	{
		.subjective = "it",
		.objective = "it",
		.determiner = "its",
		.independent_possessive = "its",
		.reflexive = "itself",
		.shortened = "it/its",
		.acts_plural = false
	};

	inline Pronouns THEY_THEM
	{
		.subjective = "they",
		.objective = "them",
		.determiner = "their",
		.independent_possessive = "theirs",
		.reflexive = "themself",
		.shortened = "they/them",
		.acts_plural = true
	};

	inline Pronouns PLURAL_THEY_THEM
	{
		.subjective = "they",
		.objective = "them",
		.determiner = "their",
		.independent_possessive = "theirs",
		.reflexive = "themselves",
		.shortened = "they/them",
		.acts_plural = true
	};

	inline Pronouns HE_HIM
	{
		.subjective = "he",
		.objective = "him",
		.determiner = "his",
		.independent_possessive = "his",
		.reflexive = "himself",
		.shortened = "he/him",
		.acts_plural = false
	};

	inline Pronouns SHE_HER
	{
		.subjective = "she",
		.objective = "her",
		.determiner = "her",
		.independent_possessive = "hers",
		.reflexive = "herself",
		.shortened = "she/her",
		.acts_plural = false
	};
}

struct Gender
{
	Noun noun;
	Noun young_noun; 

	Pronouns pronouns;

	bool is_plural = false;
	bool uses_noun = true;

	enum class type : int
	{
		NEUTER = 1 << 0,
		EPICENE = 1 << 1,
		MASCULINE = 1 << 2,
		FEMININE = 1 << 3,
	};

	type gender_type;
};

IS_FLAG_ENUM(Gender::type)

inline std::map<std::string, Gender::type> AllGenderTypes{
	{ "NEUTER",		Gender::type::NEUTER	},
	{ "EPICENE",	Gender::type::EPICENE	},
	{ "MASCULINE",	Gender::type::MASCULINE },
	{ "FEMININE",	 Gender::type::FEMININE },

};

namespace gender
{
	inline Gender NEUTER{
		.noun = {},
		.pronouns = pronouns::IT_ITS,
		.is_plural = false,
		.uses_noun = false,
		.gender_type = Gender::type::NEUTER
	};

	inline Gender NON_SPECIFIC{
		.noun = {"person", "people"},
		.young_noun = { "young person", "young people" },
		.pronouns = pronouns::THEY_THEM,
		.is_plural = false,
		.uses_noun = false,
		.gender_type = Gender::type::EPICENE
	};

	inline Gender MASCULINE{
		.noun = {"man", "men"},
		.young_noun = { "boy", "boys" },
		.pronouns = pronouns::HE_HIM,
		.is_plural = false,
		.uses_noun = true,
		.gender_type = Gender::type::MASCULINE
	};

	inline Gender FEMININE{
		.noun = {"woman", "women"},
		.young_noun = { "girl", "girls" },
		.pronouns = pronouns::SHE_HER,
		.is_plural = false,
		.uses_noun = true,
		.gender_type = Gender::type::FEMININE
	};
}

enum class SexualAttraction : unsigned char
{
	OPPOSITE = 1 << 0,
	SAME = 1 << 1,
	MALE = 1 << 2,
	FEMALE = 1 << 3,

	NEUTER = 1 << 4,
	EPICENE = 1 << 5,
	MASCULINE = 1 << 6,
	FEMININE = 1 << 7,

	NONE = 0,
	ANY = OPPOSITE | SAME | MALE | FEMALE | NEUTER | EPICENE | MASCULINE | FEMININE
};

IS_FLAG_ENUM(SexualAttraction)

struct Sexuality
{
	Noun noun;
	SexualAttraction attraction;
};

namespace sexuality
{
	inline Sexuality HETEROSEXUAL
	{
		.noun = {"heterosexual", "heterosexuals"},
		.attraction = SexualAttraction::SAME
	};

	inline Sexuality HOMOSEXUAL
	{
		.noun = {"homosexual", "homosexuals"},
		.attraction = SexualAttraction::OPPOSITE
	};

	inline Sexuality ASEXUAL
	{
		.noun = {"asexual", "asexuals"},
		.attraction = SexualAttraction::NONE
	};

	inline Sexuality BISEXUAL
	{
		.noun = {"bisexual", "bisexuals"},
		.attraction = SexualAttraction::MALE | SexualAttraction::FEMALE
	};

	inline Sexuality GYNESEXUAL
	{
		.noun = {"gynesexual", "gynesexuals"},
		.attraction = SexualAttraction::FEMININE
	};

	inline Sexuality ANDROSEXUAL
	{
		.noun = {"androsexual", "androsexuals"},
		.attraction = SexualAttraction::MASCULINE
	};

	inline Sexuality PANSEXUAL
	{
		.noun = {"pansexual", "pansexuals"},
		.attraction = SexualAttraction::ANY
	};
}

enum class SexualPosition : short
{
	DOM = 1 << 0, VERSE = 1 << 1, SUB = 1 << 2,
	TOP = 1 << 3, SWITCH = 1 << 4, BOTTOM = 1 << 5,

	DOM_TOP = DOM | TOP,
	DOM_SWITCH = DOM | SWITCH,
	DOM_BOTTOM = DOM | BOTTOM,

	VERSE_TOP = VERSE | TOP,
	VERSE_SWITCH = VERSE | SWITCH,
	VERSE_BOTTOM = VERSE | BOTTOM,

	SUB_TOP = SUB | TOP,
	SUB_SWITCH = SUB | SWITCH,
	SUB_BOTTOM = SUB | BOTTOM
};

IS_FLAG_ENUM(SexualPosition)

struct GrammarObject
{
	virtual Noun get_noun() const { return Noun{ "grammar object", "grammar objects" }; }
	virtual Name get_name() const { return Name(); }
	virtual Pronouns get_pronouns() const { return pronouns::IT_ITS; }

	std::string singular_noun() const { return get_noun().singular; }
	std::string plural_noun() const { return get_noun().plural; }

	bool is_named() const { return !get_name().empty(); }

	std::string nickname() const
	{
		if (is_named())
			return prefix() + get_name().nickname + suffix();
		return prefix() + singular_noun() + suffix();
	}

	std::string full_name() const
	{
		if (is_named())
			return prefix() + get_name().full_name + suffix();
		return prefix() + singular_noun() + suffix();
	}

	std::string they() const { return get_pronouns().subjective; }
	std::string them() const { return get_pronouns().objective; }
	std::string their() const { return get_pronouns().determiner; }
	std::string theirs() const { return get_pronouns().independent_possessive; }
	std::string themselves() const { return get_pronouns().reflexive; }

	virtual bool is_plural() const { return get_noun().always_plural; }
	virtual bool is_pronouns_plural() const { return get_pronouns().acts_plural; }

	virtual std::string prefix() const { return ""; }
	virtual std::string suffix() const { return ""; }
};


inline constexpr std::string possessive(const GrammarObject& T) { return T.nickname() + "'s"; }
inline constexpr std::string Possessive(const GrammarObject& T) { return capitalize(possessive(T)); }

inline constexpr std::string they(const GrammarObject& T) { return T.get_pronouns().subjective; }
inline constexpr std::string They(const GrammarObject& T) { return capitalize(they(T)); }

inline constexpr std::string them(const GrammarObject& T) { return T.get_pronouns().objective; }
inline constexpr std::string Them(const GrammarObject& T) { return capitalize(them(T)); }

inline constexpr std::string their(const GrammarObject& T) { return T.get_pronouns().determiner; }
inline constexpr std::string Their(const GrammarObject& T) { return capitalize(their(T)); }

inline constexpr std::string theirs(const GrammarObject& T) { return T.get_pronouns().independent_possessive; }
inline constexpr std::string Theirs(const GrammarObject& T) { return capitalize(theirs(T)); }

inline constexpr std::string themselves(const GrammarObject& T) { return T.get_pronouns().reflexive; }
inline constexpr std::string Themselves(const GrammarObject& T) { return capitalize(themselves(T)); }

template<typename GramObj>
inline constexpr std::string possessive(const GramObj& T) { return T.nickname() + "'s"; }
template<typename GramObj>
inline constexpr std::string Possessive(const GramObj& T) { return capitalize(possessive(T)); }

template<typename GramObj>
inline constexpr std::string they(const GramObj& T) { return T.get_pronouns().subjective; }
template<typename GramObj>
inline constexpr std::string They(const GramObj& T) { return capitalize(they(T)); }

template<typename GramObj>
inline constexpr std::string them(const GramObj& T) { return T.get_pronouns().objective; }
template<typename GramObj>
inline constexpr std::string Them(const GramObj& T) { return capitalize(them(T)); }

template<typename GramObj>
inline constexpr std::string their(const GramObj& T) { return T.get_pronouns().determiner; }
template<typename GramObj>
inline constexpr std::string Their(const GramObj& T) { return capitalize(their(T)); }

template<typename GramObj>
inline constexpr std::string theirs(const GramObj& T) { return T.get_pronouns().independent_possessive; }
template<typename GramObj>
inline constexpr std::string Theirs(const GramObj& T) { return capitalize(theirs(T)); }

template<typename GramObj>
inline constexpr std::string themselves(const GramObj& T) { return T.get_pronouns().reflexive; }
template<typename GramObj>
inline constexpr std::string Themselves(const GramObj& T) { return capitalize(themselves(T)); }

#endif // !LEMONADE_GAME_SRC_SNOWY_GRAMMAR_H
