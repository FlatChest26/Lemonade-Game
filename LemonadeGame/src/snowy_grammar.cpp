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
}