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

struct BodyPartPlan
{
	BodyPartType* type;
	ThingID ID{ type->ID };
	double rel_size{ type->default_rel_size };
	BodyPart::Position position{ BodyPart::Position::CENTER };

	std::string prefix{ "" };
};

struct BodyConnectionPlan
{
	ThingID parent;
	ThingID child;
	BodyPart::RelativePosition relative_position{ BodyPart::RelativePosition::AROUND };
};

struct BodyPlan
{
	snowy::StringID ID;

	units::Length height;
	units::Mass weight;

	std::vector<BodyPartPlan> body_parts;
	std::vector<BodyConnectionPlan> body_connections;
	std::vector<BodyConnectionPlan> body_relations;
};

namespace body_plan
{
	using BodyPlanDB = snowy::SingletonDatabase<BodyPlan>;

	inline BodyPlan GenericBody
	{
		.ID = "GenericBody",
		.height = AVERAGE_HEIGHT,
		.weight = AVERAGE_WEIGHT,
		.body_parts
		{
			{.type = &body_part::Head,		.ID = "BP_HEAD",			.position = BodyPart::Position::TOP },
			{.type = &body_part::Neck,		.ID = "BP_NECK",			.position = BodyPart::Position::CENTER },

			{.type = &body_part::UpperBody,	.ID = "BP_UPPERBODY",		.position = BodyPart::Position::CENTER },
			{.type = &body_part::LowerBody,	.ID = "BP_LOWERBODY",		.position = BodyPart::Position::CENTER },

			{.type = &body_part::UpperArm,	.ID = "BP_LEFT_UPPER_ARM",	.position = BodyPart::Position::LEFT, .prefix = "left "},
			{.type = &body_part::LowerArm,	.ID = "BP_LEFT_LOWER_ARM",	.position = BodyPart::Position::LEFT, .prefix = "left "},
			{.type = &body_part::Hand,		.ID = "BP_LEFT_HAND",		.position = BodyPart::Position::LEFT, .prefix = "left "},

			{.type = &body_part::Groin,		.ID = "BP_GROIN",			.position = BodyPart::Position::CENTER },

			{.type = &body_part::UpperArm,	.ID = "BP_RIGHT_UPPER_ARM",	.position = BodyPart::Position::RIGHT, .prefix = "right "},
			{.type = &body_part::LowerArm,	.ID = "BP_RIGHT_LOWER_ARM",	.position = BodyPart::Position::RIGHT, .prefix = "right "},
			{.type = &body_part::Hand,		.ID = "BP_RIGHT_HAND",		.position = BodyPart::Position::RIGHT, .prefix = "right "},

			{.type = &body_part::UpperLeg,	.ID = "BP_LEFT_UPPER_LEG",	.position = BodyPart::Position::LEFT, .prefix = "left "},
			{.type = &body_part::LowerLeg,	.ID = "BP_LEFT_LOWER_LEG",	.position = BodyPart::Position::LEFT, .prefix = "left "},
			{.type = &body_part::Foot,		.ID = "BP_LEFT_FOOT",		.position = BodyPart::Position::LEFT, .prefix = "left "},

			{.type = &body_part::UpperLeg,	.ID = "BP_RIGHT_UPPER_LEG",	.position = BodyPart::Position::RIGHT, .prefix = "right "},
			{.type = &body_part::LowerLeg,	.ID = "BP_RIGHT_LOWER_LEG",	.position = BodyPart::Position::RIGHT, .prefix = "right "},
			{.type = &body_part::Foot,		.ID = "BP_RIGHT_FOOT",		.position = BodyPart::Position::RIGHT, .prefix = "right "},

			{.type = &body_part::Butt, .ID = "BP_BUTT", .position = BodyPart::Position::CENTER },
			{.type = &body_part::Anus, .ID = "BP_ANUS", .position = BodyPart::Position::CENTER },
		},

		.body_connections
		{
			{ "BP_NECK",			"BP_HEAD",				BodyPart::RelativePosition::ABOVE	},
			{ "BP_UPPERBODY",		"BP_NECK",				BodyPart::RelativePosition::ABOVE	},
			{ "BP_UPPERBODY",		"BP_LOWERBODY",			BodyPart::RelativePosition::BELOW	},
			{ "BP_UPPERBODY",		"BP_LEFT_UPPER_ARM",	BodyPart::RelativePosition::LEFT	},
			{ "BP_LEFT_UPPER_ARM",	"BP_LEFT_LOWER_ARM",	BodyPart::RelativePosition::BELOW	},
			{ "BP_LEFT_LOWER_ARM",	"BP_LEFT_HAND",			BodyPart::RelativePosition::BELOW	},
			{ "BP_UPPERBODY",		"BP_RIGHT_UPPER_ARM",	BodyPart::RelativePosition::RIGHT	},
			{ "BP_RIGHT_UPPER_ARM", "BP_RIGHT_LOWER_ARM",	BodyPart::RelativePosition::BELOW	},
			{ "BP_RIGHT_LOWER_ARM", "BP_RIGHT_HAND",		BodyPart::RelativePosition::BELOW	},
			{ "BP_LOWERBODY",		"BP_GROIN",				BodyPart::RelativePosition::BELOW	},
			{ "BP_GROIN",			"BP_LEFT_UPPER_LEG",	BodyPart::RelativePosition::BELOW	},
			{ "BP_LEFT_UPPER_LEG",	"BP_LEFT_LOWER_LEG",	BodyPart::RelativePosition::BELOW	},
			{ "BP_LEFT_LOWER_LEG",	"BP_LEFT_FOOT",			BodyPart::RelativePosition::BELOW	},
			{ "BP_GROIN",			"BP_RIGHT_UPPER_LEG",	BodyPart::RelativePosition::BELOW	},
			{ "BP_RIGHT_UPPER_LEG", "BP_RIGHT_LOWER_LEG",	BodyPart::RelativePosition::BELOW	},
			{ "BP_RIGHT_LOWER_LEG", "BP_RIGHT_FOOT",		BodyPart::RelativePosition::BELOW	},
			{ "BP_GROIN",			"BP_BUTT",				BodyPart::RelativePosition::BEHIND	},
			{ "BP_BUTT",			"BP_ANUS",				BodyPart::RelativePosition::INSIDE	},
}
	};

	static inline void initialize_db()
	{
		BodyPlanDB::init();
		BodyPlanDB::add({ {"GenericBody", GenericBody} });
	}
};

struct BodyMod
{
	snowy::StringID ID;

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
};

namespace body_mod
{
	using BodyModDB = snowy::SingletonDatabase<BodyMod>;

	inline BodyMod DemonParts
	{
		.ID = "DemonParts",

		.add_parts
		{
			{.type = &body_part::Horns,		.ID = "BP_HORNS",	.position = BodyPart::Position::TOP },
			{.type = &body_part::Tail,		.ID = "BP_TAIL",	.position = BodyPart::Position::BACK },
		},

		.add_connections
		{
			{ "BP_HEAD", "BP_HORNS",		BodyPart::RelativePosition::ABOVE },
			{ "BP_LOWERBODY", "BP_TAIL",	BodyPart::RelativePosition::BEHIND },
		},
	};

	inline BodyMod FemaleParts
	{
		.ID = "FemaleParts",

		.add_parts
		{
			{.type = &body_part::Breasts,	.ID = "BP_BREASTS",		.position = BodyPart::Position::FRONT },
			{.type = &body_part::Vagina,	.ID = "BP_VAGINA",		.position = BodyPart::Position::BOTTOM },
		},

		.add_connections
		{
			{ "BP_UPPERBODY", "BP_BREASTS", BodyPart::RelativePosition::IN_FRONT },
			{ "BP_LOWERBODY", "BP_VAGINA",	BodyPart::RelativePosition::BELOW },
		},
	};

	inline BodyMod MaleParts
	{
		.ID = "MaleParts",

		.add_parts
		{
			{.type = &body_part::Penis,			.ID = "BP_PENIS",		.position = BodyPart::Position::FRONT },
			{.type = &body_part::Testicles,		.ID = "BP_TESTICLE",	.position = BodyPart::Position::BOTTOM },
		},

		.add_connections
		{
			{ "BP_UPPERBODY", "BP_PENIS",		BodyPart::RelativePosition::IN_FRONT },
			{ "BP_LOWERBODY", "BP_TESTICLE",	BodyPart::RelativePosition::IN_FRONT },
		},
	};

	inline BodyMod GirlCock
	{
		.ID = "GirlCock",

		.add_parts
		{
			{.type = &body_part::Breasts,		.ID = "BP_BREASTS",		.position = BodyPart::Position::FRONT },
			{.type = &body_part::Penis,			.ID = "BP_PENIS",		.position = BodyPart::Position::FRONT },
			{.type = &body_part::Testicles,		.ID = "BP_TESTICLE",	.position = BodyPart::Position::BOTTOM },
		},

		.add_connections
		{
			{ "BP_UPPERBODY", "BP_BREASTS", BodyPart::RelativePosition::IN_FRONT },
			{ "BP_UPPERBODY", "BP_PENIS",		BodyPart::RelativePosition::IN_FRONT },
			{ "BP_LOWERBODY", "BP_TESTICLE",	BodyPart::RelativePosition::IN_FRONT },
		},
	};

	inline BodyMod BoyPussy
	{
		.ID = "BoyPussy",

		.add_parts
		{
			{.type = &body_part::Vagina,	.ID = "BP_VAGINA",		.position = BodyPart::Position::BOTTOM },
		},

		.add_connections
		{
			{ "BP_LOWERBODY", "BP_VAGINA",	BodyPart::RelativePosition::BELOW },
		},
	};

	inline BodyMod HeadDetails
	{
		.ID = "HeadDetails",

		.add_parts
		{
			{.type = &body_part::Eyes,	.ID = "BP_EYES",	.position = BodyPart::Position::FRONT },
			{.type = &body_part::Mouth,	.ID = "BP_MOUTH",	.position = BodyPart::Position::FRONT },
			{.type = &body_part::Ears,	.ID = "BP_EARS",	.position = BodyPart::Position::SIDES },
			{.type = &body_part::Nose,	.ID = "BP_NOSE",	.position = BodyPart::Position::FRONT },
			{.type = &body_part::Tongue,.ID = "BP_TONGUE",	.position = BodyPart::Position::INSIDE },
		},

		.add_connections
		{
			{ "BP_HEAD", "BP_EYES",	 BodyPart::RelativePosition::IN_FRONT },
			{ "BP_HEAD", "BP_MOUTH", BodyPart::RelativePosition::IN_FRONT },
			{ "BP_HEAD", "BP_EARS", BodyPart::RelativePosition::SIDES },
			{ "BP_HEAD", "BP_NOSE", BodyPart::RelativePosition::IN_FRONT },
			{ "BP_MOUTH", "BP_TONGUE", BodyPart::RelativePosition::INSIDE },
		},

		.add_relations
		{
			{ "BP_EYES", "BP_MOUTH", BodyPart::RelativePosition::BELOW },
			{ "BP_EYES", "BP_NOSE", BodyPart::RelativePosition::BETWEEN },
		}
	};

	static inline void initialize_db()
	{
		BodyModDB::init();
		BodyModDB::add({
			{ "DemonParts",		DemonParts	},
			{ "FemaleParts",	FemaleParts	},
			{ "MaleParts",		MaleParts	},
			{ "GirlCock",		GirlCock	},
			{ "BoyPussy",		BoyPussy	},
			{ "HeadDetails",	HeadDetails	},
			});
	}
}

#endif // !LEMONADE_GAME_SRC_BODY_PLAN_H