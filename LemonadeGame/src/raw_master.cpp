#include "raw_master.h"
#include "lemonade_game.h"
#include <fstream>
#include "file_management.h"

#include "snowy_macros.h"

#include <unordered_map>
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
#include "material_def.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace fs = std::filesystem;

namespace RawMaster
{
	using NameSetsDB = RawDB<NameSet>;
	using TileTypeDB = RawDB<TileType>;
	using BodyPlanDB = RawDB<BodyPlan>;
	using BodyPartTypeDB = RawDB<BodyPartType>;
	using BodyModDB = RawDB<BodyMod>;
	using BodyGlossDB = RawDB<BodyGloss>;
	using SpeciesDB = RawDB<Species>;
	using ItemDefsDB = RawDB<ItemDefinition>;
	using FurnitureDefsDB = RawDB<FurnitureDefinition>;
	using MaterialTypesDB = RawDB<MaterialType>;
	using PronounsDB = RawDB<Pronouns>;
	using GendersDB = RawDB<Gender>;

	inline std::vector<std::string> VALID_TYPES
	{
		"pronouns",
		"gender",
		"name_set",
		"color",
		"material_type",
		"tile_type",
		"body_part_type",
		"body_plan",
		"body_mod",
		"body_gloss",
		"item_definition",
		"furniture_definition",
		"species",
	};

	std::unordered_map<RawID, ModID> mod_ids;

	bool has_mod_id(const RawID& id)
	{
		return mod_ids.contains(id);
	}

	ModID get_mod_id(const RawID& id)
	{
		if (has_mod_id(id))
		{
			return mod_ids.at(id);
		}

		return {};
	}

	static void initialize_databases()
	{
		if (!color::_colors_initialized)
			color::initialize_db();

		NameSetsDB::init();
		PronounsDB::init();
		GendersDB::init();

		MaterialTypesDB::init();
		MaterialTypesDB::add({
			{ material_types::NullMaterial.ID,  &material_types::NullMaterial }
		});

		TileTypeDB::init();
		TileTypeDB::add({
			{ tile_types::VoidTile.ID, &tile_types::VoidTile }
		});

		BodyPartTypeDB::init();
		
		BodyGlossDB::init();
		BodyPlanDB::init();
		BodyModDB::init();

		ItemDefsDB::init();
		FurnitureDefsDB::init();

		SpeciesDB::init();
		SpeciesDB::add({
			{ species::NULL_CREATURE.ID, &species::NULL_CREATURE }
		});
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
		using TDB = RawDB<T>;

		for (const auto& json_obj : json)
		{
			if (!json_obj.contains("ID") || !json_obj["ID"].is_string())
			{
				CERR("Invalid 'ID' field in species JSON object: " << json_obj);
				continue;
			}

			auto ID = json_obj.at("ID").get<std::string>();

			Ref<T> new_type = make_ref<T>(json_obj);
			TDB::add(ID, new_type);

			mod_ids[ID] = json_obj["mod_id"];

			COUT("\tLoaded: " << json_obj.at("ID").get<std::string>());
		}
	}

	static void collect_json_from_raw_path(ModID mod_id, std::filesystem::path raw_path, std::unordered_map<std::string, std::vector<json>>& raw_data)
	{
		for (const auto& entry : fs::recursive_directory_iterator(raw_path))
		{
			if (entry.is_directory())
				continue;

			if (!entry.exists())
				continue;

			if (!entry.is_regular_file())
				continue;

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


				json_obj["mod_id"] = mod_id;
				raw_data[type].push_back(json_obj);
			}
		}
	}

	void load_raws()
	{
		_raws_loading_allowed = true;

		auto core_raws = files::raws_directory / "core";
		auto mod_raws = files::raws_directory / "mods";

		std::unordered_map<std::string, std::vector<json>> raw_data;

		// Get core json files
		collect_json_from_raw_path("core", core_raws, raw_data);

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

		COUT("Loading material types\n");
		load_raw_type<MaterialType>(raw_data["material_type"]);

		COUT("Loading tile types\n");
		load_raw_type<TileType>(raw_data["tile_type"]);

		COUT("Loading body part types\n");
		load_raw_type<BodyPartType>(raw_data["body_part_type"]);

		COUT("Loading body plans\n");
		load_raw_type<BodyPlan>(raw_data["body_plan"]);

		COUT("Loading body gloss\n");
		load_raw_type<BodyGloss>(raw_data["body_gloss"]);

		COUT("Loading body mods\n");
		load_raw_type<BodyMod>(raw_data["body_mod"]);

		COUT("Loading item definitions\n");
		load_raw_type<ItemDefinition>(raw_data["item_definition"]);

		COUT("Loading furniture definitions\n");
		load_raw_type<FurnitureDefinition>(raw_data["furniture_definition"]);

		COUT("Loading species\n");
		load_raw_type<Species>(raw_data["species"]);

		_raws_loading_allowed = false;
	}
}