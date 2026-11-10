#include "special_actions.h"
#include "entity.h"
#include "lemonade_game.h"
#include "world_state.h"

bool StartSleepAction::check_action() const
{
	if (!EntityAction::check_action())
		return false;

	if (!get_actor()->has_living())
	{
		ACTION_CERR("Actor is not a living thing (and thus cannot sleep)! " << get_actor()->ID());
		return false;
	}

	if (get_actor()->get_living()->is_asleep())
	{
		ACTION_CERR("Actor is already asleep! " << get_actor()->ID());
		return false;
	}

	return true;
}

ActionResult StartSleepAction::perform()
{
	auto actor = get_actor();
	actor->get_living()->fall_asleep();

	return ActionResult(true);
}

bool EndSleepAction::check_action() const
{
	if (!EntityAction::check_action())
		return false;

	if (!get_actor()->has_living())
	{
		ACTION_CERR("Actor is not a living thing (and thus cannot wake up)! " << get_actor()->ID());
		return false;
	}

	if (get_actor()->get_living()->is_awake())
	{
		ACTION_CERR("Actor is awake! " << get_actor()->ID());
		return false;
	}

	return true;
}

ActionResult EndSleepAction::perform()
{
	auto actor = get_actor();
	actor->get_living()->wake_up();

	return ActionResult(true);
}

bool DeathAction::check_action() const
{
	if (!EntityAction::check_action())
		return false;

	if (!get_actor()->has_living())
	{
		ACTION_CERR("Actor is not a living thing!");
		return false;
	}

	if (get_actor()->get_living()->is_dead())
	{
		ACTION_CERR("Actor is already dead!");
		return false;
	}

	return true;
}

ActionResult DeathAction::perform()
{
	auto actor = get_actor();
	actor->get_living()->die();

	return ActionResult(true);
}

ActionResult StarveToDeathAction::perform()
{
	auto actor = get_actor();
	actor->get_living()->starve_to_death();

	return ActionResult(true);
}

ActionResult DieOfDehydrationAction::perform()
{
	auto actor = get_actor();
	actor->get_living()->die_of_dehydration();

	return ActionResult(true);
}