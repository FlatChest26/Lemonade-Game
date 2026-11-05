#pragma once

#ifndef LEMONADE_GAME_SRC_SPECIAL_ACTIONS_H
#define LEMONADE_GAME_SRC_SPECIAL_ACTIONS_H

#include "interactions.h"

class StartSleepAction : public EntityAction
{
public:
	StartSleepAction(Entity* actor = nullptr) :
		EntityAction(TimeUnit::from_seconds(1.0), actor)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();

	static std::string action_name() { return "go to sleep"; }
};

class EndSleepAction : public EntityAction
{
public:
	EndSleepAction(Entity* actor = nullptr) :
		EntityAction(TimeUnit::from_seconds(1.0), actor)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();

	static std::string action_name() { return "wake up"; }
};

#endif // !LEMONADE_GAME_SRC_SPECIAL_ACTIONS_H
