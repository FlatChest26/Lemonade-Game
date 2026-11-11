#pragma once

#ifndef LEMONADE_GAME_SRC_BODY_PLAN_H
#define LEMONADE_GAME_SRC_BODY_PLAN_H

#include <string>
#include <vector>

#include "body_part.h"
#include "body_part_type.h"
#include "snowy_database.h"
#include "snowy_units.h"
#include "thing.h"

#include "posture.h"

#include "raw_master.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct BodyPartPlan
{
	RawMaster::RawID type;
	BodyPartID ID{ type };
	double rel_size{ 1.0f };
	BodyPartPosition position{ BodyPartPosition::CENTER };

	std::string prefix{ "" };
	bool is_root = false;

	static std::shared_ptr<RawObjectChecker> get_checker();
	static BodyPartPlan from_json(const json& j);
};

struct BodyConnectionPlan
{
	ThingID parent;
	ThingID child;
	BodyPartRelativePosition relative_position{ BodyPartRelativePosition::AROUND };

	static std::shared_ptr<RawObjectChecker> get_checker();
	static BodyConnectionPlan from_json(const json& j);
};

struct BodyPlan
{
	snowy::StringID ID;

	units::Length height;
	units::Mass weight;

	std::vector<BodyPartPlan> body_parts;
	std::vector<BodyConnectionPlan> body_connections;
	std::vector<BodyConnectionPlan> body_relations;

	PostureMap postures{ postures::DEFAULT_POSTURES };

	static std::shared_ptr<RawObjectChecker> get_checker();
	static BodyPlan from_json(const json& j);
};

namespace body_plan
{
	inline BodyPlan GenericBody
	{
		.ID = "GenericBody",
		.height = AVERAGE_HEIGHT,
		.weight = AVERAGE_WEIGHT,
		.body_parts
		{
			{ .type = "upper_body",		.ID = "BP_UPPERBODY",		.position = BodyPartPosition::CENTER, .is_root = true },
			{ .type = "lower_body",		.ID = "BP_LOWERBODY",		.position = BodyPartPosition::CENTER },

			{ .type = "head",			.ID = "BP_HEAD",			.position = BodyPartPosition::TOP },
			{ .type = "neck",			.ID = "BP_NECK",			.position = BodyPartPosition::CENTER },

			{ .type = "upper_arm",		.ID = "BP_LEFT_UPPER_ARM",	.position = BodyPartPosition::LEFT, .prefix = "left " },
			{ .type = "lower_arm",		.ID = "BP_LEFT_LOWER_ARM",	.position = BodyPartPosition::LEFT, .prefix = "left " },
			{ .type = "hand",			.ID = "BP_LEFT_HAND",		.position = BodyPartPosition::LEFT, .prefix = "left " },

			{ .type = "groin",			.ID = "BP_GROIN",			.position = BodyPartPosition::CENTER },

			{ .type = "upper_arm",		.ID = "BP_RIGHT_UPPER_ARM",	.position = BodyPartPosition::RIGHT, .prefix = "right " },
			{ .type = "lower_arm",		.ID = "BP_RIGHT_LOWER_ARM",	.position = BodyPartPosition::RIGHT, .prefix = "right " },
			{ .type = "hand",			.ID = "BP_RIGHT_HAND",		.position = BodyPartPosition::RIGHT, .prefix = "right " },

			{ .type = "upper_leg",		.ID = "BP_LEFT_UPPER_LEG",	.position = BodyPartPosition::LEFT, .prefix = "left " },
			{ .type = "lower_leg",		.ID = "BP_LEFT_LOWER_LEG",	.position = BodyPartPosition::LEFT, .prefix = "left " },
			{ .type = "foot",			.ID = "BP_LEFT_FOOT",		.position = BodyPartPosition::LEFT, .prefix = "left " },

			{ .type = "upper_leg",		.ID = "BP_RIGHT_UPPER_LEG",	.position = BodyPartPosition::RIGHT, .prefix = "right " },
			{ .type = "lower_leg",		.ID = "BP_RIGHT_LOWER_LEG",	.position = BodyPartPosition::RIGHT, .prefix = "right " },
			{ .type = "foot",			.ID = "BP_RIGHT_FOOT",		.position = BodyPartPosition::RIGHT, .prefix = "right " },

			{ .type = "butt",			.ID = "BP_BUTT", .position = BodyPartPosition::CENTER },
			{ .type = "anus",			.ID = "BP_ANUS", .position = BodyPartPosition::CENTER },
		},

		.body_connections
		{
			{ "BP_NECK",			"BP_HEAD",				BodyPartRelativePosition::ABOVE },
			{ "BP_UPPERBODY",		"BP_NECK",				BodyPartRelativePosition::ABOVE },
			{ "BP_UPPERBODY",		"BP_LOWERBODY",			BodyPartRelativePosition::BELOW },
			{ "BP_UPPERBODY",		"BP_LEFT_UPPER_ARM",	BodyPartRelativePosition::LEFT },
			{ "BP_LEFT_UPPER_ARM",	"BP_LEFT_LOWER_ARM",	BodyPartRelativePosition::BELOW },
			{ "BP_LEFT_LOWER_ARM",	"BP_LEFT_HAND",			BodyPartRelativePosition::BELOW },
			{ "BP_UPPERBODY",		"BP_RIGHT_UPPER_ARM",	BodyPartRelativePosition::RIGHT },
			{ "BP_RIGHT_UPPER_ARM", "BP_RIGHT_LOWER_ARM",	BodyPartRelativePosition::BELOW },
			{ "BP_RIGHT_LOWER_ARM", "BP_RIGHT_HAND",		BodyPartRelativePosition::BELOW },
			{ "BP_LOWERBODY",		"BP_GROIN",				BodyPartRelativePosition::BELOW },
			{ "BP_GROIN",			"BP_LEFT_UPPER_LEG",	BodyPartRelativePosition::BELOW },
			{ "BP_LEFT_UPPER_LEG",	"BP_LEFT_LOWER_LEG",	BodyPartRelativePosition::BELOW },
			{ "BP_LEFT_LOWER_LEG",	"BP_LEFT_FOOT",			BodyPartRelativePosition::BELOW },
			{ "BP_GROIN",			"BP_RIGHT_UPPER_LEG",	BodyPartRelativePosition::BELOW },
			{ "BP_RIGHT_UPPER_LEG", "BP_RIGHT_LOWER_LEG",	BodyPartRelativePosition::BELOW },
			{ "BP_RIGHT_LOWER_LEG", "BP_RIGHT_FOOT",		BodyPartRelativePosition::BELOW },
			{ "BP_GROIN",			"BP_BUTT",				BodyPartRelativePosition::BEHIND },
			{ "BP_BUTT",			"BP_ANUS",				BodyPartRelativePosition::INSIDE },
		}
	};
};


struct BodyGloss
{
	RawMaster::RawID ID;
	std::vector<NounReplacement> replacements;

	bool operator==(const BodyGloss& other) const
	{
		return ID == other.ID;
	}

	bool operator!=(const BodyGloss& other) const
	{
		return ID != other.ID;
	}

	static std::shared_ptr<RawObjectChecker> get_checker();
	static BodyGloss from_json(const json& j);
};


struct BodyMod
{
	snowy::StringID ID;

	double modify_size{ 1.0 };

	units::Length set_height{ -1 };
	units::Length add_height{ 0 };
	double multiply_height{ 1.0 };

	units::Mass set_weight{ -1 };
	units::Mass add_weight{ 0 };
	double multiply_weight{ 1.0 };

	std::vector<BodyPartPlan> add_parts;
	std::vector<BodyPartID> remove_parts;
	std::map<ThingID, BodyPartPlan> replace_parts;

	std::vector<BodyConnectionPlan> add_connections;
	std::vector<BodyConnectionPlan> add_relations;

	std::vector<RawMaster::Ref<BodyGloss>> add_body_gloss;
	std::vector<RawMaster::RawID> remove_body_gloss;

	static std::shared_ptr<RawObjectChecker> get_checker();
	static BodyMod from_json(const json& j);
};

namespace body_mod
{
	inline BodyMod DemonParts
	{
		.ID = "DemonParts",

		.add_parts
		{
			{ .type = "horns",	.ID = "BP_HORNS",	.position = BodyPartPosition::TOP },
			{ .type = "tail",	.ID = "BP_TAIL",	.position = BodyPartPosition::BACK },
		},

		.add_connections
		{
			{ "BP_HEAD",		"BP_HORNS",		BodyPartRelativePosition::ABOVE },
			{ "BP_LOWERBODY",	"BP_TAIL",		BodyPartRelativePosition::BEHIND },
		},
	};

	inline BodyMod FemaleParts
	{
		.ID = "FemaleParts",

		.add_parts
		{
			{ .type = "breasts",	.ID = "BP_BREASTS",		.position = BodyPartPosition::FRONT },
			{ .type = "vagina",		.ID = "BP_VAGINA",		.position = BodyPartPosition::BOTTOM },
		},

		.add_connections
		{
			{ "BP_UPPERBODY", "BP_BREASTS", BodyPartRelativePosition::IN_FRONT },
			{ "BP_LOWERBODY", "BP_VAGINA",	BodyPartRelativePosition::BELOW },
		},
	};

	inline BodyMod MaleParts
	{
		.ID = "MaleParts",

		.add_parts
		{
			{ .type = "penis",			.ID = "BP_PENIS",		.position = BodyPartPosition::FRONT },
			{ .type = "testicles",		.ID = "BP_TESTICLES",	.position = BodyPartPosition::BOTTOM },
		},

		.add_connections
		{
			{ "BP_UPPERBODY", "BP_PENIS",		BodyPartRelativePosition::IN_FRONT },
			{ "BP_LOWERBODY", "BP_TESTICLES",	BodyPartRelativePosition::IN_FRONT },
		},
	};

	inline BodyMod GirlCock
	{
		.ID = "GirlCock",

		.add_parts
		{
			{ .type = "breasts",		.ID = "BP_BREASTS",		.position = BodyPartPosition::FRONT },
			{ .type = "penis",		.ID = "BP_PENIS",		.position = BodyPartPosition::FRONT },
			{ .type = "testicles",	.ID = "BP_TESTICLES",	.position = BodyPartPosition::BOTTOM },
		},

		.add_connections
		{
			{ "BP_UPPERBODY", "BP_BREASTS",		BodyPartRelativePosition::IN_FRONT },
			{ "BP_UPPERBODY", "BP_PENIS",		BodyPartRelativePosition::IN_FRONT },
			{ "BP_LOWERBODY", "BP_TESTICLES",	BodyPartRelativePosition::IN_FRONT },
		},
	};

	inline BodyMod BoyPussy
	{
		.ID = "BoyPussy",

		.add_parts
		{
			{ .type = "vagina",		.ID = "BP_VAGINA",		.position = BodyPartPosition::BOTTOM },
		},

		.add_connections
		{
			{ "BP_LOWERBODY", "BP_VAGINA",	BodyPartRelativePosition::BELOW },
		},
	};

	inline BodyMod HeadDetails
	{
		.ID = "HeadDetails",

		.add_parts
		{
			{ .type = "eyes",		.ID = "BP_EYES",	.position = BodyPartPosition::FRONT },
			{ .type = "mouth",		.ID = "BP_MOUTH",	.position = BodyPartPosition::FRONT },
			{ .type = "ears",		.ID = "BP_EARS",	.position = BodyPartPosition::SIDES },
			{ .type = "nose",		.ID = "BP_NOSE",	.position = BodyPartPosition::FRONT },
			{ .type = "tongue",		.ID = "BP_TONGUE",	.position = BodyPartPosition::INSIDE },
		},

		.add_connections
		{
			{ "BP_HEAD", "BP_EYES",		BodyPartRelativePosition::IN_FRONT },
			{ "BP_HEAD", "BP_MOUTH",	BodyPartRelativePosition::IN_FRONT },
			{ "BP_HEAD", "BP_EARS",		BodyPartRelativePosition::SIDES },
			{ "BP_HEAD", "BP_NOSE",		BodyPartRelativePosition::IN_FRONT },
			{ "BP_MOUTH", "BP_TONGUE",	BodyPartRelativePosition::INSIDE },
		},

		.add_relations
		{
			{ "BP_EYES", "BP_MOUTH",	BodyPartRelativePosition::BELOW },
			{ "BP_EYES", "BP_NOSE",		BodyPartRelativePosition::BETWEEN },
		}
	};
}



#endif // !LEMONADE_GAME_SRC_BODY_PLAN_H