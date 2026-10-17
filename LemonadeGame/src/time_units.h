#pragma once

#ifndef LEMONADE_GAME_SRC_TIME_UNITS_H
#define LEMONADE_GAME_SRC_TIME_UNITS_H

#include <iostream>

using TimeUnit_t = uint64_t;

constexpr TimeUnit_t MILISECONDS_IN_SECOND = 1000;
constexpr TimeUnit_t SECONDS_IN_MINUTE = 60;
constexpr TimeUnit_t MINUTES_IN_HOUR = 60;
constexpr TimeUnit_t HOURS_IN_DAY = 24;
constexpr TimeUnit_t MONTHS_IN_YEAR = 12;

struct TimeUnit
{
	TimeUnit_t miliseconds;

	double total_hours() const { return total_minutes() / MINUTES_IN_HOUR; }
	double total_minutes() const { return total_seconds() / SECONDS_IN_MINUTE; }
	double total_seconds() const { return total_miliseconds() / MILISECONDS_IN_SECOND; }
	double total_miliseconds() const { return (double)miliseconds; }

	TimeUnit(TimeUnit_t miliseconds = 0, double seconds = 0.0, double minutes = 0.0, double hours = 0.0) :
		miliseconds(miliseconds + TimeUnit_t((seconds * MILISECONDS_IN_SECOND) + (minutes * SECONDS_IN_MINUTE * MILISECONDS_IN_SECOND) + (hours * MINUTES_IN_HOUR * SECONDS_IN_MINUTE * MILISECONDS_IN_SECOND)))
	{
	}

	// -- Static Constructors -- //

	static TimeUnit none() { return TimeUnit(0); }

	static TimeUnit from_miliseconds(double miliseconds) { return TimeUnit(static_cast<TimeUnit_t>(miliseconds)); }
	static TimeUnit from_miliseconds(TimeUnit_t miliseconds) { return TimeUnit(miliseconds); }

	static TimeUnit from_seconds(double seconds) { return TimeUnit(static_cast<TimeUnit_t>(seconds * MILISECONDS_IN_SECOND)); }
	static TimeUnit from_seconds(TimeUnit_t seconds) { return TimeUnit(seconds * MILISECONDS_IN_SECOND); }

	static TimeUnit from_minutes(double minutes) { return TimeUnit(static_cast<TimeUnit_t>(minutes * SECONDS_IN_MINUTE * MILISECONDS_IN_SECOND)); }
	static TimeUnit from_minutes(TimeUnit_t minutes) { return TimeUnit(minutes * SECONDS_IN_MINUTE * MILISECONDS_IN_SECOND); }

	static TimeUnit from_hours(double hours) { return TimeUnit(static_cast<TimeUnit_t>(hours * MINUTES_IN_HOUR * SECONDS_IN_MINUTE * MILISECONDS_IN_SECOND)); }
	static TimeUnit from_hours(TimeUnit_t hours) { return TimeUnit(hours * MINUTES_IN_HOUR * SECONDS_IN_MINUTE * MILISECONDS_IN_SECOND); }

	// -- Utilities -- //

	void add(TimeUnit other)
	{
		miliseconds += other.miliseconds;
	}

	void subtract(TimeUnit other)
	{
		if (other.miliseconds > miliseconds)
			miliseconds = 0;
		else
			miliseconds -= other.miliseconds;
	}

	void print_time_24_hr() const
	{
		TimeUnit_t display_hours = TimeUnit_t(total_hours()) % 24;
		TimeUnit_t display_minutes = TimeUnit_t(total_minutes()) % 60;

		std::cout << display_hours << ":" << display_minutes << std::endl;
	}

	void print_time_12_hr() const
	{
		TimeUnit_t display_hours = TimeUnit_t(total_hours()) % 12;
		TimeUnit_t display_minutes = TimeUnit_t(total_minutes()) % 60;

		if (display_hours == 0)
		{
			display_hours = 12;
		}

		std::cout << display_hours << ":" << display_minutes;

		if (total_hours() >= 12)
		{
			std::cout << " PM";
		}
		else
		{
			std::cout << " AM";
		}

		std::cout << std::endl;
	}

	// -- Operations -- //

	constexpr TimeUnit& operator=(const TimeUnit& other)
	{
		miliseconds = other.miliseconds;
		return *this;
	}

	template<typename U>
	constexpr TimeUnit& operator=(const U& other)
	{
		miliseconds = static_cast<U>(other);
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
};

struct Month
{
	const char* name;
	const char* abbrv;
	uint32_t length;
};

inline const Month ALL_MONTHS[MONTHS_IN_YEAR] =
{
	{ "January",	"Jan",	31},
	{ "Feburary",	"Feb",	28},
	{ "March",		"Mar",	31},
	{ "April",		"Apr",	30},
	{ "May",		"May",	31},
	{ "June",		"Jun",	30},
	{ "July",		"Jul",	31},
	{ "August",		"Aug",	31},
	{ "September",	"Sep",	30},
	{ "October",	"Oct",	31},
	{ "November",	"Nov",	30},
	{ "December",	"Dec",	31},
};

inline constexpr uint32_t days_in_month(int month)
{
	return ALL_MONTHS[month].length;
}

struct TimeDate
{
	TimeUnit time_point;
	uint32_t day;
	uint32_t month;
	uint32_t year;

	void add(TimeUnit time_delta)
	{
		time_point.add(time_delta);
		fix_time();
	}

	void fix_time()
	{
		while ((TimeUnit_t)time_point.total_hours() >= HOURS_IN_DAY)
		{
			time_point.miliseconds -= (HOURS_IN_DAY * MINUTES_IN_HOUR * SECONDS_IN_MINUTE * MILISECONDS_IN_SECOND);
			day++;
		}

		while (day >= days_in_month(month))
		{
			day -= days_in_month(month);
			month++;
		}

		while (month >= MONTHS_IN_YEAR)
		{
			month -= MONTHS_IN_YEAR;
			year++;
		}
	}

	void print_date() const
	{
		std::cout << month << "/" << day << "/" << year << std::endl;
	}
};

#endif // !LEMONADE_GAME_SRC_TIME_UNITS_H