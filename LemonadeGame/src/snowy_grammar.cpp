#include "snowy_grammar.h"

namespace grammar
{
	constexpr std::string capitalize(std::string_view str)
	{
		std::string new_str{ str };

		if (str.empty())
		{
			return new_str;
		}

		new_str[0] = std::toupper(str[0]);
		return new_str;
	}

	std::string trim(std::string_view s)
	{
		const std::string whitespace = " \t\n\r\f\v";
		std::string new_string{ s };

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

	bool has_starting_vowel_sound(std::string_view noun)
	{
		if (noun.size() == 1)
		{
			for (auto& chr : "aefhilmnosx")
			{
				if (std::tolower(noun[0]) == chr)
					return true;
			}

			return false;
		}

		for (auto& vowel : "aeiou")
		{
			if (std::tolower(noun[0]) == vowel)
			{
				return true;
			}
		}

		return false;
	}

	std::string poss(std::string_view noun)
	{ 
		std::string new_string{ noun };
		return new_string + "'s";
	}

	std::string a(std::string_view noun)
	{
		std::string new_string{ noun };
		return has_starting_vowel_sound(noun) ? "an " + new_string : "a " + new_string;
	}

	std::string the(std::string_view noun)
	{
		std::string new_string{ noun };
		return "the " + new_string;
	}

	std::string poss(const Noun& noun, bool plural)
	{
		if (noun.always_plural)
		{
			return poss(std::string_view(plural ? noun.plural : noun.singular));;
		}
		else if (plural)
		{
			return poss(std::string_view(noun.plural));
		}

		return poss(std::string_view(noun.singular));
	}

	std::string a(const Noun& noun, bool plural)
	{
		if (noun.always_plural)
		{
			return "some " + (plural ? noun.plural : noun.singular);
		}
		else if (plural)
		{
			return "some " + noun.plural;
		}

		return a(std::string_view(noun.singular));
	}

	std::string the(const Noun& noun, bool plural)
	{
		return the(std::string_view(plural ? noun.plural : noun.singular));
	}

	std::string poss(const GrammarNoun& noun, bool plural)
	{
		return poss(noun.as_noun(), plural);
	}

	bool is_proper(const GrammarNoun& g_noun)
	{
		return g_noun.is_proper;
	}

	bool is_plural(const GrammarNoun& g_noun)
	{
		return g_noun.noun.always_plural;
	}

	std::string to_string(const GrammarNoun& g_noun, bool plural)
	{
		return g_noun.as_string();
	}

	std::string a(const GrammarNoun& g_noun, bool plural)
	{
		if (g_noun.is_proper)
		{
			return g_noun.as_string();
		}

		return a(g_noun.as_noun(), plural);
	}
	std::string the(const GrammarNoun& g_noun, bool plural)
	{
		if (g_noun.is_proper)
		{
			return g_noun.as_string();
		}

		return the(g_noun.as_noun(), plural);
	}

	std::string say_regarding(const Verb& verb, const Noun& noun, bool plural)
	{
		if (noun.always_plural || plural)
		{
			return verb.plural;
		}

		return verb.singular;
	}

	std::string say_regarding(const Verb& verb, const GrammarNoun& g_noun, bool plural)
	{
		if (g_noun.noun.always_plural || plural)
		{
			return verb.plural;
		}

		return verb.singular;
	}

	std::string say_regarding(const Verb& verb, int num)
	{
		if (num == 1)
		{
			return verb.singular;
		}

		return verb.plural;
	}

	std::string word_replace(std::string_view str, const WordReplacement& replacement)
	{
		std::string new_word{ str };

		if (!replacement.empty())
		{
			size_t pos = new_word.find(replacement.old_str);
			
			if (pos != std::string::npos)
			{
				new_word.replace(pos, replacement.old_str.length(), replacement.new_str);
			}
		}

		return new_word;
	}

	Noun noun_replace(const Noun& noun, const NounReplacement& replacement)
	{
		Noun new_noun = noun;

		if (!replacement.singular.empty())
		{
			new_noun.singular = word_replace(noun.singular, replacement.singular);
		}

		if (!replacement.plural.empty())
		{
			new_noun.plural = word_replace(noun.plural, replacement.plural);
		}

		return new_noun;
	}
}

std::shared_ptr<RawObjectChecker> Noun::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("noun");

	checker->add_property("singular", RawValueType::RAW_String, true);
	checker->add_property("plural", RawValueType::RAW_String, false);
	checker->add_property("always_plural", RawValueType::RAW_Bool, false);

	return checker;
}

Noun Noun::from_json(const json& j)
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

Name Name::from_json(const json& j)
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

Age Age::from_json(const json& j)
{
	Age a;

	if (j.is_number())
	{
		a.physical_age = j.get<Age_t>();
		a.chronological_age = j.get<Age_t>();
	}
	else if (j.is_array() && j.size() > 0)
	{
		if (j.at(0).is_number())
		{
			a.physical_age = j.get<Age_t>();
		}

		if (j.size() > 1 && j.at(1).is_number())
		{
			a.chronological_age = j.at(1).get<Age_t>();
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
				a.physical_age = j.at("physical_age").get<Age_t>();
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
				a.chronological_age = j.at("chronological_age").get<Age_t>();
			}
		}
	}

	return a;
}

std::shared_ptr<RawObjectChecker> Pronouns::get_checker()
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

Pronouns Pronouns::from_json(const json& j)
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

std::shared_ptr<RawObjectChecker> Gender::get_checker()
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

Gender Gender::from_json(const json& j)
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

std::shared_ptr<RawObjectChecker> Verb::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("verb");

	checker->add_property("singular", RawValueType::RAW_String, true);
	checker->add_property("plural", RawValueType::RAW_String, true);

	return checker;
}

Verb Verb::from_json(const json& j)
{
	auto checker = Verb::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	Verb v;

	v.singular = j.at("singular").get<std::string>();
	v.plural = j.at("plural").get<std::string>();

	return v;
}

std::shared_ptr<RawObjectChecker> WordReplacement::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("word_replacement");

	checker->add_property("old", RawValueType::RAW_String, true);
	checker->add_property("new", RawValueType::RAW_String, true);

	return checker;
}

WordReplacement WordReplacement::from_json(const json& j)
{
	auto checker = WordReplacement::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	WordReplacement wr;

	wr.old_str = j.at("old").get<std::string>();
	wr.new_str = j.at("new").get<std::string>();

	return wr;
}

std::shared_ptr<RawObjectChecker> NounReplacement::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("word_replacement");

	checker->add_property("singular", RawValueType::RAW_Custom, true, WordReplacement::get_checker());
	checker->add_property("plural", RawValueType::RAW_Custom, true, WordReplacement::get_checker());

	return checker;
}

NounReplacement NounReplacement::from_json(const json& j)
{
	auto checker = NounReplacement::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	NounReplacement nr;

	nr.singular = WordReplacement::from_json(j.at("singular"));
	nr.plural = WordReplacement::from_json(j.at("plural"));

	return nr;
}