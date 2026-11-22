#include "combat_actions.h"

bool AutoAttackAction::check_action() const
{
	if (!Interaction::check_action())
		return false;

	if (!get_actor()->has_inventory())
	{
		ACTION_CERR("Subject does not have anatomy!");
		return false;
	}

	if (!get_subject()->has_anatomy())
	{
		ACTION_CERR("Subject does not have anatomy!");
		return false;
	}

	if (!get_subject()->has_living() || get_subject()->living()->is_dead())
	{
		ACTION_CERR("Subject is dead!");
		return false;
	}

	return true;
}

#include "snowy_rng.h"

ActionResult AutoAttackAction::perform()
{
	// Choose weapon

	snowy::rng::WeightedVector<Entity*> weapons;

	for (auto& item : get_actor()->inventory()->get_items())
	{
		if (item->item_def()->is_weapon())
		{
			weapons.push_back(std::tuple(1.0, item));
		}
	}
	Entity* chosen_weapon = nullptr;

	if (!weapons.empty())
	{
		chosen_weapon = snowy::rng::get_weighted_random(weapons, snowy::rng::generator);
	}

	if (!chosen_weapon && !get_actor()->has_species())
	{
		return ActionResult
		{
			.success = false
		};
	}

	// Choose attack

	snowy::rng::WeightedVector<const Attack*> attacks;

	if (chosen_weapon)
	{
		for (auto& attack : chosen_weapon->item_def()->weapon_info.attacks)
		{
			attacks.push_back(std::tuple(
				attack.frequency == AttackFrequency::MAIN ? 10.0 : 1.0,
				&attack
			));
		}
	}
	else
	{
		for (auto& attack : get_actor()->species()->attacks)
		{
			attacks.push_back(std::tuple(
				attack.frequency == AttackFrequency::MAIN ? 10.0 : 1.0,
				&attack
			));
		}
	}

	if (attacks.empty())
	{
		return ActionResult
		{
			.success = false
		};
	}

	auto chosen_attack = snowy::rng::get_weighted_random(attacks, snowy::rng::generator);

	// Choose target part

	snowy::rng::WeightedVector<BodyPartID> body_parts;

	auto anatomy = get_subject()->anatomy();

	for (const auto& bp_ID : anatomy->get_body_part_ids())
	{
		auto bp = anatomy->get_body_part(bp_ID);

		if (bp->is_internal())
			continue;

		body_parts.push_back(std::tuple(bp->get_relative_size() * bp->get_relative_size(), bp_ID));
	}

	if (body_parts.empty())
	{
		return ActionResult
		{
			.success = false
		};
	}

	auto chosen_bp = snowy::rng::get_weighted_random(body_parts, snowy::rng::generator);

	return ActionResult
	{
		.success = true,
		.next = new AttackAction(
			get_actor(), 
			get_subject(), 
			AttackParams
			{
				.attack_info = chosen_attack,
				.target_part = chosen_bp,
				.attack_location = get_subject()->movable()->pos()
			}
		)
	};
}

bool AttackAction::check_action() const
{
	if (!Interaction::check_action())
		return false;

	if (!get_subject()->has_anatomy())
	{
		ACTION_CERR("Subject does not have anatomy!");
		return false;
	}

	if (!get_subject()->anatomy()->has_body_part(m_attack_params.target_part))
	{
		ACTION_CERR("Subject does not have the target part!");
		return false;
	}

	return true;
}

TimeUnit AttackAction::get_action_length() const
{
	return m_attack_params.attack_info->prepare;
}

BodyPart* AttackAction::get_target_part() const
{
	return get_subject()->anatomy()->get_body_part(m_attack_params.target_part);
}

ActionResult AttackAction::perform()
{
	auto actor = get_actor();
	auto subject = get_subject();
	auto target_part = get_target_part();

	auto attack_info = m_attack_params.attack_info;

	auto n1 = actor->display_noun();
	auto n2 = subject->display_noun();
	auto n3 = target_part->display_noun();

	bool is_miss = false;

	subject->on_attacked(actor);

	if (attack_info->range == AttackRange::MELEE)
	{
		if (!(actor->movable()->pos() == subject->movable()->pos() || transforms::is_adjacent(actor->movable()->pos(), subject->movable()->pos())))
		{
			is_miss = true;
		}
		else
		{
			auto actor_agility = actor->living()->get_stat(attributes::AGILITY);
			auto subject_agility = subject->living()->get_stat(attributes::AGILITY);

			auto actor_roll = actor->make_attack_roll(nullptr, attack_info);
			auto subject_roll = subject->make_dodge_roll(attack_info, m_attack_params.attack_location, actor->movable()->pos());

			is_miss = subject_roll >= actor_roll;
		}
	}

	if (is_miss)
	{
		subject->successful_dodge();
		actor->next_attack_position = Position(-1, -1, -1);

		return ActionResult
		{
			.success = false,
			.responses = 
			{
				GameMessage(
					fmt::format(
						"{} {} at {} and {}!",
						subject->is_player() ? grammar::The(n1) : grammar::A(n1),
						grammar::say_regarding(attack_info->attack_verb, n1),
						actor->is_player() ? grammar::the(n2) : grammar::a(n2),
						grammar::say_regarding(Verb{ "misses", "miss" }, n1)
					)
				)
			}
		};
	}

	Damage damage
	{
		.attacker = actor,
		.amount = roll(attack_info->damage_dice, snowy::rng::generator),
		.type = attack_info->damage_type
	};

	target_part->take_damage(damage);

	subject->on_damage_taken(damage);
	actor->add_action_cooldown(attack_info->recover);

	return ActionResult
	{
		.success = true,
		.responses = 
		{
			GameMessage
			{
				snowy::rng::flip_coin() ?
				fmt::format(
					"{} {} {} {} for {} damage!",
					subject->is_player() ? grammar::The(n1) : grammar::A(n1),
					grammar::say_regarding(attack_info->attack_verb, n1),
					grammar::poss(std::string_view(actor->is_player() ? grammar::the(n2) : grammar::a(n2))), grammar::to_string(n3),
					damage.amount
				) :
				fmt::format(
					"{} {} {} in {} for {} damage!",
					subject->is_player() ? grammar::The(n1) : grammar::A(n1),
					grammar::say_regarding(attack_info->attack_verb, n1),
					actor->is_player() ? grammar::the(n2) : grammar::a(n2), grammar::the(n3),
					damage.amount
				),
				actor->is_player() ? color::get("purple") : color::get("red"),
				actor->movable()->pos()
			}
		}
	};
}