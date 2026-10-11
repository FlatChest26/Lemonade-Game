#pragma once

#ifndef LEMONADE_GAME_SRC_RAW_MASTER_H
#define LEMONADE_GAME_SRC_RAW_MASTER_H

#include "species.h"
#include "body_part_type.h"
#include "body_plan.h"
#include "tile_type.h"

namespace RawMaster
{
	extern void load_raws();

	inline constexpr TileType* get_tile_type(const snowy::StringID& id)
	{
		if (tile_types::TileTypeDB::has(id))
		{
			return &(tile_types::TileTypeDB::get(id));
		}

		if (!tile_types::tiles_db.contains(id))
		{
			CERR("Unrecognized tile: " << id);
			return &tile_types::NO_TILE;
		}

		return tile_types::tiles_db[id];
	}

	inline constexpr Species* get_species(const snowy::StringID& id)
	{
		if (species::SpeciesDB::has(id))
		{
			return &species::SpeciesDB::get(id);
		}

		if (!species::_species_db.contains(id))
		{
			CERR("Unrecognized species id: " << id);
			return &species::HUMAN;
		}

		return species::_species_db[id];
	}

	inline constexpr BodyPlan get_body_plan(const snowy::StringID& id)
	{
		if (!body_plan::BodyPlanDB::has(id))
		{
			CERR("Unrecognized body plan: " << id);
			return {};
		}

		return body_plan::BodyPlanDB::get(id);
	}

	inline BodyMod get_body_mod(const snowy::StringID& id)
	{
		if (!body_mod::BodyModDB::has(id))
		{
			CERR("Unrecognized body mod: " << id);
			return {};
		}

		return body_mod::BodyModDB::get(id);
	}

	inline constexpr BodyPartType* get_body_part_type(const snowy::StringID& id)
	{
		if (body_part::BodyPartTypeDB::has(id))
		{
			return &body_part::BodyPartTypeDB::get(id);
		}

		if (!body_part::_body_parts_db.contains(id))
		{
			CERR("Unrecognized body part type id: " << id);
			return nullptr;
		}

		return body_part::_body_parts_db[id];
	}
};

#endif // !LEMONADE_GAME_SRC_RAW_MASTER_H