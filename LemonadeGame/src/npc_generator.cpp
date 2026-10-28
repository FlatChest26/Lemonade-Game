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

template<typename T>
using WeightedVector = std::vector<std::tuple<double, T>>;

template<typename T>
static T get_weighted_random(WeightedVector<T> vector, std::mt19937 generator)
{
	std::vector<double> weights;
	std::vector<T> values;

	for (const auto& [key, val] : vector)
	{
		weights.push_back(key);
		values.push_back(val);
	}

	std::discrete_distribution<size_t> weighted_distribution(weights.begin(), weights.end());
	return values[weighted_distribution(generator)];
}


Entity* NPCGenerator::generate_npc()
{
	WeightedVector<RawMaster::RawID> ValidSpecies;

	for (const auto& species_id : RawMaster::get_ids<Species>())
	{
		auto species_def = RawMaster::get<Species>(species_id);
		if (species_def->does_not_exist())
			continue;
		
		ValidSpecies.emplace_back(species_def->frequency, species_id);
	}

	WeightedVector<snowy::StringID> ValidColors;
	for (const auto& color_id : RawMaster::get_color_ids())
	{
		if (color_id == "black" || color_id == "pitch_black")
			continue;

		ValidColors.emplace_back(1.0, color_id);
	}

	static WeightedVector<Gender> ValidGender{
		{ 1.0f,  gender::NON_SPECIFIC }, 
		{ 10.0f, gender::MASCULINE },
		{ 10.0f, gender::FEMININE },
	};

	static WeightedVector<RawMaster::RawID> ValidBodyMods{
		{ 10.0f, "FemaleParts"	},
		{ 10.0f, "MaleParts"	},
		{ 5.0f,  "GirlCock"		},
		{ 5.0f,  "BoyPussy"		},
	};
	
	// Generate random species
	auto random_species = RawMaster::get<Species>(get_weighted_random(ValidSpecies, snowy::rng::generator));

	// Generate random age
	std::uniform_int_distribution<Age::age_t> lifespan_dist(random_species->lifespan_min, random_species->lifespan_max);
	Age random_lifespan{ lifespan_dist(snowy::rng::generator) };

	std::uniform_int_distribution<Age::age_t> age_distrib(random_species->adult_age, random_lifespan);
	Age random_age{ age_distrib(snowy::rng::generator) };

	// Generate gender
	Gender random_gender = get_weighted_random(ValidGender, snowy::rng::generator);

	// Random color
	color_t random_color = color::get(get_weighted_random(ValidColors, snowy::rng::generator));

	// Generate name
	RawMaster::Ref<NameSet> first_name_set;
	RawMaster::Ref<NameSet> family_name_set = RawMaster::get<NameSet>("family_name");

	if (random_gender.gender_type & Gender::type::FEMININE)
	{
		first_name_set = RawMaster::get<NameSet>("female_given");
	}
	else if (random_gender.gender_type & Gender::type::MASCULINE)
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
	random_body_mods.push_back(RawMaster::get<BodyMod>(get_weighted_random(ValidBodyMods, snowy::rng::generator)));
	 
	// Make NPC
	auto npc = EntityBuilder::create_creature(
		"NPC_" + std::to_string(m_npc_count),
		{
			.soul = new CreatureSoul{
				.ID = "NPC_SOUL_" + std::to_string(m_npc_count),
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
