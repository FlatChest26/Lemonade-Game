#pragma once

#ifndef LEMONADE_GAME_SRC_COMBAT_ACTIONS_H
#define LEMONADE_GAME_SRC_COMBAT_ACTIONS_H

#include "interactions.h"
#include "weapon_info.h"

struct AttackParams
{
	Entity* weapon;
	const Attack* attack_info;
	BodyPartID target_part;
};

class AutoAttackAction : public Interaction
{
public:
	AutoAttackAction(Entity* actor, Entity* subject) :
		Interaction(actor, subject)
	{
	}

	virtual TimeUnit get_action_length() const override { return TimeUnit::none(); }
	static std::string action_name() { return "auto-attack"; }

	virtual bool check_action() const override;
	virtual ActionResult perform();
};

class AttackAction : public Interaction
{
protected:
	AttackParams m_attack_params;

public:
	AttackAction(Entity* actor, Entity* subject, AttackParams attack_params) :
		Interaction(actor, subject), m_attack_params(attack_params)
	{}

	BodyPart* get_target_part() const;

	static std::string action_name() { return "attack"; }

	virtual TimeUnit get_action_length() const override;
	virtual bool check_action() const override;
	virtual ActionResult perform();
};



#endif // !LEMONADE_GAME_SRC_COMBAT_ACTIONS_H
