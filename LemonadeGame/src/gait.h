#pragma once

#ifndef LEMONADE_GAME_SRC_GAIT_H
#define LEMONADE_GAME_SRC_GAIT_H

#include <string>
#include <unordered_map>
#include "posture.h"

#include "raw_object_checker.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using GaitName = std::string;

struct GaitDef
{
	GaitName name{ "<unnamed gait>" };
	double speed{ 1.0f };

	PostureType posture_type{ PostureType::STAND };

	bool is_default{ false };

	static RawObjectChecker* get_checker();
	static GaitDef from_json(const json& j);
};

using GaitMap = std::unordered_map<GaitName, GaitDef>;

class Gait
{
protected:
	GaitMap m_gaits{};
	GaitName m_current{};

public:
	Gait(const GaitMap& gaits, const GaitName& current = "") :
		m_gaits(gaits), m_current(current)
	{
		if (m_current.empty())
		{
			m_current = get_default_gait_name();
		}
	}

	std::vector<const GaitDef*> get_gaits() const
	{
		std::vector<const GaitDef*> gaits;

		for (auto& [gait_key, gait_type] : m_gaits)
		{
			gaits.push_back(&gait_type);
		}

		return gaits;
	}

	const GaitDef* get_current_gait() const
	{
		if (m_gaits.empty() || m_current.empty() || !m_gaits.contains(m_current))
		{
			return nullptr;
		}

		return &m_gaits.at(m_current);
	}

	GaitName get_default_gait_name(const PostureType& posture_type = PostureType::STAND) const
	{
		for (auto& [gait_key, gait_type] : m_gaits)
		{
			if (gait_type.posture_type != posture_type)
				continue;

			if (gait_type.is_default)
			{
				return gait_key;
			}
		}

		return "walk";
	}

	void set_current_gait(const GaitName& gait_id)
	{
		if (!m_gaits.contains(gait_id))
		{
			CERR("Invalid gait name: " << gait_id);
			return;
		}

		m_current = gait_id;
	}

	double get_current_speed() const
	{
		return get_current_gait()->speed;
	}
};

namespace gaits
{
	inline GaitMap StandardGaits
	{
		{ "walk", GaitDef{ .name = "walk", .speed = 1.0, .posture_type = PostureType::STAND, .is_default = true } },
		{ "crawl", GaitDef{ .name = "crawl", .speed = 0.8, .posture_type = PostureType::CRAWL, .is_default = true } }
	};
}

#endif // !LEMONADE_GAME_SRC_GAIT_H
