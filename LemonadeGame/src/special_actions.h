#pragma once

#ifndef LEMONADE_GAME_SRC_SPECIAL_ACTIONS_H
#define LEMONADE_GAME_SRC_SPECIAL_ACTIONS_H

#include "interactions.h"

class StartSleepAction : public EntityAction
{
public:
	StartSleepAction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr) :
		EntityAction(action_length, actor)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();

	static std::string action_name() { return "go to sleep"; }
};

class EndSleepAction : public EntityAction
{
public:
	EndSleepAction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr) :
		EntityAction(action_length, actor)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();

	static std::string action_name() { return "wake up"; }
};

#endif // !LEMONADE_GAME_SRC_SPECIAL_ACTIONS_H
