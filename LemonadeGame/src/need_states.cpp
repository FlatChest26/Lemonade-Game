#include "need_states.h"
#include "entity_filter.h"

#include "ai_handler.h"
#include "interactions.h"
#include "combat_actions.h"

void AIState_Horny::init()
{
	if (auto entity = get_entity())
	{
		entity->say(fmt::format("Man, I'm {}!", entity->living()->get_stat(needs::AROUSAL).description()));
	}
}

bool AIState_Horny::check() const
{
	if (!AIState::check())
		return false;

	if (m_search_attempts < 1)
		return false;

	if (m_still_valid_callback && !m_still_valid_callback())
		return false;

	return true;
}

StateResult AIState_Horny::step(const TimeUnit& time_delta)
{
	run_behaviors(time_delta);

	if (has_behavior())
		return StateResult(StateResultType::NONE);

	auto entity = get_entity();

	if (!m_horny_target)
	{
		add_behavior(new Behavior_SearchForEntity(Behavior_SearchForEntity::initializer{
			.filter = new EntityFilter{ .ignore_ids = { entity->ID() }, .required_flags = EntityFlag::IS_CREATURE },
			.search_radius = 10,
			.is_valid_callback = [entity](Entity* other_entity)
			{
				if (!other_entity->living()->is_adult())
					return false;

				if (!other_entity->species()->is_part_of_society())
					return false;

				if (other_entity->living()->is_unconscious())
					return false;

				Age_t max_age_gap = std::max(10, std::min(entity->living()->get_age(), other_entity->living()->get_age()) / 2);

				if (other_entity->living()->get_age() - entity->living()->get_age() > max_age_gap)
					return false;

				if (!entity->can_see(other_entity))
					return false;

				return true;
			},
			.get_weight_callback = [entity](Entity* other_entity)
			{
				double distance = (entity->movable()->pos() - other_entity->movable()->pos()).length_squared();
				return entity->living()->get_attractiveness(other_entity) / (distance > DBL_EPSILON ? distance : 1);
			},

			.on_found_callback = [this](Entity* selected_entity) { this->set_horny_target(selected_entity); }

		}));

		m_search_attempts--;
	}
	else if (!((entity->movable()->pos() == m_horny_target->movable()->pos()) || transforms::is_adjacent(entity->movable()->pos(), m_horny_target->movable()->pos())))
	{
		add_behavior(new Behavior_GetAdjacentToEntity({ .target = m_horny_target }));
		set_entity_gait_based_on_urgency();
	}
	else
	{
		entity->reset_actions(new KissAction(entity, m_horny_target));
	}

	return StateResult(StateResultType::NONE);
}

void AIState_FindConsumableEffect::init()
{
	if (auto entity = get_entity())
	{
		entity->say(fmt::format("Man, I'm {}!", entity->living()->get_stat(m_stat_id).description()));
	}
}

bool AIState_FindConsumableEffect::check() const
{
	if (!AIState::check())
		return false;

	if (m_search_attempts < 1)
		return false;

	if (m_still_valid_callback && !m_still_valid_callback())
		return false;

	return true;
}

StateResult AIState_FindConsumableEffect::step(const TimeUnit& time_delta)
{
	run_behaviors(time_delta);

	if (has_behavior())
		return StateResult(StateResultType::NONE);

	auto entity = get_entity();
	auto& entity_pos = entity->movable()->pos();

	if (!m_consumable_target)
	{
		add_behavior(new Behavior_SearchForEntity(Behavior_SearchForEntity::initializer{
			.filter = new EntityFilter{ .required_flags = EntityFlag::IS_ITEM },
			.search_radius = 10,
			.is_valid_callback = [entity, this](Entity* item)
			{
				if (!item->has_item_def())
					return false;

				if (!item->item_def()->is_consumable())
					return false;

				if (item->item_def()->consumable_effects.empty())
					return false;

				for (const auto& consumable_effect : item->item_def()->consumable_effects)
				{
					if (consumable_effect.modified_stat == m_stat_id)
						return true;
				}

				if (!entity->can_see(item))
					return false;

				return false;
			},
			.get_weight_callback = [entity, this](Entity* item)
			{
				double distance = (entity->movable()->pos() - item->movable()->pos()).length_squared();
				double replenishment = 0.0;

				for (const auto& consumable_effect : item->item_def()->consumable_effects)
				{
					if (consumable_effect.modified_stat == m_stat_id)
						replenishment += static_cast<double>(consumable_effect.amount);
				}

				return std::abs(replenishment) / (distance > DBL_EPSILON ? distance : 1);
			},
			.on_found_callback = [this](Entity* selected_entity) { this->set_consumable_target(selected_entity); }

		}));

		m_search_attempts--;
	}
	
	else if (entity->inventory()->has_item(m_consumable_target) && entity->reset_actions(new ConsumeAction(entity, m_consumable_target)))
	{
		return StateResult(StateResultType::POP);
	}
	else if (m_consumable_target)
	{
		if (entity_pos != m_consumable_target->movable()->pos())
		{
			add_behavior(new Behavior_GetToEntity({ .target = m_consumable_target }));
			set_entity_gait_based_on_urgency();
		}
		else
		{
			entity->reset_actions(new GrabAction(entity, m_consumable_target));
		}
	}

	return StateResult(StateResultType::NONE);
}

void AIState_Kill::init()
{
	if (auto entity = get_entity())
	{
		entity->say(fmt::format("Prepare to die, {}!", entity->name_for(m_kill_target)));
	}
}

bool AIState_Kill::check() const
{
	if (!AIState::check())
		return false;

	if (m_search_attempts < 1)
		return false;

	return true;
}

StateResult AIState_Kill::step(const TimeUnit& time_delta)
{
	run_behaviors(time_delta);

	if (has_behavior())
		return StateResult(StateResultType::NONE);

	auto entity = get_entity();

	auto& entity_pos = entity->movable()->pos();
	auto& target_pos = m_kill_target->movable()->pos();


	if (m_kill_target->living()->is_dead())
	{
		return StateResult(StateResultType::POP);
	}

	if (m_weapon_target)
	{
		if (entity->inventory()->has_item(m_weapon_target))
		{
			m_weapon_target = nullptr;
		}
		else
		{
			auto weapon_pos = m_weapon_target->movable()->pos();

			if (entity_pos != weapon_pos)
			{
				add_behavior(new Behavior_GetToEntity({ .target = m_weapon_target }));
				set_entity_gait_based_on_urgency();
			}
			else
			{
				entity->reset_actions(new GrabAction(entity, m_weapon_target));
			}

			return StateResult(StateResultType::NONE);
		}
	}

	bool has_weapon = false;

	if (entity->has_species() && !entity->species()->attacks.empty())
	{
		has_weapon = true;
	}
	else
	{
		for (auto& item : entity->inventory()->get_items())
		{
			if (item->item_def()->is_weapon())
			{
				has_weapon = true;
				break;
			}
		}
	}

	if (!has_weapon)
	{
		add_behavior(new Behavior_SearchForEntity(Behavior_SearchForEntity::initializer{
			.filter = new EntityFilter{ .required_flags = EntityFlag::IS_ITEM },
			.search_radius = 10,
			.is_valid_callback = [entity](Entity* item)
			{
				return entity->can_see(item) && item->has_item_def() && item->item_def()->is_weapon();
			},
			.get_weight_callback = [entity](Entity* item)
			{
				double distance = (entity->movable()->pos() - item->movable()->pos()).length_squared();
				return static_cast<double>(entity->view_range() * entity->view_range()) / (distance > DBL_EPSILON ? distance : 1);
			},
			.on_found_callback = [this](Entity* selected_entity) { this->set_weapon_target(selected_entity); }
			}));

		m_search_attempts--;
	}
	
	else if (!((entity_pos == target_pos) || transforms::is_adjacent(entity_pos, target_pos)))
	{
		add_behavior(new Behavior_GetAdjacentToEntity({ .target = m_kill_target }));
		set_entity_gait_based_on_urgency();
	}
	else
	{
		entity->reset_actions(new AutoAttackAction(entity, m_kill_target));
	}

	return StateResult(StateResultType::NONE);
}