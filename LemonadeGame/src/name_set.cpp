#include "name_set.h"

NameSet NameSet::from_json(const json& j)
{
	NameSet ns;

	ns.ID = j.at("ID");

	if (j.contains("names"))
	{
		if (!j.at("names").is_array())
		{
			CERR("Invalid 'names' field in noun JSON object: " << j);
		}
		else
		{
			ns.names_json = j.at("names");
		}
	}

	return ns;
}
