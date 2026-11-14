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

	if (get_actor()->living()->is_asleep())
	{
		ACTION_CERR("Actor is already asleep! " << get_actor()->ID());
		return false;
	}

	return true;
}

ActionResult StartSleepAction::perform()
{
	auto actor = get_actor();
	actor->living()->fall_asleep();

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

	if (get_actor()->living()->is_awake())
	{
		ACTION_CERR("Actor is awake! " << get_actor()->ID());
		return false;
	}

	return true;
}

ActionResult EndSleepAction::perform()
{
	auto actor = get_actor();
	actor->living()->wake_up();

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

	if (get_actor()->living()->is_dead())
	{
		ACTION_CERR("Actor is already dead!");
		return false;
	}

	return true;
}

ActionResult DeathAction::perform()
{
	auto actor = get_actor();
	actor->living()->die();

	return ActionResult(true);
}

ActionResult StarveToDeathAction::perform()
{
	auto actor = get_actor();
	actor->living()->starve_to_death();

	return ActionResult(true);
}

ActionResult DieOfDehydrationAction::perform()
{
	auto actor = get_actor();
	actor->living()->die_of_dehydration();

	return ActionResult(true);
}

bool SwitchPostureAction::check_action() const
{
	if (!EntityAction::check_action())
		return false;

	if (!get_actor()->has_anatomy())
	{
		ACTION_CERR("Actor does not have anatomy!");
		return false;
	}

	return true;
}

ActionResult SwitchPostureAction::perform()
{
	auto actor = get_actor();
	auto& posture = actor->anatomy()->posture();
	auto n1 = actor->display_noun();

	auto new_posture = posture.get_posture(m_params.next_posture);

	if (!actor->anatomy()->can_stand() && new_posture->type == PostureType::STAND)
	{
		return ActionResult
		{
			.success = false,
			.responses = {
				actor->is_player() ? 
				GameMessage(fmt::format(
					"{} can no longer stand!",
					grammar::The(n1)
				), 
					color::get("light_red")
				) 
				: GameMessage()
			}
		};
	}

	double old_height = posture.get_current_posture()->height;

	if (!posture.set_current_posture(m_params.next_posture))
	{
		return ActionResult(false);
	}

	double new_height = posture.get_current_posture()->height;

	

	auto verb = grammar::say_regarding(new_posture->verb, n1);

	if (new_posture->directional_verb && std::fabs(new_height - old_height) > DBL_EPSILON)
	{
		verb += fmt::format("{}", new_height > old_height ? " up" : " down");
	}

	if (!new_posture->on_noun.empty())
	{
		verb += fmt::format(" on {} {}", grammar::their(actor), new_posture->on_noun.singular);
	}
	else if (!new_posture->on_bp.empty())
	{
		auto bp = actor->anatomy()->get_body_part(new_posture->on_bp);

		if (bp)
		{
			verb += fmt::format(" on {} {}", grammar::their(actor), grammar::to_string(bp->display_noun()));
		}
	}

	if (actor->has_gait())
	{
		auto current_gait = actor->gait()->get_current_gait();

		if (current_gait && current_gait->posture_type != new_posture->type)
		{
			actor->gait()->set_current_gait(actor->gait()->get_default_gait_name(new_posture->type));
		}
	}

	return ActionResult{
		.success = true,
		.responses = {
			fmt::format(
				"{} {}.", 
				grammar::The(n1),
				verb
			),
		}
	
	};
}