#include "person.h"
#include "lemonade_game.h"
#include "game_settings.h"
#include "entity.h"
#include "special_actions.h"
#include "world_state.h"

StatInt_t Stat::get_value() const
{
	if (!m_info.can_be_negative)
	{
		return std::clamp(m_value.current(), 0, 100);
	}

	return std::clamp(m_value.current(), -100, 100);
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

void Person::add_defaults()
{
	add_stat(
		needs::AROUSAL,
		StatInfo
		{
			.type = StatType::Need,
			.positive_name = "arousal",
			.can_be_negative = false,
			.description = {
				{ "cold",			0,  10, color::get("gray") },
				{ "simulated",		10, 25, color::get("pale_pink") },
				{ "aroused",		25, 50, color::get("pink") },
				{ "horny",			50, 80, color::get("fuchsia") },
				{ "sex-crazed",		80, 101, color::get("red") },
			},
		},
		StatRate{ .amount = -1, .tick = TimeUnit::from_minutes(20.0f) }
		);

	add_stat(
		needs::HUNGER,
		StatInfo
		{
			.type = StatType::Need,
			.positive_name = "hunger",
			.can_be_negative = false,
			.description = {
				{ "full",			0,  10, color::get("gray") },
				{ "peckish",		10, 30, color::get("tan") },
				{ "hungry",			30, 50, color::get("copper") },
				{ "famished",		50, 80, color::get("burnt_umber") },
				{ "starving",		80, 101, color::get("red") },
			},
		},
		StatRate{ .amount = 1, .tick = TimeUnit::from_minutes(20.0f) }
		);

	add_stat(
		needs::THIRST,
		StatInfo
		{
			.type = StatType::Need,
			.positive_name = "thirst",
			.can_be_negative = false,
			.description = {
				{ "hydrated",		0,  10, color::get("gray") },
				{ "parched",		10, 30, color::get("light_blue") },
				{ "thirsty",		30, 70, color::get("blue") },
				{ "dehydrated",		70, 101, color::get("dark_blue") },
			},
		},
		StatRate{ .amount = 1, .tick = TimeUnit::from_minutes(16.0f) }
		);

	add_stat(
		needs::HYGIENE,
		StatInfo
		{
			.type = StatType::Need,
			.positive_name = "hygiene",
			.can_be_negative = false,
			.description = {
				{ "immaculate",		0,  10, color::get("gray") },
				{ "presentable",	10, 40, color::get("peach") },
				{ "grody",			40, 60, color::get("yellow") },
				{ "disheveled",		60, 80, color::get("orange") },
				{ "filthy",			80, 101, color::get("dark_brown") },
			}
		},
		StatRate{ .amount = 1, .tick = TimeUnit::from_minutes(28.0f) }
	);

	add_stat(
		needs::FATIGUE,
		StatInfo
		{
			.type = StatType::Need,
			.positive_name = "fatigue",
			.can_be_negative = false,
			.description = {
				{ "wide awake",		0,  10, color::get("gray") },
				{ "alert",			10, 40, color::get("lavender") },
				{ "wearied",		40, 60, color::get("lilac") },
				{ "tired",			60, 80, color::get("purple") },
				{ "exhausted",		80, 101, color::get("dark_indigo") },
			}
		},
		StatRate{ .amount = 1, .tick = TimeUnit::from_minutes(23.0f) }
	);

	add_stat(
		needs::MOOD,
		StatInfo
		{
			.type = StatType::Need,
			.positive_name = "mood",
			.negative_name = "mood",
			.can_be_negative = true,
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
		StatRate{ .amount = -1, .tick = TimeUnit::from_minutes(42.0f) }
	);
}

void Person::add_stat(const StatID& ID, const StatInfo& stat, const StatRate& change_rate)
{
	if (has_stat(ID))
		return;

	m_stats[ID] = Stat(stat);

	switch (stat.type)
	{
	case StatType::Need:
		m_needs.push_back(ID);
		break;
	case StatType::Trait:
		m_traits.push_back(ID);
		break;
	case StatType::Attribute:
		m_attributes.push_back(ID);
		break;
	case StatType::Skill:
		m_skills.push_back(ID);
		break;
	}

	add_timer(ID, change_rate);
}

void Person::remove_stat(const StatID& ID)
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

double Person::get_sleep_quality() const
{
	return 1.0f;
}

bool Person::can_sleep() const
{
	auto fatigue = stat(needs::FATIGUE);

	if (fatigue.get_value() > 20)
	{
		return true;
	}

	return false;
}

bool Person::can_wake_up() const
{
	auto fatigue = stat(needs::FATIGUE);

	if (fatigue.get_value() < 40)
	{
		return true;
	}

	return false;
}

bool Person::should_sleep() const
{
	auto fatigue = stat(needs::FATIGUE);

	if (fatigue.get_value() > 92)
	{
		return true;
	}

	return false;
}

bool Person::should_wake_up() const
{
	auto fatigue = stat(needs::FATIGUE);

	if (fatigue.get_value() < 20)
	{
		return true;
	}

	return false;
}

void Person::fall_asleep()
{
	add_timer("REST_TIMER", StatRate{ .amount = -1, .tick = TimeUnit::from_minutes(5.0) });
	m_asleep = true;
}

void Person::wake_up()
{
	remove_timer("REST_TIMER");
	m_asleep = false;
}

void Person::step(const TimeUnit& time_delta)
{
	// Manage sleep
	if (is_asleep() && should_wake_up())
	{
		m_owner->start_action(std::make_unique<EndSleepAction>(m_owner->interact_speed(), m_owner));
	}

	if (!is_asleep() && should_sleep())
	{
		m_owner->start_action(std::make_unique<StartSleepAction>(m_owner->interact_speed(), m_owner));
	}

	// Manage stats
	for (auto& [ID, stat] : m_stats)
	{
		if (has_timer(ID))
		{
			auto& [rate, timer] = get_timer(ID);
			timer += time_delta * GameSettings::WORLD_RELATIVE_SPEED;

			while (timer >= rate.tick)
			{
				stat.value() += rate.amount;
				timer -= rate.tick;
			}
		}

		if (!stat.info().can_be_negative)
		{
			stat.value() = std::clamp(stat.value().current(), 0, 100);
		}
		else
		{
			stat.value() = std::clamp(stat.value().current(), -100, 100);
		}

		if (ID == needs::FATIGUE && has_timer("REST_TIMER"))
		{
			auto& [rate, timer] = get_timer("REST_TIMER");
			timer += time_delta * GameSettings::WORLD_RELATIVE_SPEED;

			while (timer >= rate.tick)
			{
				stat.value() += rate.amount;
				timer -= rate.tick;
			}
		}
	}
}