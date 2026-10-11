#pragma once

#include "snowy_grammar.h"
#include "renderable.h"
#include "snowy_database.h"
#include "body_plan.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct Species
{
	std::string ID;
	Noun noun{ "unknown creature", "unknown creatures" };
	std::string adjective{ noun.singular };

	Renderable renderable{ '@', color::get("cyan"), color::get("black") };

	BodyPlan default_body_plan{ body_plan::GenericBody };
	std::vector<BodyMod> body_mods;

	std::string description{ "A generic species." };

	bool operator==(const Species& other) const { return other.ID == ID; }

	static Species from_json(const json& j);
};

namespace species
{
	using SpeciesDB = snowy::SingletonDatabase<Species>;

	inline Species HUMAN
	{
		.ID = "HUMAN",
		.noun = { "human", "humans" },
		.renderable = {'U', color::get("cyan"), color::get("black") }
	};

	inline snowy::Database<Species*> _species_db
	{
		{ "HUMAN", &HUMAN }
	};

	static inline void initialize_db()
	{
		SpeciesDB::init();
	}
}
