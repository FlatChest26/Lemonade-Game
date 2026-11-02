#pragma once


#ifndef LEMONADE_GAME_SRC_GAME_MESSAGE_H
#define LEMONADE_GAME_SRC_GAME_MESSAGE_H

#include "color.h"
#include "time_units.h"


struct GameMessage
{
	std::string text = "";
	color_t color = DEFAULT_FG_COLOR;

	GameMessage(std::string text = "", color_t color = DEFAULT_FG_COLOR) :
		text(text), color(color)
	{}

	bool empty() const { return text.empty(); }
	operator std::string() const { return text; }
};



#endif // !LEMONADE_GAME_SRC_GAME_MESSAGE_H
