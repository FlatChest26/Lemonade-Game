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

#include "raw_master.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;


struct BodyPartPlan
{
	RawMaster::RawID type;
	ThingID ID{ type };
	double rel_size{ 1.0f };
	BodyPartPosition position{ BodyPartPosition::CENTER };

	std::string prefix{ "" };

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
			{.type = "BP_HEAD",			.ID = "BP_HEAD",			.position = BodyPartPosition::TOP},
			{.type = "BP_NECK",			.ID = "BP_NECK",			.position = BodyPartPosition::CENTER},

			{.type = "BP_UPPERBODY",	.ID = "BP_UPPERBODY",		.position = BodyPartPosition::CENTER},
			{.type = "BP_LOWERBODY",	.ID = "BP_LOWERBODY",		.position = BodyPartPosition::CENTER},

			{.type = "BP_UPPER_ARM",	.ID = "BP_LEFT_UPPER_ARM",	.position = BodyPartPosition::LEFT, .prefix = "left "},
			{.type = "BP_LOWER_ARM",	.ID = "BP_LEFT_LOWER_ARM",	.position = BodyPartPosition::LEFT, .prefix = "left "},
			{.type = "BP_HAND",			.ID = "BP_LEFT_HAND",		.position = BodyPartPosition::LEFT, .prefix = "left "},

			{.type = "BP_GROIN",		.ID = "BP_GROIN",			.position = BodyPartPosition::CENTER},

			{.type = "BP_UPPER_ARM",	.ID = "BP_RIGHT_UPPER_ARM",	.position = BodyPartPosition::RIGHT, .prefix = "right "},
			{.type = "BP_LOWER_ARM",	.ID = "BP_RIGHT_LOWER_ARM",	.position = BodyPartPosition::RIGHT, .prefix = "right "},
			{.type = "BP_HAND",			.ID = "BP_RIGHT_HAND",		.position = BodyPartPosition::RIGHT, .prefix = "right "},

			{.type = "BP_UPPER_LEG",	.ID = "BP_LEFT_UPPER_LEG",	.position = BodyPartPosition::LEFT, .prefix = "left "},
			{.type = "BP_LOWER_LEG",	.ID = "BP_LEFT_LOWER_LEG",	.position = BodyPartPosition::LEFT, .prefix = "left "},
			{.type = "BP_FOOT",			.ID = "BP_LEFT_FOOT",		.position = BodyPartPosition::LEFT, .prefix = "left "},

			{.type = "BP_UPPER_LEG",	.ID = "BP_RIGHT_UPPER_LEG",	.position = BodyPartPosition::RIGHT, .prefix = "right "},
			{.type = "BP_LOWER_LEG",	.ID = "BP_RIGHT_LOWER_LEG",	.position = BodyPartPosition::RIGHT, .prefix = "right "},
			{.type = "BP_FOOT",			.ID = "BP_RIGHT_FOOT",		.position = BodyPartPosition::RIGHT, .prefix = "right "},

			{.type = "BP_BUTT", .ID = "BP_BUTT", .position = BodyPartPosition::CENTER},
			{.type = "BP_ANUS", .ID = "BP_ANUS", .position = BodyPartPosition::CENTER},
		},

		.body_connections
		{
			{ "BP_NECK",			"BP_HEAD",				BodyPartRelativePosition::ABOVE	},
			{ "BP_UPPERBODY",		"BP_NECK",				BodyPartRelativePosition::ABOVE	},
			{ "BP_UPPERBODY",		"BP_LOWERBODY",			BodyPartRelativePosition::BELOW	},
			{ "BP_UPPERBODY",		"BP_LEFT_UPPER_ARM",	BodyPartRelativePosition::LEFT	},
			{ "BP_LEFT_UPPER_ARM",	"BP_LEFT_LOWER_ARM",	BodyPartRelativePosition::BELOW	},
			{ "BP_LEFT_LOWER_ARM",	"BP_LEFT_HAND",			BodyPartRelativePosition::BELOW	},
			{ "BP_UPPERBODY",		"BP_RIGHT_UPPER_ARM",	BodyPartRelativePosition::RIGHT	},
			{ "BP_RIGHT_UPPER_ARM", "BP_RIGHT_LOWER_ARM",	BodyPartRelativePosition::BELOW	},
			{ "BP_RIGHT_LOWER_ARM", "BP_RIGHT_HAND",		BodyPartRelativePosition::BELOW	},
			{ "BP_LOWERBODY",		"BP_GROIN",				BodyPartRelativePosition::BELOW	},
			{ "BP_GROIN",			"BP_LEFT_UPPER_LEG",	BodyPartRelativePosition::BELOW	},
			{ "BP_LEFT_UPPER_LEG",	"BP_LEFT_LOWER_LEG",	BodyPartRelativePosition::BELOW	},
			{ "BP_LEFT_LOWER_LEG",	"BP_LEFT_FOOT",			BodyPartRelativePosition::BELOW	},
			{ "BP_GROIN",			"BP_RIGHT_UPPER_LEG",	BodyPartRelativePosition::BELOW	},
			{ "BP_RIGHT_UPPER_LEG", "BP_RIGHT_LOWER_LEG",	BodyPartRelativePosition::BELOW	},
			{ "BP_RIGHT_LOWER_LEG", "BP_RIGHT_FOOT",		BodyPartRelativePosition::BELOW	},
			{ "BP_GROIN",			"BP_BUTT",				BodyPartRelativePosition::BEHIND	},
			{ "BP_BUTT",			"BP_ANUS",				BodyPartRelativePosition::INSIDE	},
}
	};
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
			{.type = "BP_HORNS",	.ID = "BP_HORNS",	.position = BodyPartPosition::TOP},
			{.type = "BP_TAIL",		.ID = "BP_TAIL",	.position = BodyPartPosition::BACK},
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
			{.type = "BP_BREASTS",	.ID = "BP_BREASTS",		.position = BodyPartPosition::FRONT},
			{.type = "BP_VAGINA",	.ID = "BP_VAGINA",		.position = BodyPartPosition::BOTTOM},
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
			{.type = "BP_PENIS",			.ID = "BP_PENIS",		.position = BodyPartPosition::FRONT},
			{.type = "BP_TESTICLES",		.ID = "BP_TESTICLES",	.position = BodyPartPosition::BOTTOM},
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
			{.type = "BP_BREASTS",		.ID = "BP_BREASTS",		.position = BodyPartPosition::FRONT},
			{.type = "BP_PENIS",		.ID = "BP_PENIS",		.position = BodyPartPosition::FRONT},
			{.type = "BP_TESTICLES",	.ID = "BP_TESTICLES",	.position = BodyPartPosition::BOTTOM},
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
			{.type = "BP_VAGINA",	.ID = "BP_VAGINA",		.position = BodyPartPosition::BOTTOM},
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
			{.type = "BP_EYES",		.ID = "BP_EYES",	.position = BodyPartPosition::FRONT},
			{.type = "BP_MOUTH",	.ID = "BP_MOUTH",	.position = BodyPartPosition::FRONT},
			{.type = "BP_EARS",		.ID = "BP_EARS",	.position = BodyPartPosition::SIDES },
			{.type = "BP_NOSE",		.ID = "BP_NOSE",	.position = BodyPartPosition::FRONT},
			{.type = "BP_TONGUE",	.ID = "BP_TONGUE",	.position = BodyPartPosition::INSIDE},
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