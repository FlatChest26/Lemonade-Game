#include "interactions.h"
#include "entity.h"
#include "world.h"

Entity* Interaction::get_subject() const
{
	return m_subject;
}

bool Interaction::check_action() const
{
	if (!EntityAction::check_action())
		return false;

	if (m_actor == nullptr || m_subject == nullptr)
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

	if (!m_actor->has_movable() || !m_subject->has_movable())
	{
		ACTION_CERR("Actor or target does not have movable!");
		return false;
	}

	if (m_actor->fixed_in_place() || m_subject->fixed_in_place())
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

	if (world->is_blocked(m_subject->get_movable()->pos(), m_subject, (m_subject->get_movable()->posz() - m_actor->get_movable()->posz())))
	{
		ACTION_CERR("Location is blocked!");
		return false;
	}

	if (world->is_blocked(m_actor->get_movable()->pos(), m_actor, (m_actor->get_movable()->posz() - m_subject->get_movable()->posz())))
	{
		ACTION_CERR("Location is blocked!");
		return false;
	}

	return true;
}

ActionResult SwapPlacesAction::perform()
{
	Position orig_pos = m_actor->get_movable()->pos();

	m_actor->get_movable()->pos() = m_subject->get_movable()->pos();
	m_subject->get_movable()->pos() = orig_pos;

	return ActionResult(true);
}

bool GrabAction::check_action() const
{
	if (!Interaction::check_action())
		return false;

	if (!m_actor->can_hold_items())
	{
		ACTION_CERR("Actor can't hold items!");
		return false;
	}

	if (!m_subject->can_be_held())
	{
		ACTION_CERR("Target can't be held!");
		return false;
	}

	return true;
}

ActionResult GrabAction::perform()
{
	m_actor->add_to_inventory(m_subject);

	auto n1 = m_actor->display_noun();
	auto n2 = m_subject->display_noun();

	return ActionResult(true, fmt::format(
		"{} {} {}.",
		grammar::A(n1),
		(grammar::is_plural(n1) ? "pick up" : "picks up"),
		m_actor->is_player() ? grammar::the(n2) : grammar::a(n2)
	));
}

bool DropAction::check_action() const
{
	if (!Interaction::check_action())
		return false;

	if (m_subject->get_entity_holder() != m_actor)
	{
		ACTION_CERR("Target is not being held by the actor!");
		return false;
	}

	return true;
}

ActionResult DropAction::perform()
{
	m_actor->remove_from_inventory(m_subject);

	auto n1 = m_actor->display_noun();
	auto n2 = m_subject->display_noun();

	return ActionResult(true, fmt::format(
		"{} {} {}.",
		grammar::A(n1),
		(grammar::is_plural(n1) ? "drop" : "drops"),
		m_actor->is_player() ? grammar::the(n2) : grammar::a(n2)
	));
}