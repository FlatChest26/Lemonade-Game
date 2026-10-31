#pragma once

#ifndef LEMONADE_GAME_SRC_TIME_UNITS_H
#define LEMONADE_GAME_SRC_TIME_UNITS_H

#include <iostream>
#include <string>

using TimeUnit_t = uint64_t;

constexpr TimeUnit_t MILISECONDS_IN_SECOND = 1000;
constexpr TimeUnit_t SECONDS_IN_MINUTE = 60;
constexpr TimeUnit_t MINUTES_IN_HOUR = 60;
constexpr TimeUnit_t HOURS_IN_DAY = 24;
constexpr TimeUnit_t MONTHS_IN_YEAR = 12;

struct TimeUnit
{
	TimeUnit_t miliseconds;

	double total_hours() const { return total_minutes() / (double)MINUTES_IN_HOUR; }
	double total_minutes() const { return total_seconds() / (double)SECONDS_IN_MINUTE; }
	double total_seconds() const { return (double)miliseconds / (double)MILISECONDS_IN_SECOND; }

	constexpr TimeUnit(const TimeUnit_t& miliseconds = 0) :
		miliseconds(miliseconds)
	{
	}

	// -- Static Constructors -- //

	static TimeUnit none() { return TimeUnit(0); }

	static TimeUnit from_miliseconds(const TimeUnit_t& miliseconds) { return TimeUnit(miliseconds); }
	static TimeUnit from_seconds(const double& seconds) { return TimeUnit(static_cast<TimeUnit_t>(seconds * (double)MILISECONDS_IN_SECOND)); }
	static TimeUnit from_minutes(const double& minutes) { return TimeUnit(static_cast<TimeUnit_t>(minutes * (double)SECONDS_IN_MINUTE * (double)MILISECONDS_IN_SECOND)); }
	static TimeUnit from_hours(const double& hours) { return TimeUnit(static_cast<TimeUnit_t>(hours * (double)MINUTES_IN_HOUR * (double)SECONDS_IN_MINUTE * (double)MILISECONDS_IN_SECOND)); }

	// -- Utilities -- //

	std::string str_24_hr() const
	{
		TimeUnit_t display_hours = TimeUnit_t(total_hours()) % 24;
		TimeUnit_t display_minutes = TimeUnit_t(total_minutes()) % 60;

		return (display_hours > 9 ? "" : "0") + std::to_string(display_hours) + ":" + (display_minutes > 9 ? "" : "0") + std::to_string(display_minutes);
	}

	std::string str_12_hr() const
	{
		TimeUnit_t display_hours = TimeUnit_t(total_hours()) % 12;
		TimeUnit_t display_minutes = TimeUnit_t(total_minutes()) % 60;

		if (display_hours == 0)
		{
			display_hours = 12;
		}

		std::string output = std::to_string(display_hours) + ":" + (display_minutes > 9 ? "" : "0") + std::to_string(display_minutes);

		if (total_hours() >= 12)
		{
			output += " PM";
		}
		else
		{
			output += " AM";
		}

		return output;
	}

	// -- Operations -- //

	constexpr operator TimeUnit_t() const
	{
		return miliseconds;
	}

	constexpr operator double() const
	{
		return static_cast<double>(miliseconds);
	}

	constexpr TimeUnit& operator=(const TimeUnit& other)
	{
		miliseconds = other.miliseconds;
		return *this;
	}

	constexpr TimeUnit& operator+=(const TimeUnit& other)
	{
		miliseconds += other.miliseconds;
		return *this;
	}

	constexpr TimeUnit& operator-=(const TimeUnit& other)
	{
		if (miliseconds < other.miliseconds)
		{
			miliseconds = 0;
		}
		else
		{
			miliseconds -= other.miliseconds;
		}

		return *this;
	}

	constexpr TimeUnit& operator*=(const TimeUnit& other)
	{
		miliseconds *= other.miliseconds;
		return *this;
	}

	constexpr TimeUnit& operator/=(const TimeUnit& other)
	{
		miliseconds /= other.miliseconds;
		return *this;
	}

	// -- Comparisons -- //

	constexpr bool operator==(const TimeUnit& other) const
	{
		return miliseconds == other.miliseconds;
	}

	constexpr bool operator!=(const TimeUnit& other) const
	{
		return miliseconds != other.miliseconds;
	}

	constexpr bool operator>(const TimeUnit& other) const
	{
		return miliseconds > other.miliseconds;
	}

	constexpr bool operator>=(const TimeUnit& other) const
	{
		return miliseconds >= other.miliseconds;
	}

	constexpr bool operator<(const TimeUnit& other) const
	{
		return miliseconds < other.miliseconds;
	}

	constexpr bool operator<=(const TimeUnit& other) const
	{
		return miliseconds <= other.miliseconds;
	}

	TimeUnit operator+(const TimeUnit& other) const
	{
		return TimeUnit(miliseconds + other.miliseconds);
	}

	TimeUnit operator-(const TimeUnit& other) const
	{
		if (miliseconds < other.miliseconds)
		{
			return 0;
		}
		else
		{
			return TimeUnit(miliseconds - other.miliseconds);
		}
	}

	TimeUnit operator*(const TimeUnit& other) const
	{
		return TimeUnit(miliseconds * other.miliseconds);
	}

	template<typename T>
	constexpr TimeUnit operator*(const T& other) const
	{
		return TimeUnit(static_cast<TimeUnit_t>(miliseconds * other));
	}

	TimeUnit operator/(const TimeUnit& other) const
	{
		return TimeUnit(miliseconds / other.miliseconds);
	}

	template<typename T>
	constexpr TimeUnit operator/(const T& other) const
	{
		return TimeUnit(static_cast<TimeUnit_t>(miliseconds / other));
	}
};

struct Month
{
	std::string name;
	std::string abbrv;
	TimeUnit_t length;
};

inline const Month ALL_MONTHS[MONTHS_IN_YEAR] =
{
	{ "January",	"Jan",	31 },
	{ "Feburary",	"Feb",	28 },
	{ "March",		"Mar",	31 },
	{ "April",		"Apr",	30 },
	{ "May",		"May",	31 },
	{ "June",		"Jun",	30 },
	{ "July",		"Jul",	31 },
	{ "August",		"Aug",	31 },
	{ "September",	"Sep",	30 },
	{ "October",	"Oct",	31 },
	{ "November",	"Nov",	30 },
	{ "December",	"Dec",	31 },
};

inline constexpr TimeUnit_t get_days_in_month(const TimeUnit_t& month)
{
	return ALL_MONTHS[month].length;
}

inline constexpr std::string get_month_name(const TimeUnit_t& month)
{
	return ALL_MONTHS[month].name;
}

inline constexpr std::string get_month_abbreviation(const TimeUnit_t& month)
{
	return ALL_MONTHS[month].abbrv;
}

class TimeDate
{
protected:
	TimeUnit m_time;
	TimeUnit_t m_day;
	TimeUnit_t m_month;
	TimeUnit_t m_year;

public:

	TimeDate(const TimeUnit& time = TimeUnit::none(), const TimeUnit_t& day = 1, const TimeUnit_t& month = 1, const TimeUnit_t& year = 1) :
		m_time(time), m_day(day), m_month(month), m_year(year)
	{
	}

	TimeUnit_t day() const { return m_day; }
	TimeUnit_t month() const { return m_month; }
	TimeUnit_t year() const { return m_year; }

	void add(const TimeUnit& time_delta)
	{
		TimeUnit clock = clock_time();

		m_time += time_delta;
		clock += time_delta;

		while (clock.total_hours() >= (double)HOURS_IN_DAY)
		{
			clock -= (HOURS_IN_DAY * MINUTES_IN_HOUR * SECONDS_IN_MINUTE * MILISECONDS_IN_SECOND);
			m_day++;
		}

		while (m_day >= get_days_in_month(m_month))
		{
			m_day -= get_days_in_month(m_month);
			m_month++;
		}

		while (m_month >= MONTHS_IN_YEAR)
		{
			m_month -= MONTHS_IN_YEAR;
			m_year++;
		}
	}

	TimeUnit& total_time()
	{
		return m_time;
	}

	const TimeUnit& total_time() const
	{
		return m_time;
	}

	TimeUnit clock_time() const
	{
		return TimeUnit::from_miliseconds(m_time.miliseconds % (HOURS_IN_DAY * MINUTES_IN_HOUR * SECONDS_IN_MINUTE * MILISECONDS_IN_SECOND));
	}

	std::string str_month_day_year() const
	{
		return std::to_string(m_month) + "/" + std::to_string(m_day) + "/" + std::to_string(m_year);
	}

	std::string str_day_month_year() const
	{
		return std::to_string(m_month) + "/" + std::to_string(m_day) + "/" + std::to_string(m_year);
	}

	std::string str_fancy(bool abbreviate_month = false) const
	{
		return (abbreviate_month ? get_month_name(m_month) : get_month_abbreviation(m_month)) + " " + std::to_string(m_day) + ", " + std::to_string(m_year);
	}
};

#endif // !LEMONADE_GAME_SRC_TIME_UNITS_H