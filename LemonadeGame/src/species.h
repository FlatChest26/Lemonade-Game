#pragma once

#include "snowy_grammar.h"
#include "renderable.h"
#include "snowy_database.h"
#include "body_plan.h"
#include "gait.h"

#include "raw_master.h"
#include "raw_object_checker.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

enum class SpeciesFlag : uint64_t
{
	NONE = 0,

	MUNDANE = 1 << 0,
	DOES_NOT_EXIST = 1 << 1,
	PART_OF_SOCIETY = 1 << 2,
};

IS_FLAG_ENUM(SpeciesFlag);

inline std::map<std::string, SpeciesFlag> AllSpeciesFlags
{
	{ "NONE",				SpeciesFlag::NONE				},
	{ "MUNDANE",			SpeciesFlag::MUNDANE			},
	{ "DOES_NOT_EXIST",		SpeciesFlag::DOES_NOT_EXIST		},
	{ "PART_OF_SOCIETY",	SpeciesFlag::PART_OF_SOCIETY	},
};

struct GenderedNouns
{
	std::unordered_map<GenderType, Noun> immature;
	std::unordered_map<GenderType, Noun> mature;

	bool has_immature_gendered_noun(const GenderType& gender_type) const
	{
		for (auto& [type, noun] : immature)
		{
			if (gender_type & type)
				return true;
		}

		return false;
	}

	bool has_mature_gendered_noun(const GenderType& gender_type) const
	{
		for (auto& [type, noun] : mature)
		{
			if (gender_type & type)
				return true;
		}

		return false;
	}

	Noun get_immature_gendered_noun(const GenderType& gender_type) const
	{
		for (auto& [type, noun] : immature)
		{
			if (gender_type & type)
				return noun;
		}

		return {};
	}

	Noun get_mature_gendered_noun(const GenderType& gender_type) const
	{
		for (auto& [type, noun] : mature)
		{
			if (gender_type & type)
				return noun;
		}

		return {};
	}
};

using SpeciesID = std::string;

struct Species
{
	SpeciesID ID;
	Noun noun{ "unknown creature", "unknown creatures" };
	std::string adjective{ noun.singular };
	GenderedNouns gendered_nouns;

	Renderable renderable{ '@', color::get("cyan"), color::get("black") };

	RawMaster::Ref<BodyPlan> default_body_plan{ nullptr };
	std::vector<RawMaster::Ref<BodyMod>> body_mods{};

	std::string description{ "A generic species." };

	GaitMap* default_gaits{ &gaits::StandardGaits };

	Age lifespan_min{ 60 }; // The minimum age a creature of this species can live to
	Age lifespan_max{ 120 }; // The max age a creature of this species can live to

	Age child_age{ 12 }; // The age at which this creature stops being a baby and is considered a child
	Age adult_age{ 18 }; // The age at which this creature stops being a child and is considered an adult
	Age mature_age{ 20 }; // The age at which this creature stops is considered "mature"

	double frequency{ 1.0f }; // How often this species is randomly generated

	SpeciesFlag flags{ SpeciesFlag::NONE };

	std::unordered_map<GenderType, double> common_genders;

	bool has_common_gender_type(GenderType gender_type) const
	{
		return common_genders.contains(gender_type);
	}

	double get_common_gender_type_frequency(GenderType gender_type) const
	{
		return common_genders.at(gender_type);
	}

public:

	// -- Flags -- //

	bool is_mundane() const { return flags & SpeciesFlag::MUNDANE; }
	bool does_not_exist() const { return flags & SpeciesFlag::DOES_NOT_EXIST; }

public:
	bool operator==(const Species& other) const { return other.ID == ID; }

	static std::shared_ptr<RawObjectChecker> get_checker();
	static Species from_json(const json& j);
};

namespace species
{
	inline Species NULL_CREATURE
	{
		.ID = "NULL_CREATURE",
		.noun = { "unknown", "unknowns" },
		.renderable = { 001, color::get("gray"), color::get("black") },
		.flags = SpeciesFlag::DOES_NOT_EXIST
	};
}
