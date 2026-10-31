#pragma once

#ifndef LEMONADE_GAME_SRC_BODY_PART_TYPE_H
#define LEMONADE_GAME_SRC_BODY_PART_TYPE_H

#include "snowy_units.h"
#include "snowy_grammar.h"
#include "enum_traits.h"
#include "snowy_database.h"

#include "body_part_components.h"
#include "raw_object_checker.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace units::literals;

constexpr auto AVERAGE_HEIGHT = 5_ft + 6_in;
constexpr auto AVERAGE_WEIGHT = 150_lb;

using BodyPartID = std::string;

enum class BodyPartFlag : uint64_t
{
	NONE = 0,

	SENSORY = 1 << 0,

	GRASP = 1 << 1,
	LIMB = 1 << 2,
	STANCE = 1 << 3,
	INTERNAL = 1 << 4,

	THOUGHT = 1 << 6,
	CIRCULATION = 1 << 7,
	DIGESTION = 1 << 8,

	EAT = 1 << 9,
	SPEAK = 1 << 10,

	PHALLUS = 1 << 11,
	ORIFICE = 1 << 12,
	HANDLE = 1 << 13,
	FLUID_PRODUCER = 1 << 15,
	CLEAVAGE = 1 << 16,

	LEWD = 1 << 17,
	KINKY = 1 << 18,
	SUGGESTIVE = 1 << 19,
	PRETTY = 1 << 20,

	CHOKABLE = 1 << 21,
};

enum class BodyPartCategory
{
	OTHER,

	HEAD,
	NECK,
	UPPER_BODY,
	LOWER_BODY,
	UPPER_ARM,
	LOWER_ARM,
	HAND,
	GROIN,
	UPPER_LEG,
	LOWER_LEG,
	FOOT,

	ANUS,
	BUTT,
	PENIS,
	TESTICLES,
	VAGINA,
	BREASTS,

	EYES,
	MOUTH,
	EARS,
	NOSE,
	TONGUE,
};

struct BodyPartType
{
	BodyPartID ID;
	Noun noun{ "body part", "body parts" };

	double default_rel_size{ 25.0f }; // Percentage size in comparison to the rest of the body

	BodyPartCategory category{ BodyPartCategory::OTHER };
	BodyPartFlag flags{ BodyPartFlag::NONE };

	bool plural = false;

	AllureType base_allure;

	static std::shared_ptr<RawObjectChecker> get_checker();
	static BodyPartType from_json(const json& j);
};

IS_FLAG_ENUM(BodyPartFlag);

inline std::map<std::string, BodyPartCategory> AllBodyPartCategories
{
	{ "OTHER",		BodyPartCategory::OTHER },
	{ "HEAD",		BodyPartCategory::HEAD },
	{ "NECK",		BodyPartCategory::NECK },
	{ "UPPER_BODY", BodyPartCategory::UPPER_BODY },
	{ "LOWER_BODY", BodyPartCategory::LOWER_BODY },
	{ "UPPER_ARM",	BodyPartCategory::UPPER_ARM },
	{ "LOWER_ARM",	BodyPartCategory::LOWER_ARM },
	{ "HAND	",		BodyPartCategory::HAND },
	{ "GROIN",		BodyPartCategory::GROIN },
	{ "UPPER_LEG",	BodyPartCategory::UPPER_LEG },
	{ "LOWER_LEG",	BodyPartCategory::LOWER_LEG },
	{ "FOOT",		BodyPartCategory::FOOT },
	{ "ANUS",		BodyPartCategory::ANUS },
	{ "BUTT",		BodyPartCategory::BUTT },
	{ "PENIS",		BodyPartCategory::PENIS },
	{ "TESTICLES",	BodyPartCategory::TESTICLES },
	{ "VAGINA",		BodyPartCategory::VAGINA },
	{ "BREASTS",	BodyPartCategory::BREASTS },
	{ "EYES",		BodyPartCategory::EYES },
	{ "MOUTH",		BodyPartCategory::MOUTH },
	{ "EARS",		BodyPartCategory::EARS },
	{ "NOSE",		BodyPartCategory::NOSE },
	{ "TONGUE",		BodyPartCategory::TONGUE },
};

inline std::map<std::string, BodyPartFlag> AllBodyPartFlags
{
	{ "GRASP",		BodyPartFlag::GRASP },
};

namespace body_part
{
	inline BodyPartType Head
	{
		.ID = "BP_HEAD",
		.noun = { "head", "heads" },
		.default_rel_size = 8.0,
		.category = BodyPartCategory::HEAD,
		.flags = BodyPartFlag::THOUGHT | BodyPartFlag::PRETTY,
		.base_allure{ .beauty = 2.0f, .beauty_names{ { "face", "faces" } } },
	};

	inline BodyPartType Neck
	{
		.ID = "BP_NECK",
		.noun = { "neck", "necks" },
		.default_rel_size = 2.0,
		.category = BodyPartCategory::NECK,
		.flags = BodyPartFlag::HANDLE | BodyPartFlag::CHOKABLE,
	};

	inline BodyPartType UpperBody
	{
		.ID = "BP_UPPERBODY",
		.noun = { "upper body", "upper bodies" },
		.default_rel_size = 22.0,
		.category = BodyPartCategory::UPPER_BODY,
		.flags = BodyPartFlag::CIRCULATION | BodyPartFlag::SUGGESTIVE | BodyPartFlag::PRETTY,
		.base_allure{
			.suggestiveness = 4.0f, .beauty = 2.0f,
			.suggestive_names{ { "chest", "chests" } },
			.beauty_names{ { "abs", "abs", true } },
		},
	};

	inline BodyPartType LowerBody
	{
		.ID = "BP_LOWERBODY",
		.noun = { "lower body", "lower bodies" },
		.default_rel_size = 16.0,
		.category = BodyPartCategory::LOWER_BODY,
		.flags = BodyPartFlag::DIGESTION | BodyPartFlag::SUGGESTIVE | BodyPartFlag::PRETTY,
		.base_allure{
			.suggestiveness = 2.0f, .beauty = 4.0f,
			.beauty_names{ { "tummy", "tummies" },{ "belly", "bellies" },{ "mid-section", "mid-sections" } },
		},
	};

	inline BodyPartType UpperArm
	{
		.ID = "BP_UPPER_ARM",
		.noun = { "upper arm", "lower arm" },
		.default_rel_size = 6.0,
		.category = BodyPartCategory::UPPER_ARM,
		.flags = BodyPartFlag::LIMB | BodyPartFlag::KINKY,
		.base_allure{
			.kinkiness = 4.0f,
			.suggestive_names{ { "arm pit", "arm pits" } },
		},
	};

	inline BodyPartType LowerArm
	{
		.ID = "BP_LOWER_ARM",
		.noun = { "lower arm", "lower arms" },
		.default_rel_size = 4.0,
		.category = BodyPartCategory::LOWER_ARM,
		.flags = BodyPartFlag::LIMB,
	};

	inline BodyPartType Hand
	{
		.ID = "BP_HAND",
		.noun = { "hand", "hands" },
		.default_rel_size = 1.5,
		.category = BodyPartCategory::HAND,
		.flags = BodyPartFlag::GRASP | BodyPartFlag::KINKY,
		.base_allure{
			.kinkiness = 2.0f,
			.kinky_names{ { "fingers", "fingers", true },{ "palm", "palms" },{ "nails", "nails", true } },
		},
	};

	inline BodyPartType Groin
	{
		.ID = "BP_GROIN",
		.noun = { "groin", "groin" },
		.default_rel_size = 10.0,
		.category = BodyPartCategory::GROIN,
		.flags = BodyPartFlag::LEWD,
		.base_allure{
			.suggestiveness = 4.0f,
			.suggestive_names{ { "crotch", "crotches" } },
		},
	};

	inline BodyPartType UpperLeg
	{
		.ID = "BP_UPPER_LEG",
		.noun = { "upper leg", "upper legs" },
		.default_rel_size = 16.0,
		.category = BodyPartCategory::UPPER_LEG,
		.flags = BodyPartFlag::STANCE | BodyPartFlag::SUGGESTIVE,
		.base_allure{
			.suggestiveness = 5.0f,
			.suggestive_names{ { "thigh", "thighs" } },
		},
	};

	inline BodyPartType LowerLeg
	{
		.ID = "BP_LOWER_LEG",
		.noun = { "lower leg", "lower legs" },
		.default_rel_size = 9.0,
		.category = BodyPartCategory::LOWER_LEG,
		.flags = BodyPartFlag::STANCE,
	};

	inline BodyPartType Foot
	{
		.ID = "BP_FOOT",
		.noun = { "foot", "feet" },
		.default_rel_size = 3.0,
		.category = BodyPartCategory::FOOT,
		.flags = BodyPartFlag::STANCE | BodyPartFlag::KINKY,
		.base_allure{
			.kinkiness = 5.0f,
			.kinky_names{ { "foot", "feet" },{ "toes", "toes", true },{ "sole", "soles" } },
		},
	};

	inline BodyPartType Penis
	{
		.ID = "BP_PENIS",
		.noun = { "penis", "penises" },
		.default_rel_size = AVERAGE_HEIGHT / 6_in,
		.category = BodyPartCategory::PENIS,
		.flags = BodyPartFlag::PHALLUS | BodyPartFlag::LEWD,
		.base_allure{
			.lewdness = 10.0f,
			.kinkiness = 8.0f,
			.lewd_names{ { "cock", "cocks" },{ "dick", "dicks" },{ "penis", "penises" } },
			.kinky_names{ { "goon rod", "goon rods" } },
		},
	};

	inline BodyPartType Vagina
	{
		.ID = "BP_VAGINA",
		.noun = { "vagina", "vaginas" },
		.default_rel_size = AVERAGE_HEIGHT / 6_in,
		.category = BodyPartCategory::VAGINA,
		.flags = BodyPartFlag::ORIFICE | BodyPartFlag::FLUID_PRODUCER | BodyPartFlag::LEWD,
		.base_allure{
			.lewdness = 10.0f,
			.kinkiness = 6.0f,
			.lewd_names{ { "pussy", "pussies" } },
		},
	};

	inline BodyPartType Breasts
	{
		.ID = "BP_BREASTS",
		.noun = { "breasts", "pairs of breasts", true },
		.default_rel_size = AVERAGE_HEIGHT / 4_in,
		.category = BodyPartCategory::BREASTS,
		.flags = BodyPartFlag::FLUID_PRODUCER | BodyPartFlag::CLEAVAGE | BodyPartFlag::LEWD,
		.plural = true,
		.base_allure{
			.lewdness = 8.0f,
			.kinkiness = 10.0f,
			.lewd_names{ { "boobs", "pairs of boobs", true },{ "tits", "pairs of tits", true },{ "tiddies", "pairs of tiddies", true } },
			.kinky_names{ { "milkers", "pairs of milkers", true },{ "melons", "pairs of melons", true },{ "milk jugs", "pairs of milk jugs", true } },
		},
	};

	inline BodyPartType Testicles
	{
		.ID = "BP_TESTICLES",
		.noun = { "testicles", "pairs of testicles", true },
		.default_rel_size = AVERAGE_HEIGHT / 2_in,
		.category = BodyPartCategory::TESTICLES,
		.flags = BodyPartFlag::FLUID_PRODUCER | BodyPartFlag::LEWD,
		.plural = true,
		.base_allure{
			.lewdness = 6.0f,
			.kinkiness = 6.0f,
			.lewd_names{ { "balls", "pairs of balls", true },{ "nuts", "pairs of nuts", true },{ "ballsack", "ballsacks" } },
		},
	};

	inline BodyPartType Butt
	{
		.ID = "BP_BUTT",
		.noun = { "butt", "butts" },
		.default_rel_size = AVERAGE_HEIGHT / 8_in,
		.category = BodyPartCategory::BUTT,
		.flags = BodyPartFlag::CLEAVAGE | BodyPartFlag::SUGGESTIVE | BodyPartFlag::LEWD,
		.base_allure{
			.lewdness = 2.0f,
			.suggestiveness = 12.0f,
			.kinkiness = 10.0f,
			.lewd_names{ { "butt", "butts" },{ "ass", "asses" },{ "booty", "booties" },{ "cheeks", "pairs of cheeks", true },{ "dumpy", "dumpies" } },
			.suggestive_names{ { "rear", "rears" },{ "behind", "behinds" },{ "butt", "butts" } },
			.kinky_names{ { "wobblemeat", "wobblemeats" },{ "dumptruck", "dumptrucks" } },
		},
	};

	inline BodyPartType Anus
	{
		.ID = "BP_ANUS",
		.noun = { "anus", "anuses" },
		.default_rel_size = AVERAGE_HEIGHT / 1_in,
		.category = BodyPartCategory::ANUS,
		.flags = BodyPartFlag::ORIFICE | BodyPartFlag::LEWD,
		.base_allure{
			.lewdness = 18.0f,
			.kinkiness = 20.0f,
			.lewd_names{ { "butthole", "buttholes" },{ "hole", "holes" } },
			.kinky_names{ { "donut", "donuts" },{ "butthole", "buttholes" } },
		},
	};

	inline BodyPartType Eyes
	{
		.ID = "BP_EYES",
		.noun = { "eyes", "pairs of eyes", true },
		.default_rel_size = 0.8,
		.category = BodyPartCategory::EYES,
		.flags = BodyPartFlag::SENSORY,
	};

	inline BodyPartType Mouth
	{
		.ID = "BP_MOUTH",
		.noun = { "mouth", "mouths" },
		.default_rel_size = 1.2,
		.category = BodyPartCategory::MOUTH,
		.flags = BodyPartFlag::SPEAK | BodyPartFlag::EAT,
	};

	inline BodyPartType Ears
	{
		.ID = "BP_EARS",
		.noun = { "ears", "pairs of ears", true },
		.default_rel_size = 1.0,
		.category = BodyPartCategory::EARS,
		.flags = BodyPartFlag::SENSORY,
	};

	inline BodyPartType Nose
	{
		.ID = "BP_NOSE",
		.noun = { "nose", "noses" },
		.default_rel_size = 0.8,
		.category = BodyPartCategory::NOSE,
		.flags = BodyPartFlag::SENSORY,
	};

	inline BodyPartType Tongue
	{
		.ID = "BP_TONGUE",
		.noun = { "tongue", "tongues" },
		.default_rel_size = 0.5,
		.category = BodyPartCategory::TONGUE,
		.flags = BodyPartFlag::SENSORY | BodyPartFlag::SUGGESTIVE | BodyPartFlag::INTERNAL,
	};

	inline BodyPartType Tail
	{
		.ID = "BP_TAIL",
		.noun = { "tail", "tails" },
		.default_rel_size = AVERAGE_HEIGHT / 6_in,
		.category = BodyPartCategory::OTHER,
		.flags = BodyPartFlag::HANDLE | BodyPartFlag::PRETTY,
	};

	inline BodyPartType Horns
	{
		.ID = "BP_HORNS",
		.noun = { "horns", "pairs of horns", true },
		.default_rel_size = AVERAGE_HEIGHT / 4.5_in,
		.category = BodyPartCategory::OTHER,
		.flags = BodyPartFlag::HANDLE,
		.plural = true,
	};
}

#endif // !LEMONADE_GAME_SRC_BODY_PART_TYPE_H