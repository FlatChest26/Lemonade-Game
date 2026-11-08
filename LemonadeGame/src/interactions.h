#pragma once

#ifndef LEMONADE_GAME_SRC_MISC_ACTIONS_H
#define LEMONADE_GAME_SRC_MISC_ACTIONS_H

#include "action.h"
#include "entity.h"

/* An action between two entities, an actor and a subject. */
class Interaction : public EntityAction
{
protected:
	Entity* m_subject;

public:
	Interaction(Entity* actor = nullptr, Entity* subject = nullptr) :
		EntityAction(actor), m_subject(subject)
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
		Interaction(actor, subject)
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
		Interaction(actor, subject)
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
		Interaction(actor, subject)
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
		Interaction(actor, subject)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();

	static std::string action_name() { return "consume"; }
};

#endif // !LEMONADE_GAME_SRC_MISC_ACTIONS_H