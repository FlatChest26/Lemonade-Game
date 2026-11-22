#pragma once


#ifndef LEMONADE_GAME_SRC_GAME_MESSAGE_H
#define LEMONADE_GAME_SRC_GAME_MESSAGE_H

#include "color.h"
#include "time_units.h"
#include "transforms.h"

struct GameMessage
{
	std::string text = "";
	color_t color = DEFAULT_FG_COLOR;
	Position origin = Position(-1, -1, -1);

	GameMessage(std::string text = "", color_t color = DEFAULT_FG_COLOR, const Position& origin = Position(-1, -1, -1)) :
		text(text), color(color)
	{}

	bool empty() const { return text.empty(); }
	operator std::string() const { return text; }

	bool has_origin() const { return origin != Position(-1, -1, -1); }
};



#endif // !LEMONADE_GAME_SRC_GAME_MESSAGE_H
