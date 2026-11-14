#pragma once

#ifndef LEMONADE_GAME_SRC_POSTURE_H
#define LEMONADE_GAME_SRC_POSTURE_H

#include "transforms.h"
#include "snowy_grammar.h"

#include "body_part.h"

enum class PostureType : int8_t
{
	STAND,
	CRAWL,
};

inline std::map<std::string, PostureType> AllPostureTypes
{
	{ "STAND", PostureType::STAND },
	{ "CRAWL", PostureType::CRAWL },
};

using PostureID = std::string;

struct PostureDef
{
	PostureID ID;

	Verb verb{ "<unnamed posture>" };

	PostureType type{ PostureType::STAND };

	double height{ 1.0 };

	bool directional_verb{ false };

	BodyPartID on_bp{ "" };
	Noun on_noun{ "" };

	bool is_default{ false };
	bool fallen_over{ false };

	static RawObjectChecker* get_checker();
	static PostureDef from_json(const json& j);
};

using PostureMap = std::unordered_map<PostureID, PostureDef>;

class Posture
{
protected:
	PostureMap m_postures;
	PostureID m_current;

public:
	Posture(const PostureMap& postures = {}, const PostureID& current = "") :
		m_postures(postures), m_current(current)
	{
		if (m_current.empty())
		{
			m_current = get_default_posture_id();
		}
	}

	std::vector<const PostureDef*> get_postures() const
	{
		std::vector<const PostureDef*> postures;

		for (auto& [posture_id, posture_type] : m_postures)
		{
			postures.push_back(&posture_type);
		}

		return postures;
	}

	bool set_current_posture(const PostureID& posture_id)
	{
		if (!m_postures.contains(posture_id))
		{
			CERR(posture_id << " is not a valid posture id!");
			return false;
		}

		m_current = posture_id;
		return true;
	}

	const PostureDef* get_current_posture() const
	{
		return get_posture(m_current);
	}

	const PostureDef* get_posture(const PostureID& posture_id) const
	{
		if (m_postures.empty() || posture_id.empty() || !m_postures.contains(posture_id))
		{
			return nullptr;
		}

		return &m_postures.at(posture_id);
	}

	PostureID get_default_posture_id(const PostureType& posture_type = PostureType::STAND) const
	{
		for (auto& [posture_id, posture_def] : m_postures)
		{
			if (posture_def.type != posture_type)
				continue;

			if (posture_def.is_default)
			{
				return posture_id;
			}
		}

		return "stand";
	}

	PostureID get_fallen_over_id(const PostureType& posture_type = PostureType::STAND) const
	{
		for (auto& [posture_id, posture_def] : m_postures)
		{
			if (posture_def.type != posture_type)
				continue;

			if (posture_def.fallen_over)
			{
				return posture_id;
			}
		}

		return "prone";
	}

	bool is_standing() const { return get_current_posture()->type == PostureType::STAND; }
	bool is_crawling() const { return get_current_posture()->type == PostureType::CRAWL; }
};

namespace postures
{
	inline PostureMap DEFAULT_POSTURES
	{
		{ "stand",	PostureDef{ .ID = "standing", .verb = { "stand", "stands" }, .type = PostureType::STAND, .height = 1.0, .is_default = true } },
		{ "sit",	PostureDef{ .ID = "sitting", .verb = { "sits", "sit" }, .type = PostureType::CRAWL, .height = 0.4, .is_default = true } },
		{ "prone",  PostureDef{ .ID = "prone", .verb = { "lies", "lie" }, .type = PostureType::CRAWL, .height = 0.3, .fallen_over = true } },
	
	};
}


#endif // !LEMONADE_GAME_SRC_POSTURE_H
