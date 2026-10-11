#include "raw_master.h"
#include "lemonade_game.h"
#include <fstream>
#include "file_management.h"

#include "snowy_macros.h"

#include <map>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace fs = std::filesystem;

inline std::vector<std::string> VALID_TYPES
{
	"body_part_type",
	"tile_type",
	"species",
};

static void initialize_databases()
{
	tile_types::initialize_db();
	body_part::initialize_db();
	body_plan::initialize_db();
	body_mod::initialize_db();
	species::initialize_db();
}

static void collect_json_from_raw_path(std::filesystem::path raw_path, std::map<std::string, std::vector<json>>& raw_data)
{
	for (const auto& entry : fs::directory_iterator(raw_path))
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

			if (!json_obj.contains("type") || !json_obj["type"].is_string())
			{
				CERR("Raw object in file " << entry.path() << " does not contain a valid 'type' field.");
				continue;
			}

			std::string type = json_obj["type"].get<std::string>();

			if (std::find(VALID_TYPES.begin(), VALID_TYPES.end(), type) == VALID_TYPES.end())
			{
				CERR("Raw object in file " << entry.path() << " found but has an invalid type: " << type);
				continue;
			}

			raw_data[type].push_back(json_obj);
		}
	}
}

static void load_body_part_type(std::vector<json> json)
{
	COUT("Loading body part types\n");

	for (const auto& json_obj : json)
	{
		if (!json_obj.contains("ID") || !json_obj["ID"].is_string())
		{
			CERR("Invalid 'ID' field in species JSON object: " << json_obj);
			continue;
		}

		auto new_body_part_type = BodyPartType::from_json(json_obj);
		body_part::BodyPartTypeDB::add(new_body_part_type.ID, new_body_part_type);

		COUT("Loaded: " << new_body_part_type.ID);
	}
}

static void load_tile_types(std::vector<json> json)
{
	COUT("Loading tile types\n");

	for (const auto& json_obj : json)
	{
		if (!json_obj.contains("ID") || !json_obj["ID"].is_string())
		{
			CERR("Invalid 'ID' field in species JSON object: " << json_obj);
			continue;
		}

		auto new_tile_type = TileType::from_json(json_obj);
		tile_types::TileTypeDB::add(new_tile_type.ID(), new_tile_type);

		COUT("Loaded: " << new_tile_type.ID());
	}
}

static void load_species(std::vector<json> json)
{
	COUT("Loading species\n");

	for (const auto& json_obj : json)
	{
		if (!json_obj.contains("ID") || !json_obj["ID"].is_string())
		{
			CERR("Invalid 'ID' field in species JSON object: " << json_obj);
			continue;
		}

		auto new_species = Species::from_json(json_obj);
		species::SpeciesDB::add(new_species.ID, new_species);

		COUT("Loaded: " << new_species.ID);
	}
}

void RawMaster::load_raws()
{
	auto core_raws = files::raws_directory / "core";
	auto mod_raws = files::raws_directory / "mods";

	std::map<std::string, std::vector<json>> raw_data;

	// Get core json files
	collect_json_from_raw_path(core_raws, raw_data);

	// Initialize databases
	initialize_databases();

	// Load raws in order
	load_tile_types(raw_data["tile_type"]);

	load_body_part_type(raw_data["body_part_type"]);

	load_species(raw_data["species"]);
}