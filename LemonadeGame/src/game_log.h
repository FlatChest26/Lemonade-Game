#pragma once

#ifndef LEMONADE_GAME_SRC_GAME_LOG_H
#define LEMONADE_GAME_SRC_GAME_LOG_H

#include "render_params.h"
#include "time_units.h"

#include "world.h"
#include <string>
#include <vector>

class WorldGameState;

struct GameMessage
{
	std::string text = "";
	color_t fg = DEFAULT_FG_COLOR;
	color_t bg = DEFAULT_BG_COLOR;
	TimeUnit time_point;

	int repeat_count = 1;

	GameMessage(std::string msg = "", color_t fg = DEFAULT_FG_COLOR, color_t bg = DEFAULT_BG_COLOR, const TimeUnit& time_point = TimeUnit::none()) :
		text(msg), fg(fg), bg(bg), time_point(time_point), repeat_count(1)
	{
	}

	bool empty() const { return text.empty(); }
	operator std::string() const { return text; }
};

constexpr size_t MAX_HISTORY_LENGTH = 256;

class GameLog
{
protected:
	WorldGameState* m_world_state;
	std::vector<GameMessage> m_messages;
	size_t m_history_length{ MAX_HISTORY_LENGTH };

public:
	GameLog(WorldGameState* world_state = nullptr);

	WorldGameState* get_world_state() const;
	World* get_world() const;

public:
	// From most to least recent
	GameMessage& get_message(size_t idx)
	{
		size_t index = m_messages.size() - (idx + 1);

		if (index >= m_messages.size())
		{
			CERR("Out of bounds access!");
		}

		return m_messages[index];
	}

	const GameMessage& get_message(size_t idx) const
	{
		size_t index = m_messages.size() - (idx + 1);

		if (index >= m_messages.size())
		{
			CERR("Out of bounds access!");
		}

		return m_messages[index];
	}

	void add_message(const GameMessage& msg)
	{
		if (m_messages.size() > 0 && get_message(0).text == msg.text)
		{
			auto& old_message = get_message(0);
			old_message.repeat_count += 1;
		}
		else
		{
			m_messages.push_back(msg);

			if (m_messages.size() >= m_history_length)
			{
				m_messages.erase(m_messages.begin());
			}
		}
	}

	void render(RenderParams params) const;
};

#endif // !LEMONADE_GAME_SRC_GAME_LOG_H
