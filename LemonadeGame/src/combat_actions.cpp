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

	if (!get_subject()->has_living() || get_subject()->get_living()->is_dead())
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

	for (auto& item : get_actor()->get_inventory()->get_items())
	{
		if (item->get_item_def()->has_weapon_info())
		{
			weapons.push_back(std::tuple(1.0, item));
		}
	}

	if (weapons.empty())
	{
		return ActionResult
		{
			.success = false
		};
	}

	auto chosen_weapon = snowy::rng::get_weighted_random(weapons, snowy::rng::generator);

	// Choose attack

	snowy::rng::WeightedVector<const Attack*> attacks;

	for (auto& attack : chosen_weapon->get_item_def()->weapon_info.attacks)
	{
		attacks.push_back(std::tuple(
			attack.frequency == AttackFrequency::MAIN ? 10.0 : 5.0,
			&attack
		));
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

	auto anatomy = get_subject()->get_anatomy();

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
				.weapon = chosen_weapon,
				.attack_info = chosen_attack,
				.target_part = chosen_bp
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

	if (!get_subject()->get_anatomy()->has_body_part(m_attack_params.target_part))
	{
		ACTION_CERR("Subject does not have the target part!");
		return false;
	}

	if (m_attack_params.weapon && !get_actor()->get_inventory()->has_item(m_attack_params.weapon))
	{
		ACTION_CERR("Actor does not have the weapon!");
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
	return get_subject()->get_anatomy()->get_body_part(m_attack_params.target_part);
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

	if (attack_info->range == AttackRange::MELEE)
	{
		if (!(actor->get_movable()->pos() == subject->get_movable()->pos() || transforms::is_adjacent(actor->get_movable()->pos(), subject->get_movable()->pos())))
		{
			is_miss = true;
		}
		else
		{
			auto actor_agility = actor->get_living()->get_stat(attributes::AGILITY);
			auto subject_agility = subject->get_living()->get_stat(attributes::AGILITY);

			auto actor_roll = roll(1_d20, snowy::rng::generator) + actor_agility.get_value();
			auto subject_roll = roll(1_d20, snowy::rng::generator) + subject_agility.get_value();

			is_miss = actor_roll < subject_roll;
		}
	}

	if (is_miss)
	{
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
						grammar::say_regarding(Verb{ "miss", "misses" }, n1)
					)
				)
			}
		};
	}

	Damage damage;

	damage.amount = roll(attack_info->damage_dice, snowy::rng::generator);
	damage.type = attack_info->damage_type;

	target_part->take_damage(damage);

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
				actor->is_player() ? color::get("purple") : color::get("light_red")
			}
		}
	};
}