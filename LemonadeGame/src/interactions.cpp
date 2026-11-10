#include "interactions.h"
#include "entity.h"
#include "world.h"

void Interaction::set_subject(Entity* subject)
{
	m_subject = subject;
}

Entity* Interaction::get_subject() const
{
	return m_subject;
}

bool Interaction::check_action() const
{
	if (!EntityAction::check_action())
		return false;

	if (get_subject() == nullptr)
	{
		ACTION_CERR("Subject is null!");
		return false;
	}

	return true;
}

bool SwapPlacesAction::check_action() const
{
	if (!Interaction::check_action())
		return false;

	if (!get_actor()->has_movable() || !get_subject()->has_movable())
	{
		ACTION_CERR("Actor or target does not have movable!");
		return false;
	}

	if (get_actor()->fixed_in_place() || get_subject()->fixed_in_place())
	{
		ACTION_CERR("Actor or target is fixed in place!");
		return false;
	}

	auto world = get_world();

	if (!world)
	{
		ACTION_CERR("World is null!");
		return false;
	}

	if (world->is_blocked(get_subject()->get_movable()->pos(), get_subject(), (get_subject()->get_movable()->posz() - get_actor()->get_movable()->posz())))
	{
		ACTION_CERR("Location is blocked!");
		return false;
	}

	if (world->is_blocked(get_actor()->get_movable()->pos(), get_actor(), (get_actor()->get_movable()->posz() - get_subject()->get_movable()->posz())))
	{
		ACTION_CERR("Location is blocked!");
		return false;
	}

	return true;
}

ActionResult SwapPlacesAction::perform()
{
	Position orig_pos = get_actor()->get_movable()->pos();

	get_actor()->get_movable()->pos() = get_subject()->get_movable()->pos();
	get_subject()->get_movable()->pos() = orig_pos;

	return ActionResult(true);
}

bool GrabAction::check_action() const
{
	if (!Interaction::check_action())
		return false;

	if (!get_actor()->can_hold_items())
	{
		ACTION_CERR("Actor can't hold items!");
		return false;
	}

	if (!get_subject()->can_be_held())
	{
		ACTION_CERR("Target can't be held!");
		return false;
	}

	return true;
}

ActionResult GrabAction::perform()
{
	auto actor = get_actor();
	auto subject = get_subject();

	actor->add_to_inventory(subject);

	auto n1 = actor->display_noun();
	auto n2 = subject->display_noun();

	return ActionResult{
		.success = true,
		.responses = {
			fmt::format(
				"{} {} {}.",
				subject->is_player() ? grammar::The(n1) : grammar::A(n1),
				(grammar::is_plural(n1) ? "pick up" : "picks up"),
				actor->is_player() ? grammar::the(n2) : grammar::a(n2)
			)
		}
	};
}

bool DropAction::check_action() const
{
	if (!Interaction::check_action())
		return false;

	if (get_subject()->get_entity_holder() != get_actor())
	{
		ACTION_CERR("Target is not being held by the actor!");
		return false;
	}

	return true;
}

ActionResult DropAction::perform()
{
	auto actor = get_actor();
	auto subject = get_subject();

	actor->remove_from_inventory(subject);

	auto n1 = actor->display_noun();
	auto n2 = subject->display_noun();

	return ActionResult{
		.success = true,
		.responses = {
			fmt::format(
				"{} {} {}.",
				subject->is_player() ? grammar::The(n1) : grammar::A(n1),
				(grammar::is_plural(n1) ? "drop" : "drops"),
				actor->is_player() ? grammar::the(n2) : grammar::a(n2)
			)
		}
	};
}

bool ConsumeAction::check_action() const
{
	if (!Interaction::check_action())
		return false;

	if (!get_actor()->has_living())
	{
		ACTION_CERR("Actor is not a living thing!");
		return false;
	}

	if (!get_subject()->is_item())
	{
		ACTION_CERR("Target is not an item!");
		return false;
	}

	if (!get_subject()->get_item_def()->is_consumable())
	{
		ACTION_CERR("Target item is not consumable!");
		return false;
	}

	return true;
}

ActionResult ConsumeAction::perform()
{
	auto actor = get_actor();
	auto subject = get_subject();

	std::vector<std::string> taste_descriptions;

	auto living = actor->get_living();

	for (auto& effect : subject->get_item_def()->consumable_effects)
	{
		if (!living->take_consumable(effect))
			continue;
		taste_descriptions.push_back(effect.taste);
	}

	actor->remove_from_inventory(subject);
	subject->queue_remove();

	auto n1 = actor->display_noun();
	auto n2 = subject->display_noun();

	std::string taste = fmt::format(
		"It tastes {}!",
		grammar::english_list(taste_descriptions)
	);

	return ActionResult{
		.success = true,
		.responses = {
			fmt::format(
				"{} {} {}.",
				subject->is_player() ? grammar::The(n1) : grammar::A(n1),
				(grammar::is_plural(n1) ? "consume" : "consumes"),
				actor->is_player() ? grammar::the(n2) : grammar::a(n2)
			),
			GameMessage(taste, color::get("yellow"))
		}
	};
}

bool CheckGrappleAction::check_action() const
{
	if (!EntityAction::check_action())
		return false;

	return true;
}

ActionResult CheckGrappleAction::perform()
{
	if (get_actor()->is_grappling_other())
	{
		return ActionResult
		{
			.success = true,
			.next = new ReleaseGrappleAction(get_actor())
		};
	}

	if (get_actor()->is_being_grappled())
	{
		return ActionResult
		{
			.success = true,
			.next = new EscapeGrappleAction(get_actor())
		};
	}

	if (has_subject())
	{
		return ActionResult
		{
			.success = true,
			.next = new GrappleAction(get_actor(), get_subject())
		};
	}

	return ActionResult(false);
}

TimeUnit GrappleAction::get_action_length() const
{
	return TimeUnit::from_seconds(0.5) / get_actor()->interact_speed();
}

bool GrappleAction::check_action() const
{
	if (!Interaction::check_action())
		return false;

	if (get_actor()->is_grappling_other())
	{
		ACTION_CERR("Actor is already grappling someone! " << get_actor()->ID());
		return false;
	}

	if (!get_subject()->can_be_grappled())
	{
		ACTION_CERR("Subject cannot be grappled! " << get_subject()->ID());
		return false;
	}

	if (get_actor()->has_anatomy() && !get_actor()->get_anatomy()->has_body_parts(BodyPartFilter{ .required_flags = BodyPartFlag::GRASP }))
	{
		ACTION_CERR("Actor cannot grapple others! " << get_actor()->ID());
		return false;
	}

	if (get_actor()->has_grapple() && get_actor()->get_grapple()->target == get_subject())
	{
		ACTION_CERR("Actor is already grappling the target! " << get_actor()->ID());
		return false;
	}

	return true;
}

ActionResult GrappleAction::perform()
{
	auto actor = get_actor();
	auto subject = get_subject();

	actor->grapple_other(subject);

	auto n1 = actor->display_noun();
	auto n2 = subject->display_noun();

	return ActionResult
	{
		.success = true,
		.responses = {
			fmt::format(
				"{} {} {}.",
				subject->is_player() ? grammar::The(n1) : grammar::A(n1),
				(grammar::is_plural(n1) ? "grapple" : "grapples"),
				actor->is_player() ? grammar::the(n2) : grammar::a(n2)
			)
		}
	};
}

bool ReleaseGrappleAction::check_action() const
{
	if (!EntityAction::check_action())
		return false;

	if (!get_actor()->is_grappling_other())
	{
		ACTION_CERR("Actor is not grappling anyone! " << get_actor()->ID());
		return false;
	}

	return true;
}

ActionResult ReleaseGrappleAction::perform()
{
	auto actor = get_actor();
	auto subject = actor->get_grapple_target();

	actor->release_grapple();

	auto n1 = actor->display_noun();
	auto n2 = subject->display_noun();

	return ActionResult
	{
		.success = true,
		.responses = {
			fmt::format(
				"{} {} go of {}.",
				subject->is_player() ? grammar::The(n1) : grammar::A(n1),
				(grammar::is_plural(n1) ? "let" : "lets"),
				actor->is_player() ? grammar::the(n2) : grammar::a(n2)
			)
		}
	};
}

bool EscapeGrappleAction::check_action() const
{
	if (!EntityAction::check_action())
		return false;

	if (!get_actor()->is_being_grappled())
	{
		ACTION_CERR("Actor is not being grappled! " << get_actor()->ID());
		return false;
	}

	return true;
}

ActionResult EscapeGrappleAction::perform()
{
	auto actor = get_actor();
	auto subject = actor->get_grapple_owner();

	actor->escape_grapple();

	auto n1 = actor->display_noun();
	auto n2 = subject->display_noun();

	return ActionResult
	{
		.success = true,
		.responses = {
			fmt::format(
				"{} {} away from {}.",
				subject->is_player() ? grammar::The(n1) : grammar::A(n1),
				(grammar::is_plural(n1) ? "break" : "breaks"),
				actor->is_player() ? grammar::the(n2) : grammar::a(n2)
			)
		}
	};
}


bool KissAction::check_action() const
{
	if (!Interaction::check_action())
		return false;

	if (!get_actor()->is_animate())
	{
		ACTION_CERR("Actor is inanimate!" << get_actor()->ID());
		return false;
	}

	if (!get_subject()->is_animate())
	{
		ACTION_CERR("Subject is inanimate!" << get_subject()->ID());
		return false;
	}

	if (!transforms::is_adjacent(get_actor()->get_movable()->pos(), get_subject()->get_movable()->pos()))
	{
		ACTION_CERR("Actor and subject are not close enough!" << get_actor()->ID() << " " << get_subject()->ID());
		return false;
	}

	return true;
}

ActionResult KissAction::perform()
{
	auto actor = get_actor();
	auto subject = get_subject();

	if (actor->has_living() && actor->get_living()->has_stat(needs::AROUSAL))
	{
		actor->get_living()->get_stat(needs::AROUSAL).add_value(12);
	}

	if (subject->has_living() && subject->get_living()->has_stat(needs::AROUSAL))
	{
		subject->get_living()->get_stat(needs::AROUSAL).add_value(12);
	}

	auto n1 = actor->display_noun();
	auto n2 = subject->display_noun();

	return ActionResult
	{
		.success = true,
		.responses = {
			fmt::format(
				"{} {} {}.",
				subject->is_player() ? grammar::The(n1) : grammar::A(n1),
				(grammar::is_plural(n1) ? "kiss" : "kisses"),
				actor->is_player() ? grammar::the(n2) : grammar::a(n2)
			)
		}
	};
}