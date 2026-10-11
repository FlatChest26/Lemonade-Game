#pragma once

#ifndef LEMONADE_GAME_SRC_BODY_PART_TYPE_H
#define LEMONADE_GAME_SRC_BODY_PART_TYPE_H

#include "snowy_units.h"
#include "snowy_grammar.h"
#include "enum_traits.h"
#include "snowy_database.h"

#include "body_part_components.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace units::literals;

constexpr auto AVERAGE_HEIGHT = 5_ft + 6_in;
constexpr auto AVERAGE_WEIGHT = 150_lb;

using BodyPartID = std::string;

struct BodyPartType
{
	enum class Category
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

	enum class Flag : uint64_t
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

	BodyPartID ID{};
	Noun noun{ "body part", "body parts" };

	double default_rel_size{ 25.0f }; // Percentage size in comparison to the rest of the body

	BodyPartType::Category category{ BodyPartType::Category::OTHER };
	BodyPartType::Flag flags{ BodyPartType::Flag::NONE };

	bool plural = false;

	AllureType base_allure;

	static BodyPartType from_json(const json& j);
};

IS_FLAG_ENUM(BodyPartType::Flag);

inline std::map<std::string, BodyPartType::Category> AllBodyPartCategories
{
	{ "OTHER",		BodyPartType::Category::OTHER		},
	{ "HEAD",		BodyPartType::Category::HEAD		},
	{ "NECK",		BodyPartType::Category::NECK		},
	{ "UPPER_BODY", BodyPartType::Category::UPPER_BODY	},
	{ "LOWER_BODY", BodyPartType::Category::LOWER_BODY	},
	{ "UPPER_ARM",	BodyPartType::Category::UPPER_ARM	},
	{ "LOWER_ARM",	BodyPartType::Category::LOWER_ARM	},
	{ "HAND	",		BodyPartType::Category::HAND		},
	{ "GROIN",		BodyPartType::Category::GROIN		},
	{ "UPPER_LEG",	BodyPartType::Category::UPPER_LEG	},
	{ "LOWER_LEG",	BodyPartType::Category::LOWER_LEG	},
	{ "FOOT",		BodyPartType::Category::FOOT		},
	{ "ANUS",		BodyPartType::Category::ANUS		},
	{ "BUTT",		BodyPartType::Category::BUTT		},
	{ "PENIS",		BodyPartType::Category::PENIS		},
	{ "TESTICLES",	BodyPartType::Category::TESTICLES	},
	{ "VAGINA",		BodyPartType::Category::VAGINA		},
	{ "BREASTS",	BodyPartType::Category::BREASTS		},
	{ "EYES",		BodyPartType::Category::EYES		},
	{ "MOUTH",		BodyPartType::Category::MOUTH		},
	{ "EARS",		BodyPartType::Category::EARS		},
	{ "NOSE",		BodyPartType::Category::NOSE		},
	{ "TONGUE",		BodyPartType::Category::TONGUE		},
};

namespace body_part
{
	using BodyPartTypeDB = snowy::SingletonDatabase<BodyPartType>;

	inline BodyPartType Head
	{
		.ID = "BP_HEAD",
		.noun = {"head", "heads"},
		.default_rel_size = 8.0,
		.category = BodyPartType::Category::HEAD,
		.flags = BodyPartType::Flag::THOUGHT | BodyPartType::Flag::PRETTY,
		.base_allure {.beauty = 2.0f, .beauty_names { {"face", "faces"} } },
	};

	inline BodyPartType Neck
	{
		.ID = "BP_NECK",
		.noun = {"neck", "necks"},
		.default_rel_size = 2.0,
		.category = BodyPartType::Category::NECK,
		.flags = BodyPartType::Flag::HANDLE | BodyPartType::Flag::CHOKABLE,
	};

	inline BodyPartType UpperBody
	{
		.ID = "BP_UPPERBODY",
		.noun = {"upper body", "upper bodies"},
		.default_rel_size = 22.0,
		.category = BodyPartType::Category::UPPER_BODY,
		.flags = BodyPartType::Flag::CIRCULATION | BodyPartType::Flag::SUGGESTIVE | BodyPartType::Flag::PRETTY,
		.base_allure {
			.suggestiveness = 4.0f, .beauty = 2.0f,
			.suggestive_names { {"chest", "chests"} },
			.beauty_names { {"abs", "abs", true} },
		},
	};

	inline BodyPartType LowerBody
	{
		.ID = "BP_LOWERBODY",
		.noun = {"lower body", "lower bodies"},
		.default_rel_size = 16.0,
		.category = BodyPartType::Category::LOWER_BODY,
		.flags = BodyPartType::Flag::DIGESTION | BodyPartType::Flag::SUGGESTIVE | BodyPartType::Flag::PRETTY,
		.base_allure {
			.suggestiveness = 2.0f, .beauty = 4.0f,
			.beauty_names { {"tummy", "tummies"}, {"belly", "bellies"}, {"mid-section", "mid-sections"} },
		},
	};

	inline BodyPartType UpperArm
	{
		.ID = "BP_UPPER_ARM",
		.noun = {"upper arm", "lower arm"},
		.default_rel_size = 6.0,
		.category = BodyPartType::Category::UPPER_ARM,
		.flags = BodyPartType::Flag::LIMB | BodyPartType::Flag::KINKY,
		.base_allure {
			.kinkiness = 4.0f,
			.suggestive_names { {"arm pit", "arm pits"}  },
		},
	};

	inline BodyPartType LowerArm
	{
		.ID = "BP_LOWER_ARM",
		.noun = {"lower arm", "lower arms"},
		.default_rel_size = 4.0,
		.category = BodyPartType::Category::LOWER_ARM,
		.flags = BodyPartType::Flag::LIMB,
	};

	inline BodyPartType Hand
	{
		.ID = "BP_HAND",
		.noun = {"hand", "hands"},
		.default_rel_size = 1.5,
		.category = BodyPartType::Category::HAND,
		.flags = BodyPartType::Flag::GRASP | BodyPartType::Flag::KINKY,
		.base_allure {
			.kinkiness = 2.0f,
			.kinky_names { {"fingers", "fingers", true}, {"palm", "palms"}, {"nails", "nails", true} },
		},
	};

	inline BodyPartType Groin
	{
		.ID = "BP_GROIN",
		.noun = {"groin", "groin"},
		.default_rel_size = 10.0,
		.category = BodyPartType::Category::GROIN,
		.flags = BodyPartType::Flag::LEWD,
		.base_allure {
			.suggestiveness = 4.0f,
			.suggestive_names { {"crotch", "crotches"} },
		},
	};

	inline BodyPartType UpperLeg
	{
		.ID = "BP_UPPER_LEG",
		.noun = {"upper leg", "upper legs"},
		.default_rel_size = 16.0,
		.category = BodyPartType::Category::UPPER_LEG,
		.flags = BodyPartType::Flag::STANCE | BodyPartType::Flag::SUGGESTIVE,
		.base_allure {
			.suggestiveness = 5.0f,
			.suggestive_names { {"thigh", "thighs"} },
		},
	};

	inline BodyPartType LowerLeg
	{
		.ID = "BP_LOWER_LEG",
		.noun = {"lower leg", "lower legs"},
		.default_rel_size = 9.0,
		.category = BodyPartType::Category::LOWER_LEG,
		.flags = BodyPartType::Flag::STANCE,
	};

	inline BodyPartType Foot
	{
		.ID = "BP_FOOT",
		.noun = {"foot", "feet"},
		.default_rel_size = 3.0,
		.category = BodyPartType::Category::FOOT,
		.flags = BodyPartType::Flag::STANCE | BodyPartType::Flag::KINKY,
		.base_allure {
			.kinkiness = 5.0f,
			.kinky_names { {"foot", "feet"}, {"toes", "toes", true}, {"sole", "soles"} },
		},
	};

	inline BodyPartType Penis
	{
		.ID = "BP_PENIS",
		.noun = {"penis", "penises"},
		.default_rel_size = AVERAGE_HEIGHT / 6_in,
		.category = BodyPartType::Category::PENIS,
		.flags = BodyPartType::Flag::PHALLUS | BodyPartType::Flag::LEWD,
		.base_allure {
			.lewdness = 10.0f,
			.kinkiness = 8.0f,
			.lewd_names { {"cock", "cocks"}, {"dick", "dicks"}, {"penis", "penises"} },
			.kinky_names { {"goon rod", "goon rods"} },
		},
	};

	inline BodyPartType Vagina
	{
		.ID = "BP_VAGINA",
		.noun = {"vagina", "vaginas"},
		.default_rel_size = AVERAGE_HEIGHT / 6_in,
		.category = BodyPartType::Category::VAGINA,
		.flags = BodyPartType::Flag::ORIFICE | BodyPartType::Flag::FLUID_PRODUCER | BodyPartType::Flag::LEWD,
		.base_allure {
			.lewdness = 10.0f,
			.kinkiness = 6.0f,
			.lewd_names { {"pussy", "pussies"} },
		},
	};

	inline BodyPartType Breasts
	{
		.ID = "BP_BREASTS",
		.noun = {"breasts", "pairs of breasts", true},
		.default_rel_size = AVERAGE_HEIGHT / 4_in,
		.category = BodyPartType::Category::BREASTS,
		.flags = BodyPartType::Flag::FLUID_PRODUCER | BodyPartType::Flag::CLEAVAGE | BodyPartType::Flag::LEWD,
		.plural = true,
		.base_allure {
			.lewdness = 8.0f,
			.kinkiness = 10.0f,
			.lewd_names { {"boobs", "pairs of boobs", true}, {"tits", "pairs of tits", true}, {"tiddies", "pairs of tiddies", true} },
			.kinky_names { {"milkers", "pairs of milkers", true}, {"melons", "pairs of melons", true}, {"milk jugs", "pairs of milk jugs", true} },
		},
	};

	inline BodyPartType Testicles
	{
		.ID = "BP_TESTICLES",
		.noun = {"testicles", "pairs of testicles", true},
		.default_rel_size = AVERAGE_HEIGHT / 2_in,
		.category = BodyPartType::Category::TESTICLES,
		.flags = BodyPartType::Flag::FLUID_PRODUCER | BodyPartType::Flag::LEWD,
		.plural = true,
		.base_allure {
			.lewdness = 6.0f,
			.kinkiness = 6.0f,
			.lewd_names { {"balls", "pairs of balls", true}, {"nuts", "pairs of nuts", true}, {"ballsack", "ballsacks" } },
		},
	};

	inline BodyPartType Butt
	{
		.ID = "BP_BUTT",
		.noun = {"butt", "butts"},
		.default_rel_size = AVERAGE_HEIGHT / 8_in,
		.category = BodyPartType::Category::BUTT,
		.flags = BodyPartType::Flag::CLEAVAGE | BodyPartType::Flag::SUGGESTIVE | BodyPartType::Flag::LEWD,
		.base_allure {
			.lewdness = 2.0f,
			.suggestiveness = 12.0f,
			.kinkiness = 10.0f,
			.lewd_names { {"butt", "butts"}, {"ass", "asses"}, {"booty", "booties"}, {"cheeks", "pairs of cheeks", true }, {"dumpy", "dumpies"} },
			.suggestive_names { {"rear", "rears"}, {"behind", "behinds"}, {"butt", "butts"} },
			.kinky_names { {"wobblemeat", "wobblemeats"}, {"dumptruck", "dumptrucks"} },
		},
	};

	inline BodyPartType Anus
	{
		.ID = "BP_ANUS",
		.noun = {"anus", "anuses"},
		.default_rel_size = AVERAGE_HEIGHT / 1_in,
		.category = BodyPartType::Category::ANUS,
		.flags = BodyPartType::Flag::ORIFICE | BodyPartType::Flag::LEWD,
		.base_allure {
			.lewdness = 18.0f,
			.kinkiness = 20.0f,
			.lewd_names { {"butthole", "buttholes"}, {"hole", "holes"} },
			.kinky_names { {"donut", "donuts"}, {"butthole", "buttholes"} },
		},
	};

	inline BodyPartType Eyes
	{
		.ID = "BP_EYES",
		.noun = {"eyes", "pairs of eyes", true},
		.default_rel_size = 0.8,
		.category = BodyPartType::Category::EYES,
		.flags = BodyPartType::Flag::SENSORY,
	};

	inline BodyPartType Mouth
	{
		.ID = "BP_MOUTH",
		.noun = {"mouth", "mouths"},
		.default_rel_size = 1.2,
		.category = BodyPartType::Category::MOUTH,
		.flags = BodyPartType::Flag::SPEAK | BodyPartType::Flag::EAT,
	};

	inline BodyPartType Ears
	{
		.ID = "BP_EARS",
		.noun = {"ears", "pairs of ears", true},
		.default_rel_size = 1.0,
		.category = BodyPartType::Category::EARS,
		.flags = BodyPartType::Flag::SENSORY,
	};

	inline BodyPartType Nose
	{
		.ID = "BP_NOSE",
		.noun = {"nose", "noses"},
		.default_rel_size = 0.8,
		.category = BodyPartType::Category::NOSE,
		.flags = BodyPartType::Flag::SENSORY,
	};

	inline BodyPartType Tongue
	{
		.ID = "BP_TONGUE",
		.noun = {"tongue", "tongues"},
		.default_rel_size = 0.5,
		.category = BodyPartType::Category::TONGUE,
		.flags = BodyPartType::Flag::SENSORY | BodyPartType::Flag::SUGGESTIVE | BodyPartType::Flag::INTERNAL,
	};

	inline BodyPartType Tail
	{
		.ID = "BP_TAIL",
		.noun = {"tail", "tails"},
		.default_rel_size = AVERAGE_HEIGHT / 6_in,
		.category = BodyPartType::Category::OTHER,
		.flags = BodyPartType::Flag::HANDLE | BodyPartType::Flag::PRETTY,
	};

	inline BodyPartType Horns
	{
		.ID = "BP_HORNS",
		.noun = {"horns", "pairs of horns", true},
		.default_rel_size = AVERAGE_HEIGHT / 4.5_in,
		.category = BodyPartType::Category::OTHER,
		.flags = BodyPartType::Flag::HANDLE,
		.plural = true,
	};

	inline snowy::Database<BodyPartType*> _body_parts_db
	{
		{ Head.ID,					&Head		},
		{ Neck.ID,					&Neck		},
		{ UpperBody.ID,				&UpperBody	},
		{ LowerBody.ID,				&LowerBody	},
		{ UpperArm.ID,				&UpperArm	},
		{ LowerArm.ID,				&LowerArm	},
		{ Hand.ID,					&Hand		},
		{ Groin.ID,					&Groin		},
		{ UpperLeg.ID,				&UpperLeg	},
		{ LowerLeg.ID,				&LowerLeg	},
		{ Foot.ID,					&Foot		},
		{ Vagina.ID,				&Vagina		},
		{ Breasts.ID,				&Breasts	},
		{ Penis.ID,					&Penis		},
		{ Testicles.ID,				&Testicles	},
		{ Butt.ID,					&Butt		},
		{ Anus.ID,					&Anus		},
		{ Eyes.ID,					&Eyes		},
		{ Mouth.ID,					&Mouth		},
		{ Ears.ID,					&Ears		},
		{ Nose.ID,					&Nose		},
		{ Tongue.ID,				&Tongue		},
		{ Tail.ID,					&Tail		},
		{ Horns.ID,					&Horns		},
	};

	static inline void initialize_db()
	{
		BodyPartTypeDB::init();
	}
}

#endif // !LEMONADE_GAME_SRC_BODY_PART_TYPE_H