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

	if (get_actor() == nullptr || get_subject() == nullptr)
	{
		ACTION_CERR("Actor or target was null!");
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
				grammar::A(n1),
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
				grammar::A(n1),
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
		english_list(taste_descriptions)
	);

	return ActionResult{
		.success = true,
		.responses = {
			fmt::format(
				"{} {} {}.",
				grammar::A(n1),
				(grammar::is_plural(n1) ? "consume" : "consumes"),
				actor->is_player() ? grammar::the(n2) : grammar::a(n2)
			),
			GameMessage(taste, color::get("yellow"))
		}
	};
}