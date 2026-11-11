#pragma once

#ifndef LEMONADE_GAME_SRC_SPECIAL_ACTIONS_H
#define LEMONADE_GAME_SRC_SPECIAL_ACTIONS_H

#include "interactions.h"

#include "posture.h"
#include "gait.h"

class StartSleepAction : public EntityAction
{
public:
	StartSleepAction(Entity* actor) :
		EntityAction(actor)
	{
	}

	virtual TimeUnit get_action_length() const override { return TimeUnit::none(); }

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

	virtual TimeUnit get_action_length() const override { return TimeUnit::none(); }

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

	virtual TimeUnit get_action_length() const override { return TimeUnit::none(); }

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

	virtual TimeUnit get_action_length() const override { return TimeUnit::none(); }

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

	virtual TimeUnit get_action_length() const override { return TimeUnit::none(); }

	virtual ActionResult perform();

	static std::string action_name() { return "die of dehydration"; }
};

struct PostureActionParams
{
	PostureID next_posture;
};

class SwitchPostureAction : public EntityAction
{
protected:
	PostureActionParams m_params;

public:
	SwitchPostureAction(Entity* actor, const PostureActionParams& params) :
		EntityAction(actor), m_params(params)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();

	static std::string action_name() { return "switch posture"; }
};

#endif // !LEMONADE_GAME_SRC_SPECIAL_ACTIONS_H
