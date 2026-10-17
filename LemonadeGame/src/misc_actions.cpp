#include "misc_actions.h"
#include "entity.h"
#include "world.h"

using namespace std;

bool SwapPlacesAction::check_action()
{
	if (!Action::check_action())
		return false;

	if (m_actor == nullptr || target == nullptr)
	{
		CERR("Actor or target was null!");
		return false;
	}

	if (!m_actor->has_movable() || !target->has_movable())
	{
		CERR("Actor or target does not have movable!");
		return false;
	}

	if (m_actor->fixed_in_place() || target->fixed_in_place())
	{
		CERR("Actor or target is fixed in place!");
		return false;
	}

	if (auto world = get_world())
	{
		if (world->is_blocked(target->get_movable()->pos(), target, (target->get_movable()->posz() - m_actor->get_movable()->posz())))
		{
			CERR("Location is blocked!");
			return false;
		}

		if (world->is_blocked(m_actor->get_movable()->pos(), m_actor, (m_actor->get_movable()->posz() - target->get_movable()->posz())))
		{
			CERR("Location is blocked!");
			return false;
		}
	}

	return true;
}

ActionResult SwapPlacesAction::perform()
{
	Position orig_pos = get_actor()->get_movable()->pos();
	m_actor->get_movable()->pos() = target->get_movable()->pos();
	target->get_movable()->pos() = orig_pos;

	return ActionResult{ true, fmt::format("{} moves past {}.", m_actor->get_display_name(), target->get_display_name()) };
}