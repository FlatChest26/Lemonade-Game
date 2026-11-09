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
	Interaction(Entity* actor, Entity* subject) :
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
	SwapPlacesAction(Entity* actor, Entity* subject) :
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
	GrabAction(Entity* actor, Entity* subject) :
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
	DropAction(Entity* actor, Entity* subject) :
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
	ConsumeAction(Entity* actor, Entity* subject) :
		Interaction(actor, subject)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();

	static std::string action_name() { return "consume"; }
};

class CheckGrappleAction : public Interaction
{
public:
	CheckGrappleAction(Entity* actor, Entity* subject = nullptr) :
		Interaction(actor, subject)
	{
	}

	virtual TimeUnit get_action_length() const override { return TimeUnit::none(); }

	virtual bool check_action() const override;
	virtual ActionResult perform();
	static std::string action_name() { return "check grapple"; }
};

class GrappleAction : public Interaction
{
public:
	GrappleAction(Entity* actor = nullptr, Entity* subject = nullptr) :
		Interaction(actor, subject)
	{
	}

	virtual TimeUnit get_action_length() const override;

	virtual bool check_action() const override;
	virtual ActionResult perform();
	static std::string action_name() { return "grapple"; }
};

class ReleaseGrappleAction : public EntityAction
{
public:
	ReleaseGrappleAction(Entity* actor = nullptr) :
		EntityAction(actor)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();
	static std::string action_name() { return "release grapple"; }
};

class EscapeGrappleAction : public EntityAction
{
public:
	EscapeGrappleAction(Entity* actor = nullptr) :
		EntityAction(actor)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();
	static std::string action_name() { return "escape grapple"; }
};

class KissAction : public Interaction
{
public:
	KissAction(Entity* actor, Entity* subject) :
		Interaction(actor, subject)
	{
	}

	virtual bool check_action() const override;
	virtual ActionResult perform();
	static std::string action_name() { return "kiss other"; }
};


#endif // !LEMONADE_GAME_SRC_MISC_ACTIONS_H