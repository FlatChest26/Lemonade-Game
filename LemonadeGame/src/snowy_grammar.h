#pragma once

#ifndef LEMONADE_GAME_SRC_SNOWY_GRAMMAR_H
#define LEMONADE_GAME_SRC_SNOWY_GRAMMAR_H

#include <string>
#include <vector>

#include "snowy_macros.h"
#include "snowy_database.h"
#include "raw_master.h"

#include "raw_object_checker.h"

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

inline std::string trim(const std::string& s)
{
	const std::string whitespace = " \t\n\r\f\v";
	std::string new_string = s;

	size_t end = s.find_last_not_of(whitespace);
	if (end != std::string::npos)
	{
		new_string.erase(end + 1);
	}
	else
	{
		new_string.clear();
		return new_string;
	}

	size_t start = s.find_first_not_of(whitespace);

	if (start != std::string::npos)
	{
		new_string.erase(0, start);
	}

	return new_string;
}

namespace grammar
{
	inline std::vector<char> vowels{ 'a', 'e', 'i', 'o', 'u' };

	extern std::string a(std::string noun);
	inline std::string A(std::string noun) { return capitalize(a(noun)); }

	inline std::string an(std::string noun) { return a(noun); }
	inline std::string An(std::string noun) { return A(noun); }

	extern std::string the(std::string noun);
	inline std::string The(std::string noun) { return capitalize(the(noun)); }
}

struct Noun
{
	std::string singular = "";
	std::string plural = "";
	bool always_plural = false;

	constexpr operator const char* () const { return singular.c_str(); }
	constexpr operator std::string() const { return singular; }

	constexpr bool empty() const { return singular[0] == '\0' && plural[0] == '\0'; }

	static std::shared_ptr<RawObjectChecker> get_checker()
	{
		static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

		checker->set_type_name("noun");

		checker->add_property("singular", RawValueType::RAW_String, true);
		checker->add_property("plural", RawValueType::RAW_String, false);
		checker->add_property("always_plural", RawValueType::RAW_Bool, false);

		return checker;
	}

	static Noun from_json(const json& j)
	{
		auto checker = Noun::get_checker();

		if (!checker->check_json(j, true))
		{
			return {};
		}

		Noun n;

		n.singular = j.at("singular").get<std::string>();

		if (checker->is_valid_property("plural", j))
		{
			n.plural = j.at("plural").get<std::string>();
		}
		else
		{
			n.plural = n.singular + "s";
		}

		if (checker->is_valid_property("always_plural", j))
		{
			n.always_plural = j.at("always_plural").get<bool>();
		}

		return n;
	}
};

namespace grammar
{
	extern std::string a(Noun noun, bool plural = false);
	inline std::string A(Noun noun, bool plural = false) { return capitalize(a(noun, plural)); }

	inline std::string an(Noun noun, bool plural = false) { return a(noun, plural); }
	inline std::string An(Noun noun, bool plural = false) { return A(noun, plural); }

	extern std::string the(Noun noun, bool plural = false);
	inline std::string The(Noun noun, bool plural = false) { return capitalize(the(noun, plural)); }
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

#include <variant>
using GrammarNoun = std::variant<Noun, std::string>; // std::string means its a PROPER NOUN for now! (fml)

namespace grammar
{
	extern bool is_proper(GrammarNoun noun);
	extern bool is_plural(GrammarNoun noun);

	extern std::string a(GrammarNoun noun, bool plural = false);
	inline std::string A(GrammarNoun noun, bool plural = false) { return capitalize(a(noun, plural)); }

	inline std::string an(GrammarNoun noun, bool plural = false) { return a(noun, plural); }
	inline std::string An(GrammarNoun noun, bool plural = false) { return A(noun, plural); }

	extern std::string the(GrammarNoun noun, bool plural = false);
	inline std::string The(GrammarNoun noun, bool plural = false) { return capitalize(the(noun, plural)); }
}

namespace std
{
	inline constexpr string to_string(Noun noun) { return noun.singular; }
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
		else if (j.is_object())
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
	std::string subjective;
	std::string objective;
	std::string determiner;
	std::string independent_possessive;
	std::string reflexive;
	std::string shortened;

	bool acts_plural = false;

	constexpr operator const char* () const { return shortened.c_str(); }
	constexpr operator std::string() const { return shortened; }

	static std::shared_ptr<RawObjectChecker> get_checker()
	{
		static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

		checker->set_type_name("pronouns");

		checker->add_property("subjective", RawValueType::RAW_String, true);
		checker->add_property("objective", RawValueType::RAW_String, true);
		checker->add_property("determiner", RawValueType::RAW_String, true);
		checker->add_property("independent_possessive", RawValueType::RAW_String, true);
		checker->add_property("reflexive", RawValueType::RAW_String, true);

		checker->add_property("shortened", RawValueType::RAW_String, false);
		checker->add_property("acts_plural", RawValueType::RAW_Bool, false);

		return checker;
	}

	static Pronouns from_json(const json& j)
	{
		auto checker = Pronouns::get_checker();

		if (!checker->check_json(j, true))
		{
			return {};
		}

		Pronouns p;

		p.subjective = j.at("subjective").get<std::string>();
		p.objective = j.at("objective").get<std::string>();
		p.determiner = j.at("determiner").get<std::string>();
		p.independent_possessive = j.at("independent_possessive").get<std::string>();
		p.reflexive = j.at("reflexive").get<std::string>();

		if (checker->is_valid_property("shortened", j))
		{
			p.shortened = j.at("shortened").get<std::string>();
		}
		else
		{
			if (p.objective == p.subjective)
			{
				p.shortened = p.subjective + "/" + p.determiner;
			}
			else
			{
				p.shortened = p.subjective + "/" + p.objective;
			}
		}

		if (checker->is_valid_property("acts_plural", j))
		{
			p.acts_plural = j.at("acts_plural").get<bool>();
		}

		return p;
	}
};


inline Pronouns DEFAULT_NEUTER_PRONOUNS = Pronouns("it", "it", "its", "its", "itself", "it/its", false);
inline Pronouns DEFAULT_EPICENE_PRONOUNS = Pronouns("they", "them", "their", "theirs", "themself", "they/them", true);


enum class GenderType : uint8_t
{
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
	GenderType gender_type;

	Noun mature_noun;
	Noun immature_noun;

	bool is_plural = false;
	bool uses_noun = true;

	bool is_neuter() const { return gender_type & GenderType::NEUTER; }
	bool is_epicene() const { return gender_type & GenderType::EPICENE; }
	bool is_masculine() const { return gender_type & GenderType::MASCULINE; }
	bool is_feminine() const { return gender_type & GenderType::FEMININE; }

	static std::shared_ptr<RawObjectChecker> get_checker()
	{
		static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

		checker->set_type_name("gender");

		checker->add_property("name", RawValueType::RAW_String, true);
		checker->add_property("pronouns", RawValueType::RAW_Custom, true);
		checker->add_property("gender_type", RawValueType::RAW_String, true);

		checker->add_property("mature_noun", RawValueType::RAW_Custom, false, Noun::get_checker());
		checker->add_property("immature_noun", RawValueType::RAW_Custom, false, Noun::get_checker());

		checker->add_property("noun", RawValueType::RAW_Custom, false, Noun::get_checker());
		checker->add_property("nouns", RawValueType::RAW_Object, false);

		checker->add_property("is_plural", RawValueType::RAW_Bool, false);
		checker->add_property("uses_noun", RawValueType::RAW_Bool, false);

		return checker;
	}

	static Gender from_json(const json& j)
	{
		auto checker = Gender::get_checker();

		if (!checker->check_json(j, true))
		{
			return {};
		}

		Gender g;

		g.name = j.at("name").get<std::string>();
		g.gender_type = GenderType::NEUTER;
		
		if (j.at("pronouns").is_string())
		{
			g.pronouns = RawMaster::get<Pronouns>(j.at("pronouns").get<std::string>());
		}
		else if (Pronouns::get_checker()->check_json(j.at("pronouns")))
		{
			g.pronouns = RawMaster::add<Pronouns>(j.at("ID"), Pronouns::from_json(j.at("pronouns")));
		}

		if (AllGenderTypes.find(j.at("gender_type").get<std::string>()) != AllGenderTypes.end())
		{
			g.gender_type = AllGenderTypes.at(j.at("gender_type").get<std::string>());
		}

		if (checker->is_valid_property("is_plural", j))
		{
			g.is_plural = j.at("is_plural").get<bool>();
		}

		if (checker->is_valid_property("mature_noun", j))
		{
			g.mature_noun = Noun::from_json(j.at("mature_noun"));
		}

		if (checker->is_valid_property("immature_noun", j))
		{
			g.immature_noun = Noun::from_json(j.at("immature_noun"));
		}

		if (checker->is_valid_property("noun", j))
		{
			g.mature_noun = Noun::from_json(j.at("noun"));
			g.immature_noun = g.mature_noun;
		}

		if (checker->is_valid_property("nouns", j))
		{
			if (j.at("nouns").contains("mature"))
			{
				g.mature_noun = Noun::from_json(j.at("nouns").at("mature"));
			}

			if (j.at("nouns").contains("immature"))
			{
				g.immature_noun = Noun::from_json(j.at("nouns").at("immature"));
			}
		}

		if (checker->is_valid_property("uses_noun", j))
		{
			g.uses_noun = j.at("uses_noun").get<bool>();
		}
		else
		{
			g.uses_noun = (!g.mature_noun.empty() || !g.immature_noun.empty());
		}

		return g;
	}

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
			return prefix() + name().nickname + suffix();
		return prefix() + singular_noun() + suffix();
	}

	virtual std::string full_name() const
	{
		if (is_named())
			return prefix() + name().full_name + suffix();
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

inline constexpr std::string possessive(const GrammarObject& T) { return T.nickname() + "'s"; }
inline constexpr std::string Possessive(const GrammarObject& T) { return capitalize(possessive(T)); }

inline constexpr std::string they(const GrammarObject& T) { return T.pronouns()->subjective; }
inline constexpr std::string They(const GrammarObject& T) { return capitalize(they(T)); }

inline constexpr std::string them(const GrammarObject& T) { return T.pronouns()->objective; }
inline constexpr std::string Them(const GrammarObject& T) { return capitalize(them(T)); }

inline constexpr std::string their(const GrammarObject& T) { return T.pronouns()->determiner; }
inline constexpr std::string Their(const GrammarObject& T) { return capitalize(their(T)); }

inline constexpr std::string theirs(const GrammarObject& T) { return T.pronouns()->independent_possessive; }
inline constexpr std::string Theirs(const GrammarObject& T) { return capitalize(theirs(T)); }

inline constexpr std::string themselves(const GrammarObject& T) { return T.pronouns()->reflexive; }
inline constexpr std::string Themselves(const GrammarObject& T) { return capitalize(themselves(T)); }

template<typename GramObj>
inline constexpr std::string possessive(const GramObj& T) { return T.nickname() + "'s"; }
template<typename GramObj>
inline constexpr std::string Possessive(const GramObj& T) { return capitalize(possessive(T)); }

template<typename GramObj>
inline constexpr std::string they(const GramObj& T) { return T.pronouns()->subjective; }
template<typename GramObj>
inline constexpr std::string They(const GramObj& T) { return capitalize(they(T)); }

template<typename GramObj>
inline constexpr std::string them(const GramObj& T) { return T.pronouns()->objective; }
template<typename GramObj>
inline constexpr std::string Them(const GramObj& T) { return capitalize(them(T)); }

template<typename GramObj>
inline constexpr std::string their(const GramObj& T) { return T.pronouns()->determiner; }
template<typename GramObj>
inline constexpr std::string Their(const GramObj& T) { return capitalize(their(T)); }

template<typename GramObj>
inline constexpr std::string theirs(const GramObj& T) { return T.pronouns()->independent_possessive; }
template<typename GramObj>
inline constexpr std::string Theirs(const GramObj& T) { return capitalize(theirs(T)); }

template<typename GramObj>
inline constexpr std::string themselves(const GramObj& T) { return T.pronouns()->reflexive; }
template<typename GramObj>
inline constexpr std::string Themselves(const GramObj& T) { return capitalize(themselves(T)); }

#endif // !LEMONADE_GAME_SRC_SNOWY_GRAMMAR_H
