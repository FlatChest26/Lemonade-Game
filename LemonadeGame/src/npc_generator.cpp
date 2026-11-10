#include "npc_generator.h"
#include "world_state.h"
#include "world.h"
#include "entity_builder.h"

#include "name_set.h"
#include "raw_master.h"


NPCGenerator::NPCGenerator(AIManager* owner) :
	m_owner(owner)
{
}

AIManager* NPCGenerator::get_ai_manager() const
{
	return m_owner;
}

WorldGameState* NPCGenerator::get_world_state() const
{
	if (m_owner)
	{
		return m_owner->get_world_state();
	}

	return nullptr;
}

World* NPCGenerator::get_world() const
{
	if (auto world_state = get_world_state())
	{
		return world_state->get_world();
	}
	return nullptr;
}



namespace npc_generator
{
	static bool _initialized = false;

	static snowy::rng::WeightedVector<color_t> ValidColors;
	static snowy::rng::WeightedVector<RawMaster::Ref<Species>> ValidSpecies;
	static snowy::rng::WeightedVector<RawMaster::Ref<BodyMod>> ValidBodyMods;

	static std::unordered_map<SpeciesID, snowy::rng::WeightedVector<RawMaster::Ref<Gender>>> ValidSpeciesGenders;
	static std::unordered_map<SpeciesID, snowy::rng::WeightedVector<RawMaster::Ref<BodyMod>>> ValidSpeciesBodyMods;

	void initialize_npc_banks()
	{
		if (_initialized)
			return;

		_initialized = true;

		for (const auto& color_id : RawMaster::get_color_ids())
		{
			if (color_id == "black" || color_id == "pitch_black")
				continue;

			ValidColors.emplace_back(1.0, color::get(color_id));
		}

		for (const auto& species_id : RawMaster::get_ids<Species>())
		{
			auto species_def = RawMaster::get<Species>(species_id);
			
			if (species_def->does_not_exist())
				continue;

			ValidSpecies.emplace_back(species_def->frequency, RawMaster::get<Species>(species_id));

			for (const auto& gender_id : RawMaster::get_ids<Gender>())
			{
				auto gender_def = RawMaster::get<Gender>(gender_id);

				if (!species_def->has_common_gender_type(gender_def->gender_type))
					continue;

				ValidSpeciesGenders[species_def->ID].emplace_back(
					species_def->get_common_gender_type_frequency(gender_def->gender_type), 
					RawMaster::get<Gender>(gender_id)
				);
			}

			for (auto& [body_mod, frequency] : species_def->common_body_mods)
			{
				ValidSpeciesBodyMods[species_def->ID].emplace_back(frequency, body_mod);
			}
		}
	}
}

Entity NPCGenerator::generate_npc()
{
	// Random color
	color_t random_color = snowy::rng::get_weighted_random(npc_generator::ValidColors, snowy::rng::generator);

	// Generate random species
	RawMaster::Ref<Species> random_species = snowy::rng::get_weighted_random(npc_generator::ValidSpecies, snowy::rng::generator);

	// Generate random age
	std::uniform_int_distribution<Age::age_t> lifespan_dist(random_species->lifespan_min, random_species->lifespan_max);
	Age random_lifespan{ lifespan_dist(snowy::rng::generator) };

	std::uniform_int_distribution<Age::age_t> age_distrib(random_species->adult_age, random_lifespan);
	Age random_age{ age_distrib(snowy::rng::generator) };

	// Generate gender
	RawMaster::Ref<Gender> random_gender = snowy::rng::get_weighted_random(npc_generator::ValidSpeciesGenders[random_species->ID], snowy::rng::generator);

	// Generate name
	RawMaster::Ref<NameSet> first_name_set;
	RawMaster::Ref<NameSet> family_name_set = RawMaster::get<NameSet>("family_name");

	if (random_gender->gender_type & GenderType::FEMININE)
	{
		first_name_set = RawMaster::get<NameSet>("female_given");
	}
	else if (random_gender->gender_type & GenderType::MASCULINE)
	{
		first_name_set = RawMaster::get<NameSet>("male_given");
	}
	else
	{
		first_name_set = RawMaster::get<NameSet>("nickname");
	}

	Name random_name;

	if (!first_name_set)
	{
		CERR("No name set found!");
		random_name.nickname = "Nameless";
	}
	else
	{
		std::uniform_int_distribution<size_t> name_distrib(0, first_name_set->names_json.size() - 1);
		random_name.nickname = first_name_set->get_name(name_distrib(snowy::rng::generator));
	}

	if (!family_name_set)
	{
		CERR("No name set found!");
		random_name.full_name = random_name.nickname;
	}
	else
	{
		std::uniform_int_distribution<size_t> name_distrib(0, family_name_set->names_json.size() - 1);
		random_name.full_name = random_name.nickname + " " + family_name_set->get_name(name_distrib(snowy::rng::generator));
	}

	// Generate body mods
	std::vector<RawMaster::Ref<BodyMod>> random_body_mods;
	random_body_mods.push_back(snowy::rng::get_weighted_random(npc_generator::ValidSpeciesBodyMods[random_species->ID], snowy::rng::generator));

	// Make NPC
	auto npc = EntityBuilder::create_creature(
		"NPC_" + std::to_string(m_npc_count),
		{
			.soul = new CreatureSoul
			{	
				.name = random_name,
				.gender = random_gender,
				.color = random_color,
			},
			.species = random_species,
			.lifespan = random_lifespan,
			.current_age = random_age,
			.body_mods = random_body_mods
		}
	);

	m_npc_count++;

	/*
	COUT("Spawned in NPC: " << npc->ID());
	COUT("\tName: " << npc->full_name());
	COUT("\tGender: " << npc->get_soul()->gender_noun());
	COUT("\tSpecies: " << npc->get_species()->noun);
	COUT("\tAge: " << npc->get_living()->get_age());
	COUT("\tLifespan: " << npc->get_living()->get_lifespan());
	*/

	return npc;
}