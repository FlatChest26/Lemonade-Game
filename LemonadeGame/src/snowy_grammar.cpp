#include "snowy_grammar.h"

namespace snowy::grammar
{
	std::string a(std::string noun)
	{
		for (auto& vowel : vowels)
		{
			if (std::tolower(noun[0]) == vowel)
			{
				return "an " + noun;
			}
		}

		return "a " + noun;
	}

	std::string the(std::string noun)
	{
		return "the " + noun;
	}

	std::string a(Noun noun, bool plural)
	{
		if (noun.always_plural)
		{
			return "some " + (plural ? noun.plural : noun.singular);
		}
		else if (plural)
		{
			return "some " + noun.plural;
		}

		return a(noun.singular);
	}

	std::string the(Noun noun, bool plural)
	{
		return the(plural ? noun.plural : noun.singular);
	}

	bool is_proper(GrammarNoun noun)
	{
		if (std::holds_alternative<Noun>(noun))
		{
			return false;
		}
		else if (std::holds_alternative<std::string>(noun))
		{
			return true;
		}

		return false;
	}

	bool is_plural(GrammarNoun noun)
	{
		if (std::holds_alternative<Noun>(noun))
		{
			return std::get<Noun>(noun).always_plural;
		}
		else if (std::holds_alternative<std::string>(noun))
		{
			if (std::get<std::string>(noun) == "you")
				return true;

			return false;
		}

		return false;
	}

	std::string a(GrammarNoun g_noun, bool plural)
	{
		if (std::holds_alternative<Noun>(g_noun))
		{
			return a(std::get<Noun>(g_noun), plural);
		}
		else if (std::holds_alternative<std::string>(g_noun)) // Proper noun
		{
			return std::get<std::string>(g_noun);
		}

		return "a ";
	}
	std::string the(GrammarNoun g_noun, bool plural)
	{
		if (std::holds_alternative<Noun>(g_noun))
		{
			return the(std::get<Noun>(g_noun), plural);
		}
		else if (std::holds_alternative<std::string>(g_noun)) // Proper noun
		{
			return the(std::get<std::string>(g_noun));
		}

		return "the ";
	}
}