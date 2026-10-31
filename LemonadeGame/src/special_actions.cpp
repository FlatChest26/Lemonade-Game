#include "special_actions.h"
#include "entity.h"
#include "lemonade_game.h"
#include "world_state.h"

bool StartSleepAction::check_action() const
{
	if (!EntityAction::check_action())
		return false;

	if (!m_actor->is_person())
	{
		ACTION_CERR("Actor is not a person (and thus cannot sleep)! " << m_actor->ID());
		return false;
	}

	if (m_actor->get_person()->is_asleep())
	{
		ACTION_CERR("Actor is already asleep! " << m_actor->ID());
		return false;
	}

	return true;
}

ActionResult StartSleepAction::perform()
{
	m_actor->get_person()->fall_asleep();

	auto n1 = m_actor->display_noun();

	if (m_actor->is_player())
	{
		get_world_game_state()->set_tick_speed(TickSpeed::REAL_TIME_8X);
	}

	return ActionResult(true, fmt::format("{} {} asleep.", grammar::A(n1), grammar::is_plural(n1) ? "fall" : "falls"));
}

bool EndSleepAction::check_action() const
{
	if (!EntityAction::check_action())
		return false;

	if (!m_actor->is_person())
	{
		ACTION_CERR("Actor is not a person (and thus cannot wake up)! " << m_actor->ID());
		return false;
	}

	if (m_actor->get_person()->is_awake())
	{
		ACTION_CERR("Actor is awake! " << m_actor->ID());
		return false;
	}

	return true;
}

ActionResult EndSleepAction::perform()
{
	m_actor->get_person()->wake_up();

	auto n1 = m_actor->display_noun();

	if (m_actor->is_player())
	{
		get_world_game_state()->set_tick_speed(TickSpeed::TURN_BASED);
	}

	return ActionResult(true, fmt::format("{} {} up.", grammar::A(n1), grammar::is_plural(n1) ? "wake" : "wakes"));
}