#pragma once

#ifndef LEMONADE_GAME_SRC_MISC_ACTIONS_H
#define LEMONADE_GAME_SRC_MISC_ACTIONS_H

#include "action.h"
#include "entity.h"

/* An action between two entities, an actor and a subject. */
class Interaction : public EntityAction
{
protected:
	EntityID m_subject;

public:
	Interaction(const TimeUnit& action_length = TimeUnit::none(), Entity* actor = nullptr, Entity* subject = nullptr) :
		EntityAction(action_length, actor), m_subject(subject->ID())
	{
	}

	void set_subject(Entity* subject) override;
	virtual Entity* get_subject() const override;

	virtual bool check_action() const override;
	static std::string action_name() { return "<unnamed interaction>"; }
};

class SwapPlacesAction : public Interaction
{
public:
	SwapPlacesAction(Entity* actor = nullptr, Entity* subject = nullptr) :
		Interaction(TimeUnit::from_seconds(1.0), actor, subject)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();

	static std::string action_name() { return "swap places"; }
};

class GrabAction : public Interaction
{
public:
	GrabAction(Entity* actor = nullptr, Entity* subject = nullptr) :
		Interaction(TimeUnit::from_seconds(1.0), actor, subject)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();
	static std::string action_name() { return "grab"; }
};

class DropAction : public Interaction
{
public:
	DropAction(Entity* actor = nullptr, Entity* subject = nullptr) :
		Interaction(TimeUnit::from_seconds(1.0), actor, subject)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();

	static std::string action_name() { return "drop"; }
};

class ConsumeAction : public Interaction
{
public:
	ConsumeAction(Entity* actor = nullptr, Entity* subject = nullptr) :
		Interaction(TimeUnit::from_seconds(1.0), actor, subject)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();

	static std::string action_name() { return "consume"; }
};

#endif // !LEMONADE_GAME_SRC_MISC_ACTIONS_H