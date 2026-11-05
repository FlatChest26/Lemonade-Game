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
};

IS_FLAG_ENUM(MaterialFlag);

inline std::map<std::string, MaterialFlag> AllMaterialFlags
{
	{ "consumable",		MaterialFlag::IS_CONSUMABLE },
	{ "is_blocked",		MaterialFlag::IS_BLOCKED },
	{ "is_opaque",		MaterialFlag::IS_OPAQUE },
};


struct MaterialStateDefinition
{
	Noun noun{ "material", "materials" };
	std::string prefix;
	std::string description;
	color_t color;

	MaterialFlag flags;

	std::vector<ConsumableEffect> consumable_effects;
	Currency value{ Currency::from_dollars(1.0) };

	static std::shared_ptr<RawObjectChecker> get_checker();
	static MaterialStateDefinition from_json(const json& j);

public:

	bool is_consumable() const { return flags & MaterialFlag::IS_CONSUMABLE; }
	bool is_blocked() const { return flags & MaterialFlag::IS_BLOCKED; }
	bool is_opaque() const { return flags & MaterialFlag::IS_OPAQUE; }
};

struct MaterialType
{
	RawMaster::RawID ID;

	units::Temperature boiling_point{};
	units::Temperature melting_point{};

	std::unordered_map<MaterialStateType, MaterialStateDefinition> states{};

	static std::shared_ptr<RawObjectChecker> get_checker();
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
	units::Temperature m_temperature;
	MaterialStateType m_state;
	MaterialAmount m_amount;

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
	const MaterialType* material_type() const { return m_mat_type.get(); }
	RawMaster::Ref<MaterialType> get_material_type() const { return m_mat_type; }

	const units::Temperature& boiling_point() const { return m_mat_type->boiling_point; }
	const units::Temperature& condensation_point() const { return m_mat_type->boiling_point; }

	const units::Temperature& melting_point() const { return m_mat_type->melting_point; }
	const units::Temperature& freezing_point() const { return m_mat_type->melting_point; }

	units::Temperature& temperature() { return m_temperature; }
	const units::Temperature& temperature() const { return m_temperature; }

	MaterialAmount& amount() { return m_amount; }
	const MaterialAmount& amount() const { return m_amount; }

	TimeUnit& current_step() { return m_current_step; }
	const TimeUnit& current_step() const { return m_current_step; }


	// Current State //

	MaterialStateType& current_state()
	{
		update_state();
		return m_state;
	}

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

		return Noun{ "<undefined material state>", "<undefined material states>" };
	}

	virtual Renderable renderable() const override
	{
		Renderable renderable = { ' ', color::get("white"), color::get("black") };

		switch (current_state())
		{
		case MaterialStateType::SOLID:
			renderable.glyph = 219; // █
			break;
		case MaterialStateType::LIQUID:
			renderable.glyph = 247; // ≈
			break;
		case MaterialStateType::GAS:
			renderable.glyph = 177; // ▒
			break;
		case MaterialStateType::POWDER:
			renderable.glyph = 219; // █
			break;
		}

		if (auto state_def = current_state_definition())
		{
			renderable.fg = state_def->color;
		}

		return renderable;
	}

	virtual GrammarNoun display_noun() const override
	{
		std::string noun = singular_noun();

		if (current_state() == MaterialStateType::LIQUID)
		{
			if (amount() == 1)
			{
				noun = "spattering of " + singular_noun();
			}
			else if (amount() > 3)
			{
				noun = "pool of " + singular_noun();
			}
			else
			{
				noun = "puddle of " + singular_noun();
			}
		}
		else if (current_state() == MaterialStateType::SOLID)
		{
			if (amount() == 1)
			{
				noun = "sheet of " + singular_noun();
			}
			else if (amount() > 3)
			{
				noun = "block of " + singular_noun();
			}
			else
			{
				noun = "chunk of " + singular_noun();
			}
		}
		else if (current_state() == MaterialStateType::GAS)
		{
			if (amount() > 2)
			{
				noun = "cloud of " + singular_noun();
			}
			else
			{
				noun = "puff of " + singular_noun();
			}
		}
		else if (current_state() == MaterialStateType::POWDER)
		{
			if (amount() > 3)
			{
				noun = "pile of " + singular_noun();
			}
			else
			{
				noun = "sheet of " + singular_noun();
			}
		}

		return grammar::a(noun);
	}

public:
	void update_state()
	{
		if (!m_mat_type || m_mat_type->states.empty())
			return;

		if (m_mat_type->states.contains(MaterialStateType::GAS) && m_temperature >= m_mat_type->boiling_point)
		{
			m_state = MaterialStateType::GAS;
		}
		else if (m_mat_type->states.contains(MaterialStateType::LIQUID) && m_temperature >= m_mat_type->melting_point)
		{
			m_state = MaterialStateType::LIQUID;
		}
		else if (m_mat_type->states.contains(MaterialStateType::SOLID) && m_state != MaterialStateType::POWDER)
		{
			m_state = MaterialStateType::SOLID;
		}
	}
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