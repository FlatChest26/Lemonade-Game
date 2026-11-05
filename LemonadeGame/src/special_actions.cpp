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

	auto n1 = actor->display_noun();

	if (actor->is_player())
	{
		get_world_state()->set_tick_speed(TickSpeed::REAL_TIME_8X);
	}

	return ActionResult{
		.success = true,
		.responses = {
			fmt::format("{} {} asleep.", grammar::A(n1), grammar::is_plural(n1) ? "fall" : "falls")
		}
	};
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

	if (actor->is_player())
	{
		get_world_state()->set_tick_speed(TickSpeed::TURN_BASED);
	}

	auto n1 = actor->display_noun();

	return ActionResult{
		.success = true,
		.responses = {
			fmt::format("{} {} up.", grammar::A(n1), grammar::is_plural(n1) ? "wake" : "wakes")
		}
	};
}

bool DeathAction::check_action() const
{
	if (!EntityAction::check_action())
		return false;

	if (!get_actor()->has_living())
	{
		ACTION_CERR("actor is not a living thing!");
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

	if (actor->is_player())
	{
		get_world_state()->set_tick_speed(TickSpeed::REAL_TIME);
	}

	return ActionResult(true);
}

ActionResult StarveToDeathAction::perform()
{
	DeathAction::perform();

	auto actor = get_actor();
	auto n1 = actor->display_noun();

	return ActionResult
	{
		.success = true,
		.responses = {
			GameMessage(
				fmt::format("{} {} to death.", grammar::A(n1), grammar::is_plural(n1) ? "starve" : "starves"),
				actor->is_player() ? color::get("red") : color::get("light_red")
			)
		}
	};
}

ActionResult DieOfDehydrationAction::perform()
{
	DeathAction::perform();

	auto actor = get_actor();
	auto n1 = actor->display_noun();

	return ActionResult
	{
		.success = true,
		.responses = {
			GameMessage(
				fmt::format("{} {} of dehydration.", grammar::A(n1), grammar::is_plural(n1) ? "die" : "dies"),
				actor->is_player() ? color::get("red") : color::get("light_red")
			)
		}
	};
}