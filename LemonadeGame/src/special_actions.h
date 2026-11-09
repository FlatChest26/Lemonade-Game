#pragma once

#ifndef LEMONADE_GAME_SRC_SPECIAL_ACTIONS_H
#define LEMONADE_GAME_SRC_SPECIAL_ACTIONS_H

#include "interactions.h"

class StartSleepAction : public EntityAction
{
public:
	StartSleepAction(Entity* actor) :
		EntityAction(actor)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();

	static std::string action_name() { return "go to sleep"; }
};

class EndSleepAction : public EntityAction
{
public:
	EndSleepAction(Entity* actor) :
		EntityAction(actor)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();

	static std::string action_name() { return "wake up"; }
};

class DeathAction : public EntityAction
{
public:
	DeathAction(Entity* actor) :
		EntityAction(actor)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();

	static std::string action_name() { return "die"; }
};

class StarveToDeathAction : public DeathAction
{
public:
	StarveToDeathAction(Entity* actor) :
		DeathAction(actor)
	{
	}

	virtual ActionResult perform();

	static std::string action_name() { return "starve to death"; }
};

class DieOfDehydrationAction : public DeathAction
{
public:
	DieOfDehydrationAction(Entity* actor) :
		DeathAction(actor)
	{
	}

	virtual ActionResult perform();

	static std::string action_name() { return "die of dehydration"; }
};

#endif // !LEMONADE_GAME_SRC_SPECIAL_ACTIONS_H
