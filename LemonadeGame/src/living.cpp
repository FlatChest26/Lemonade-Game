#include "living.h"
#include "entity.h"

#include "lemonade_game.h"
#include "game_settings.h"
#include "consumable.h"
#include "special_actions.h"
#include "world_state.h"


StatInt_t Stat::get_value() const
{
	return std::clamp<StatInt_t>(m_value, m_info.min_value, m_info.max_value);
}

void Stat::set_value(StatInt_t value)
{
	m_value = std::clamp<StatInt_t>(value, m_info.min_value, m_info.max_value);
}

void Stat::add_value(StatInt_t value)
{
	m_value += value;
	m_value = std::clamp<StatInt_t>(m_value, m_info.min_value, m_info.max_value);
}

std::string Stat::name() const
{
	return get_value() >= 0 ? m_info.positive_name : m_info.negative_name;
}

std::string Stat::to_string() const
{
	return fmt::format("{} {}", std::to_string(get_value()), name());
}

std::string Stat::description() const
{
	for (const auto& stat_desc : m_info.description)
	{
		if (stat_desc.min <= get_value() && get_value() < stat_desc.max)
		{
			return stat_desc.description;
		}
	}

	return "<invalid description>";
}

color_t Stat::description_color() const
{
	for (const auto& stat_desc : m_info.description)
	{
		if (stat_desc.min <= get_value() && get_value() < stat_desc.max)
		{
			return stat_desc.color;
		}
	}

	return color::get("gray");
}

Living::Living(Entity* owner, Age current_age, Age total_lifespan) :
	m_owner(owner), m_current_age(current_age), m_total_lifespan(total_lifespan)
{
	if (m_owner->has_anatomy())
	{
		m_required_important_parts = m_owner->get_anatomy()->count_body_parts(BodyPartFilter{ .required_flags = BodyPartFlag::IMPORTANT });
	}
}

bool Living::is_young() const
{
	if (auto species = m_owner->get_species())
	{
		return get_age() < species->mature_age;
	}

	return false;
}

bool Living::is_mature() const
{
	if (auto species = m_owner->get_species())
	{
		return get_age() >= species->mature_age;
	}

	return false;
}

bool Living::is_baby() const
{
	if (auto species = get_owner()->get_species())
	{
		return get_age() < species->child_age;
	}

	return false;
}

bool Living::is_child() const
{
	if (auto species = get_owner()->get_species())
	{
		return get_age() >= species->child_age && get_age() < species->adult_age;
	}

	return false;
}

bool Living::is_adult() const
{
	if (auto species = get_owner()->get_species())
	{
		return get_age() >= species->adult_age;
	}

	return false;
}


void Living::add_defaults()
{
	// Needs //

	add_stat(
		{
			.ID = needs::AROUSAL,
			.stat_info = StatInfo
			{
				.type = StatType::Need,
				.positive_name = "arousal",
				.min_value = 0,
				.max_value = 100,
				.description = {
					{ "cold",			0,  10, color::get("gray") },
					{ "simulated",		10, 25, color::get("pale_pink") },
					{ "aroused",		25, 50, color::get("pink") },
					{ "horny",			50, 80, color::get("fuchsia") },
					{ "sex-crazed",		80, 101, color::get("red") },
				},
			},
			.change_rate = StatRate{ .amount = -1, .tick = TimeUnit::from_minutes(20.0) },
			.starting_value = 0
		}
	);

	add_stat(
		{
			.ID = needs::HUNGER,
			.stat_info = StatInfo
			{
				.type = StatType::Need,
				.positive_name = "hunger",
				.min_value = 0,
				.max_value = 100,
				.description = {
					{ "full",			0,  10, color::get("gray") },
					{ "peckish",		10, 30, color::get("tan") },
					{ "hungry",			30, 50, color::get("copper") },
					{ "famished",		50, 80, color::get("burnt_umber") },
					{ "starving",		80, 101, color::get("red") },
				},
			},
			.change_rate = StatRate{ .amount = 1, .tick = TimeUnit::from_minutes(26.0) },
			.starting_value = 0
		}
	);

	add_stat(
		{
			.ID = needs::THIRST,
			.stat_info = StatInfo
			{
				.type = StatType::Need,
				.positive_name = "thirst",
				.min_value = 0,
				.max_value = 100,
				.description = {
					{ "hydrated",		0,  10, color::get("gray") },
					{ "parched",		10, 30, color::get("light_blue") },
					{ "thirsty",		30, 70, color::get("blue") },
					{ "dehydrated",		70, 101, color::get("dark_blue") },
				},
			},
			.change_rate = StatRate{ .amount = 1, .tick = TimeUnit::from_minutes(22.0) },
			.starting_value = 0
		}
	);

	add_stat(
		{
			.ID = needs::HYGIENE,
			.stat_info = StatInfo
			{
				.type = StatType::Need,
				.positive_name = "hygiene",
				.min_value = 0,
				.max_value = 100,
				.description = {
					{ "immaculate",		0,  10, color::get("gray") },
					{ "presentable",	10, 40, color::get("peach") },
					{ "grody",			40, 60, color::get("yellow") },
					{ "disheveled",		60, 80, color::get("orange") },
					{ "filthy",			80, 101, color::get("dark_brown") },
				}
			},
			.change_rate = StatRate{ .amount = 1, .tick = TimeUnit::from_minutes(28.0) },
			.starting_value = 0
		}
	);

	add_stat(
		{
			.ID = needs::FATIGUE,
			.stat_info = StatInfo
			{
				.type = StatType::Need,
				.positive_name = "fatigue",
				.min_value = 0,
				.max_value = 100,
				.description = {
					{ "wide awake",		0,  10, color::get("gray") },
					{ "alert",			10, 40, color::get("lavender") },
					{ "wearied",		40, 60, color::get("lilac") },
					{ "tired",			60, 80, color::get("purple") },
					{ "exhausted",		80, 101, color::get("dark_indigo") },
				}
			},
			.change_rate = StatRate{ .amount = 1, .tick = TimeUnit::from_minutes(23.0) },
			.starting_value = 0
		}
	);

	add_stat(
		{
			.ID = needs::MOOD,
			.stat_info = StatInfo
			{
				.type = StatType::Need,
				.positive_name = "mood",
				.negative_name = "mood",
				.min_value = -100,
				.max_value = 100,
				.description = {
					{ "hopeless",		-100, -80, color::get("dark_scarlet") },
					{ "miserable",		-80,  -60, color::get("carmine") },
					{ "upset",			-60,  -30, color::get("chestnut") },
					{ "meh",			-30,  -10, color::get("rose") },
					{ "fine",			-10,   10, color::get("gray") },
					{ "chill",			 10,   30, color::get("moss_green") },
					{ "great",			 30,   60, color::get("mint_green") },
					{ "ecstatic",		 60,   80, color::get("green") },
					{ "loving life!",	 80,  101, color::get("spring_green") },
				}
			},
			.change_rate = StatRate{ .amount = -1, .tick = TimeUnit::from_minutes(42.0) },
			.starting_value = 0
		}
	);

	// Attributes //

	add_stat(
		{
			.ID = attributes::STRENGTH,
			.stat_info = StatInfo
			{
				.type = StatType::Attribute,
				.positive_name = "strength",
				.min_value = 0,
				.max_value = 20,
				.description = {
					{ "weak",			0,  2, color::get("purple") },
					{ "average",		2,  8, color::get("gray") },
					{ "competent",		8,  12, color::get("white") },
					{ "excellent",		12, 16, color::get("light_yellow") },
					{ "incredible",		16, 20, color::get("yellow") },
				}
			},
			.change_rate = StatRate{ .amount = -1, .tick = TimeUnit::from_hours(48.0) },
			.starting_value = 6
		}
	);

	add_stat(
		{
			.ID = attributes::AGILITY,
			.stat_info = StatInfo
			{
				.type = StatType::Attribute,
				.positive_name = "agility",
				.min_value = 0,
				.max_value = 20,
				.description = {
					{ "weak",			0,  2, color::get("purple") },
					{ "average",		2,  8, color::get("gray") },
					{ "competent",		8,  12, color::get("white") },
					{ "excellent",		12, 16, color::get("light_yellow") },
					{ "incredible",		16, 20, color::get("yellow") },
				}
			},
			.change_rate = StatRate{ .amount = -1, .tick = TimeUnit::from_hours(48.0) },
			.starting_value = 6
		}
	);

	add_stat(
		{
			.ID = attributes::ENDURANCE,
			.stat_info = StatInfo
			{
				.type = StatType::Attribute,
				.positive_name = "endurance",
				.min_value = 0,
				.max_value = 20,
				.description = {
					{ "weak",			0,  2, color::get("purple") },
					{ "average",		2,  8, color::get("gray") },
					{ "competent",		8,  12, color::get("white") },
					{ "excellent",		12, 16, color::get("light_yellow") },
					{ "incredible",		16, 20, color::get("yellow") },
				}
			},
			.change_rate = StatRate{ .amount = -1, .tick = TimeUnit::from_hours(48.0) },
			.starting_value = 6
		}
	);
}

void Living::add_stat(const StatInitializer& init)
{
	if (has_stat(init.ID))
		return;

	m_stats[init.ID] = Stat(init.stat_info, init.starting_value);

	switch (init.stat_info.type)
	{
	case StatType::Need:
		m_needs.push_back(init.ID);
		break;
	case StatType::Trait:
		m_traits.push_back(init.ID);
		break;
	case StatType::Attribute:
		m_attributes.push_back(init.ID);
		break;
	case StatType::Skill:
		m_skills.push_back(init.ID);
		break;
	}

	add_timer(init.ID + "_TIMER", init.ID, init.change_rate);
}

void Living::remove_stat(const StatID& ID)
{
	if (!has_stat(ID))
		return;

	auto& stat = m_stats[ID];

	switch (stat.info().type)
	{
	case StatType::Need:
		std::erase(m_needs, ID);
		break;
	case StatType::Trait:
		std::erase(m_traits, ID);
		break;
	case StatType::Attribute:
		std::erase(m_attributes, ID);
		break;
	case StatType::Skill:
		std::erase(m_skills, ID);
		break;
	}

	m_stats.erase(ID);
}

void Living::add_timer(const TimerID& ID, const StatID& modified_stat, const StatRate& rate)
{
	m_timers_by_id[ID] = m_timers.size();
	m_timers.emplace_back(ID, modified_stat, rate, rate.tick);
}

void Living::remove_timer(const TimerID& ID)
{
	if (!has_timer(ID))
		return;

	size_t idx = m_timers_by_id.at(ID);
	m_timers_by_id.erase(ID);

	if (m_timers[idx] != m_timers.back())
	{
		TimerID swap_id = m_timers.back().ID;

		std::swap(m_timers[idx], m_timers.back());

		m_timers_by_id[swap_id] = idx;
	}

	m_timers.pop_back();
}

double Living::get_sleep_quality() const
{
	double sleep_quality = 1.0;

	if (get_owner()->has_movable())
	{
		auto tile = get_owner()->get_tile_map()->get_tile(get_owner()->get_movable()->pos());
		sleep_quality = tile->get_type()->sleep_quality;

		auto furniture_at = get_owner()->get_world()->get_entities_at(
			get_owner()->get_movable()->pos(),
			EntityFilter{
				.ignore_ids = { get_owner()->ID() },
				.required_flags = { EntityFlag::IS_FURNITURE },
			}
		);

		if (!furniture_at.empty())
		{
			sleep_quality = furniture_at.front()->get_furniture_def()->sleep_quality;
		}
	}

	return sleep_quality;
}

bool Living::can_sleep() const
{
	auto fatigue = get_stat(needs::FATIGUE);

	if (fatigue.get_value() > 20)
	{
		return true;
	}

	return false;
}

bool Living::can_wake_up() const
{
	auto fatigue = get_stat(needs::FATIGUE);

	if (fatigue.get_value() < 40)
	{
		return true;
	}

	return false;
}

bool Living::should_sleep() const
{
	auto fatigue = get_stat(needs::FATIGUE);

	if (fatigue.get_value() > 92)
	{
		return true;
	}

	return false;
}

bool Living::should_wake_up() const
{
	auto fatigue = get_stat(needs::FATIGUE);

	if (fatigue.get_value() < 20)
	{
		return true;
	}

	return false;
}

void Living::fall_asleep()
{
	add_timer("REST_TIMER", needs::FATIGUE, StatRate{ .amount = -1, .tick = TimeUnit::from_minutes(12.0) });
	
	if (get_owner()->is_player())
	{
		get_owner()->get_world_state()->set_tick_speed(TickSpeed::REAL_TIME_8X);
	}

	auto n1 = m_owner->display_noun();
	m_owner->game_message(
		fmt::format("{} {} asleep.", grammar::The(n1), grammar::is_plural(n1) ? "fall" : "falls")
	);
	
	set_asleep(true);
}

void Living::wake_up()
{
	remove_timer("REST_TIMER");

	if (get_owner()->is_player())
	{
		get_owner()->get_world_state()->set_tick_speed(TickSpeed::TURN_BASED);
	}

	auto n1 = m_owner->display_noun();
	m_owner->game_message(
		fmt::format("{} {} up.", grammar::The(n1), grammar::is_plural(n1) ? "wake" : "wakes")
	);

	set_asleep(false);
}

bool Living::take_consumable(const ConsumableEffect& effect)
{
	if (!has_stat(effect.modified_stat))
	{
		return false;
	}

	auto& stat = m_stats[effect.modified_stat];
	stat.add_value(effect.amount);

	return true;
}

void Living::die()
{
	auto n1 = m_owner->display_noun();
	m_owner->game_message(
		GameMessage(
			fmt::format("{} {}.", grammar::A(n1), grammar::is_plural(n1) ? "die" : "dies"),
			m_owner->is_player() ? color::get("red") : color::get("light_red")
		)
	);
	
	set_alive(false);
}

void Living::starve_to_death()
{
	auto n1 = m_owner->display_noun();
	m_owner->game_message(
		GameMessage(
			fmt::format("{} {} to death.", grammar::A(n1), grammar::is_plural(n1) ? "starve" : "starves"),
			m_owner->is_player() ? color::get("red") : color::get("light_red")
		)
	);

	set_alive(false);
}

void Living::die_of_dehydration()
{
	auto n1 = m_owner->display_noun();
	m_owner->game_message(
		GameMessage(
			fmt::format("{} {} of dehydration.", grammar::A(n1), grammar::is_plural(n1) ? "die" : "dies"),
			m_owner->is_player() ? color::get("red") : color::get("light_red")
		)
	);

	set_alive(false);
}


void Living::step(const TimeUnit& time_delta)
{
	// Check if dead
	if (is_dead())
	{
		return;
	}

	// Manage stats
	if (m_required_important_parts > 0)
	{
		if (m_required_important_parts > m_owner->get_anatomy()->count_body_parts(BodyPartFilter{ .required_flags = BodyPartFlag::IMPORTANT }))
		{
			die();
			return;
		}
	}

	// Manage needs
	auto hunger = get_stat(needs::HUNGER);

	if (hunger.get_value() > 95)
	{
		starve_to_death();
		return;
	}

	auto thirst = get_stat(needs::THIRST);

	if (thirst.get_value() > 95)
	{
		die_of_dehydration();
		return;
	}

	if (is_asleep() && should_wake_up())
	{
		wake_up();
	}

	if (!is_asleep() && should_sleep())
	{
		fall_asleep();
	}

	// Manage timers
	for (auto& timer_data : m_timers)
	{
		auto& [timer_id, modified_stat, rate, timer] = timer_data;

		auto& stat = get_stat(modified_stat);
		timer += time_delta * GameSettings::WORLD_RELATIVE_SPEED;

		while (timer >= rate.tick)
		{
			stat.add_value(rate.amount);
			timer -= std::min(rate.tick, timer);
		}
	}
}