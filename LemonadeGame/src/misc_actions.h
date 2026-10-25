#pragma once

#ifndef LEMONADE_GAME_SRC_MISC_ACTIONS_H
#define LEMONADE_GAME_SRC_MISC_ACTIONS_H

#include "action.h"
#include "transforms.h"

class SwapPlacesAction : public EntityAction
{
protected:
	Entity* m_target;

public:
	SwapPlacesAction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, Entity* target = nullptr) :
		EntityAction(action_length, actor), m_target(target)
	{}

	virtual bool check_action() const override;
	virtual ActionResult perform();
	virtual std::string get_action_name() const override { return "swap places action"; }
};

class GrabAction : public EntityAction
{
protected:
	Entity* m_target;

public:
	GrabAction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, Entity* target = nullptr) :
		EntityAction(action_length, actor), m_target(target)
	{}

	virtual bool check_action() const override;
	virtual ActionResult perform();
	virtual std::string get_action_name() const override { return "grab action"; }
};

class DropAction : public EntityAction
{
protected:
	Entity* m_target;

public:
	DropAction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, Entity* target = nullptr) :
		EntityAction(action_length, actor), m_target(target)
	{}

	virtual bool check_action() const override;
	virtual ActionResult perform();

	virtual std::string get_action_name() const override { return "drop action"; }
};

#endif // !LEMONADE_GAME_SRC_MISC_ACTIONS_H