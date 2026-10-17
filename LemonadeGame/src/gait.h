#pragma once


#ifndef LEMONADE_GAME_SRC_GAIT_H
#define LEMONADE_GAME_SRC_GAIT_H

#include <string>
#include <unordered_map>

struct GaitType
{
	std::string name{ "<unnamed gait>" };
	double speed{ 1.0f };
	bool default_gait{ false };
};

using GaitMap = std::unordered_map<std::string, GaitType>;

class Gait
{
protected:
	GaitMap m_gaits{ };
	std::string m_current_gait{ };

public:
	Gait(const GaitMap& gaits, const std::string& current_gait = "") :
		m_gaits(gaits), m_current_gait(current_gait)
	{
		if (m_current_gait.empty())
		{
			for (auto& [gait_key, gait_type] : m_gaits)
			{
				if (gait_type.default_gait)
				{
					m_current_gait = gait_key;
					break;
				}
			}
		}
	}

	std::vector<GaitType*> get_gaits()
	{
		std::vector<GaitType*> gaits;

		for (auto& [gait_key, gait_type] : m_gaits)
		{
			gaits.push_back(&gait_type);
		}

		return gaits;
	}

	const GaitType* get_current_gait() const
	{
		if (m_gaits.empty() || !m_gaits.contains(m_current_gait))
		{
			return nullptr;
		}

		return &m_gaits.at(m_current_gait);
	}

	void set_current_gait(std::string gait_id)
	{
		m_current_gait = gait_id;
	}

	double get_current_speed() const
	{
		return get_current_gait()->speed;
	}
};


namespace gaits
{
	inline std::unordered_map<std::string, GaitType> StandardGaits{
		{ "CREEP",		{"stroll",	0.2	}		},
		{ "STROLL",		{"stroll",	0.5	}		},
		{ "WALK",		{"walk",	0.8, true }	},
		{ "RUN",		{"run",		1.2	}		},
		{ "SPRINT",		{"sprint",	2.0	}		},
	};
}



#endif // !LEMONADE_GAME_SRC_GAIT_H
