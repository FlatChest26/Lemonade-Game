#include "raw_master.h"
#include "lemonade_game.h"
#include <fstream>
#include "file_management.h"

#include "snowy_macros.h"

#include <map>
#include <vector>

#include "species.h"
#include "body_part_type.h"
#include "body_plan.h"
#include "tile_type.h"
#include "color.h"
#include "name_set.h"
#include "item_def.h"
#include "furniture_def.h"
#include "snowy_grammar.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace fs = std::filesystem;

namespace RawMaster
{
	using NameSetsDB = snowy::SingletonDatabase<Ref<NameSet>>;
	using TileTypeDB = snowy::SingletonDatabase<Ref<TileType>>;
	using BodyPlanDB = snowy::SingletonDatabase<Ref<BodyPlan>>;
	using BodyPartTypeDB = snowy::SingletonDatabase<Ref<BodyPartType>>;
	using BodyModDB = snowy::SingletonDatabase<Ref<BodyMod>>;
	using SpeciesDB = snowy::SingletonDatabase<Ref<Species>>;
	using ItemDefsDB = snowy::SingletonDatabase<Ref<ItemDefinition>>;
	using FurnitureDefsDB = snowy::SingletonDatabase<Ref<FurnitureDefinition>>;

	using PronounsDB = snowy::SingletonDatabase<Ref<Pronouns>>;
	using GendersDB = snowy::SingletonDatabase<Ref<Gender>>;

	inline std::vector<std::string> VALID_TYPES
	{
		"body_part_type",
		"body_mod",
		"body_plan",
		"tile_type",
		"species",
		"color",
		"name_set",
		"item_definition",
		"furniture_definition",
		"pronouns",
		"gender",
	};

	static void initialize_databases()
	{
		if (!color::_colors_initialized)
			color::initialize_db();

		NameSetsDB::init();

		PronounsDB::init();
		GendersDB::init();

		SpeciesDB::init();
		SpeciesDB::add({
			{ "NULL_CREATURE", std::make_shared<Species>(species::NULL_CREATURE) }
		});

		BodyPartTypeDB::init();
		BodyPartTypeDB::add({
			{ "BP_HEAD",			std::make_shared<BodyPartType>(body_part::Head) },
			{ "BP_NECK",			std::make_shared<BodyPartType>(body_part::Neck) },
			{ "BP_UPPERBODY",		std::make_shared<BodyPartType>(body_part::UpperBody) },
			{ "BP_LOWERBODY",		std::make_shared<BodyPartType>(body_part::LowerBody) },
			{ "BP_UPPER_ARM",		std::make_shared<BodyPartType>(body_part::UpperArm) },
			{ "BP_LOWER_ARM",		std::make_shared<BodyPartType>(body_part::LowerArm) },
			{ "BP_HAND",			std::make_shared<BodyPartType>(body_part::Hand) },
			{ "BP_GROIN",			std::make_shared<BodyPartType>(body_part::Groin) },
			{ "BP_UPPER_LEG",		std::make_shared<BodyPartType>(body_part::UpperLeg) },
			{ "BP_LOWER_LEG",		std::make_shared<BodyPartType>(body_part::LowerLeg) },
			{ "BP_FOOT",			std::make_shared<BodyPartType>(body_part::Foot) },
			{ "BP_PENIS",			std::make_shared<BodyPartType>(body_part::Penis) },
			{ "BP_VAGINA",			std::make_shared<BodyPartType>(body_part::Vagina) },
			{ "BP_BREASTS",			std::make_shared<BodyPartType>(body_part::Breasts) },
			{ "BP_TESTICLES",		std::make_shared<BodyPartType>(body_part::Testicles) },
			{ "BP_BUTT",			std::make_shared<BodyPartType>(body_part::Butt) },
			{ "BP_ANUS",			std::make_shared<BodyPartType>(body_part::Anus) },
			{ "BP_EYES",			std::make_shared<BodyPartType>(body_part::Eyes) },
			{ "BP_MOUTH",			std::make_shared<BodyPartType>(body_part::Mouth) },
			{ "BP_EARS",			std::make_shared<BodyPartType>(body_part::Ears) },
			{ "BP_NOSE",			std::make_shared<BodyPartType>(body_part::Nose) },
			{ "BP_TONGUE",			std::make_shared<BodyPartType>(body_part::Tongue) },
			{ "BP_TAIL",			std::make_shared<BodyPartType>(body_part::Tail) },
			{ "BP_HORNS",			std::make_shared<BodyPartType>(body_part::Horns) },
		});

		TileTypeDB::init();
		TileTypeDB::add({
			{ "TILE_VOID", std::make_shared<TileType>(tile_types::VoidTile) }
		});

		BodyPlanDB::init();
		BodyPlanDB::add({
			{ "GenericBody", std::make_shared<BodyPlan>(body_plan::GenericBody) }
		});

		BodyModDB::init();
		BodyModDB::add({
			{ "DemonParts",		std::make_shared<BodyMod>(body_mod::DemonParts) },
			{ "FemaleParts",	std::make_shared<BodyMod>(body_mod::FemaleParts) },
			{ "MaleParts",		std::make_shared<BodyMod>(body_mod::MaleParts) },
			{ "GirlCock",		std::make_shared<BodyMod>(body_mod::GirlCock) },
			{ "BoyPussy",		std::make_shared<BodyMod>(body_mod::BoyPussy) },
			{ "HeadDetails",	std::make_shared<BodyMod>(body_mod::HeadDetails) },
		});

		ItemDefsDB::init();
		FurnitureDefsDB::init();
	}

	static void load_colors(std::vector<json> json)
	{
		for (const auto& json_obj : json)
		{
			if (!json_obj.contains("ID") || !json_obj["ID"].is_string())
			{
				CERR("Invalid 'ID' field in species JSON object: " << json_obj);
				continue;
			}

			auto ID = json_obj.at("ID").get<std::string>();

			color::ColorsDB::add(ID, color::from_json(json_obj.at("color")));

			COUT("\tLoaded: " << ID);
		}
	}

	template<typename T>
	static void load_raw_type(std::vector<json> json)
	{
		using TDB = snowy::SingletonDatabase<Ref<T>>;

		for (const auto& json_obj : json)
		{
			if (!json_obj.contains("ID") || !json_obj["ID"].is_string())
			{
				CERR("Invalid 'ID' field in species JSON object: " << json_obj);
				continue;
			}

			Ref<T> new_type = std::make_shared<T>(T::from_json(json_obj));
			TDB::add(json_obj.at("ID").get<std::string>(), new_type);

			COUT("\tLoaded: " << json_obj.at("ID").get<std::string>());
		}
	}

	static void collect_json_from_raw_path(std::filesystem::path raw_path, std::map<std::string, std::vector<json>>& raw_data)
	{
		for (const auto& entry : fs::recursive_directory_iterator(raw_path))
		{
			if (entry.path().filename().extension() != ".json")
			{
				CERR("Non .json file found: " << entry.path() << " in directory: " << raw_path);
				continue;
			}

			std::ifstream f(entry.path());
			json data = json::parse(f);

			if (!data.is_array())
			{
				CERR("Expected JSON array in file " << entry.path());
				continue;
			}

			for (auto& json_obj : data)
			{
				if (!json_obj.is_object())
				{
					continue;
				}

				if (!json_obj.contains("type") || !json_obj.at("type").is_string())
				{
					CERR("Raw object in file " << entry.path() << " does not contain a valid 'type' field: " << json_obj);
					continue;
				}

				std::string type = json_obj.at("type").get<std::string>();

				if (std::find(VALID_TYPES.begin(), VALID_TYPES.end(), type) == VALID_TYPES.end())
				{
					CERR("Raw object in file " << entry.path() << " found but has an invalid type: " << type);
					continue;
				}

				raw_data[type].push_back(json_obj);
			}
		}
	}

	void load_raws()
	{
		_raws_loading_allowed = true;

		auto core_raws = files::raws_directory / "core";
		auto mod_raws = files::raws_directory / "mods";

		std::map<std::string, std::vector<json>> raw_data;

		// Get core json files
		collect_json_from_raw_path(core_raws, raw_data);

		// Initialize databases
		initialize_databases();

		// Load raws in order
		COUT("Loading colors\n");
		load_colors(raw_data["color"]);

		COUT("Loading pronouns\n");
		load_raw_type<Pronouns>(raw_data["pronouns"]);

		COUT("Loading gender\n");
		load_raw_type<Gender>(raw_data["gender"]);

		COUT("Loading name sets\n");
		load_raw_type<NameSet>(raw_data["name_set"]);

		COUT("Loading tile types\n");
		load_raw_type<TileType>(raw_data["tile_type"]);

		COUT("Loading item definitions\n");
		load_raw_type<ItemDefinition>(raw_data["item_definition"]);

		COUT("Loading furniture definitions\n");
		load_raw_type<FurnitureDefinition>(raw_data["furniture_definition"]);

		COUT("Loading body part types\n");
		load_raw_type<BodyPartType>(raw_data["body_part_type"]);

		COUT("Loading body plans\n");
		load_raw_type<BodyPlan>(raw_data["body_plan"]);

		COUT("Loading body mods\n");
		load_raw_type<BodyMod>(raw_data["body_mod"]);

		COUT("Loading species\n");
		load_raw_type<Species>(raw_data["species"]);

		_raws_loading_allowed = false;
	}
}