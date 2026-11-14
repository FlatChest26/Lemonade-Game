#pragma once

#ifndef LEMONADE_GAME_SRC_MATERIAL_DEF_H
#define LEMONADE_GAME_SRC_MATERIAL_DEF_H

#include "renderable.h"
#include "snowy_grammar.h"
#include "raw_master.h"
#include "snowy_units.h"
#include "thing.h"
#include "time_units.h"
#include "enum_traits.h"
#include "consumable.h"
#include "currency.h"

#include "raw_object_checker.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace units::literals;

enum class MaterialStateType : uint8_t
{
	SOLID, 
	LIQUID, 
	GAS,
	POWDER,
};

inline std::map<std::string, MaterialStateType> AllMaterialStateTypes
{
	{ "solid",		MaterialStateType::SOLID },
	{ "liquid",		MaterialStateType::LIQUID },
	{ "gas",		MaterialStateType::GAS },
	{ "powder",		MaterialStateType::POWDER },
};

enum class MaterialFlag : uint64_t
{
	NONE = 0,

	IS_CONSUMABLE = 1 << 0,
	IS_BLOCKED = 1 << 1,
	IS_OPAQUE = 1 << 2,
	STAINS = 1 << 3,
};

IS_FLAG_ENUM(MaterialFlag);

inline std::map<std::string, MaterialFlag> AllMaterialFlags
{
	{ "consumable",		MaterialFlag::IS_CONSUMABLE },
	{ "is_blocked",		MaterialFlag::IS_BLOCKED	},
	{ "is_opaque",		MaterialFlag::IS_OPAQUE		},
	{ "stains",			MaterialFlag::STAINS		},
};


struct MaterialStateDefinition
{
	Noun noun{ "<unnamed material state>", "<unnamed material state>" };
	color_t color{ color::get("white") };

	std::string prefix{ "<unnamed material state>" };
	std::string description{ "" };

	MaterialFlag flags{ MaterialFlag::NONE };

	units::SpecificHeatCapacity specific_heat_capacity{ 0 };
	units::Density density{ 0 };

	TimeUnit flow_time{ TimeUnit::from_seconds(0.5) };

	std::vector<ConsumableEffect> consumable_effects;
	Currency value{ Currency::from_dollars(1.0) };

	static RawObjectChecker* get_checker();
	static MaterialStateDefinition from_json(const json& j);

public:

	bool is_consumable() const { return flags & MaterialFlag::IS_CONSUMABLE; }
	bool is_blocked() const { return flags & MaterialFlag::IS_BLOCKED; }
	bool is_opaque() const { return flags & MaterialFlag::IS_OPAQUE; }
	bool stains() const { return flags & MaterialFlag::STAINS; }
};

struct MaterialType
{
	RawMaster::RawID ID;

	units::Temperature boiling_point{};
	units::Temperature melting_point{};

	units::MolarMass molar_mass{ units::molar_mass::grams_per_mole_t(18.02) };
	double gas_density_factor;

	std::unordered_map<MaterialStateType, MaterialStateDefinition> states{};

	static RawObjectChecker* get_checker();
	static MaterialType from_json(const json& j);

	constexpr bool operator==(const MaterialType& other) const
	{
		return ID == other.ID;
	}
};

namespace material_types
{
	inline MaterialType NullMaterial
	{
		.ID = "null",
		.states = {}
	};
}


using MaterialAmount = uint8_t;
inline constexpr MaterialAmount DEFAULT_MAX_MATERIAL_AMOUNT = 7;

class Material : public Thing
{
protected:
	RawMaster::Ref<MaterialType> m_mat_type;

	TimeUnit m_current_step;

	MaterialStateType m_state;
	MaterialAmount m_amount;

	units::Temperature m_temperature;
	units::ThermalMass m_thermal_mass;
	units::Mass m_mass;

public:
	struct initializer
	{
		RawMaster::Ref<MaterialType> material_type{ nullptr };
		units::Temperature starting_temperature{ 0 };
		MaterialAmount starting_amount{ 0 };
	};

public:
	Material(const initializer& init = {}) :
		m_mat_type(init.material_type),
		m_temperature(init.starting_temperature),
		m_amount(init.starting_amount)
	{
		update_state();
	}


public:
	RawMaster::Ref<MaterialType> type() const { return m_mat_type; }

	const units::Temperature& boiling_point() const { return m_mat_type->boiling_point; }
	const units::Temperature& condensation_point() const { return m_mat_type->boiling_point; }

	const units::Temperature& melting_point() const { return m_mat_type->melting_point; }
	const units::Temperature& freezing_point() const { return m_mat_type->melting_point; }

	units::Temperature& temperature() { return m_temperature; }
	const units::Temperature& temperature() const { return m_temperature; }

	MaterialAmount& amount() { return m_amount; }
	const MaterialAmount& amount() const { return m_amount; }

	units::Mass& mass() { return m_mass; }
	const units::Mass& mass() const { return m_mass; }

	units::ThermalMass& thermal_mass() { return m_thermal_mass; }
	const units::ThermalMass& thermal_mass() const { return m_thermal_mass; }

	units::Density density() const
	{
		if (current_state() == MaterialStateType::GAS)
		{
			return units::density::kilograms_per_cubic_meter_t(
				units::in<units::temperature::kelvin>(temperature()) / type()->gas_density_factor
			);
		}

		return current_state_definition()->density;
	}

	const units::SpecificHeatCapacity& specific_heat_capacity() const { return current_state_definition()->specific_heat_capacity; }

	TimeUnit& current_step() { return m_current_step; }
	const TimeUnit& current_step() const { return m_current_step; }

	bool should_stain() const
	{
		return current_state() == MaterialStateType::LIQUID && amount() == 1 && current_state_definition()->stains();
	}

	// Current State //
	const MaterialStateType& current_state() const { return m_state; }

	const MaterialStateDefinition* current_state_definition()
	{
		if (!m_mat_type->states.contains(current_state()))
			return nullptr;

		return &m_mat_type->states.at(current_state());
	}

	const MaterialStateDefinition* current_state_definition() const
	{
		if (!m_mat_type->states.contains(current_state()))
			return nullptr;

		return &m_mat_type->states.at(current_state());
	}

	// Thing // 

	virtual Noun noun() const override
	{
		if (auto state_def = current_state_definition())
		{
			return state_def->noun;
		}

		return Noun{ "<undefined material state>", "<undefined material state>" };
	}

	virtual Renderable renderable() const override;
	virtual GrammarNoun display_noun() const override;

public:
	void update_state();
	void update_thermal_mass();
	void update_mass(units::Volume total_container_volume, MaterialAmount max_material_amount = DEFAULT_MAX_MATERIAL_AMOUNT);
};

class Entity;

class MaterialContainer
{
protected:
	Entity* m_owner;
	Material m_material;
	MaterialAmount m_max_amount;
public:
	MaterialContainer(Entity* owner, const Material& material, MaterialAmount max_amount = DEFAULT_MAX_MATERIAL_AMOUNT) :
		m_owner(owner), m_material(material), m_max_amount(max_amount)
	{}

	Entity* get_owner() const { return m_owner; }
	void set_owner(Entity* owner) { m_owner = owner; }

	Material& material() { return m_material; }
	const Material& material() const { return m_material; }

	MaterialAmount& max_amount() { return m_max_amount; }
	const MaterialAmount& max_amount() const { return m_max_amount; }
};


#endif // !LEMONADE_GAME_SRC_MATERIAL_DEF_H