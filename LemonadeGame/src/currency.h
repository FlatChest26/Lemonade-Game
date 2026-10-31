#pragma once

#ifndef LEMONADE_GAME_SRC_CURRENCY_H
#define LEMONADE_GAME_SRC_CURRENCY_H

#include <iostream>
#include <string>

using MoneyInt_t = int32_t;

struct Currency
{
	MoneyInt_t cents;

	double total_dollars() const { return static_cast<double>(cents) / 100; }

	constexpr Currency(const MoneyInt_t& cents = 0) :
		cents(cents)
	{
	}

	static Currency none() { return Currency(0); }
	static Currency from_cents(MoneyInt_t cents) { return Currency(cents); }

	static Currency from_dollars(MoneyInt_t dollars) { return Currency(dollars * 100); }
	static Currency from_dollars(double dollars) { return Currency(static_cast<MoneyInt_t>(dollars * 100)); }

	constexpr operator MoneyInt_t() const
	{
		return cents;
	}

	constexpr operator double() const
	{
		return static_cast<double>(cents);
	}

	constexpr Currency& operator=(const Currency& other)
	{
		cents = other.cents;
		return *this;
	}

	constexpr Currency& operator+=(const Currency& other)
	{
		return *this;
	}

	constexpr Currency& operator-=(const Currency& other)
	{
		cents -= other.cents;
		return *this;
	}

	constexpr Currency& operator*=(const Currency& other)
	{
		cents *= other.cents;
		return *this;
	}

	constexpr Currency& operator/=(const Currency& other)
	{
		cents /= other.cents;
		return *this;
	}

	// -- Comparisons -- //

	constexpr bool operator==(const Currency& other) const
	{
		return cents == other.cents;
	}

	constexpr bool operator!=(const Currency& other) const
	{
		return cents != other.cents;
	}

	constexpr bool operator>(const Currency& other) const
	{
		return cents > other.cents;
	}

	constexpr bool operator>=(const Currency& other) const
	{
		return cents >= other.cents;
	}

	constexpr bool operator<(const Currency& other) const
	{
		return cents < other.cents;
	}

	constexpr bool operator<=(const Currency& other) const
	{
		return cents <= other.cents;
	}

	Currency operator+(const Currency& other) const
	{
		return Currency(cents + other.cents);
	}

	Currency operator-(const Currency& other) const
	{
		if (cents < other.cents)
		{
			return 0;
		}
		else
		{
			return Currency(cents - other.cents);
		}
	}

	Currency operator*(const Currency& other) const
	{
		return Currency(cents * other.cents);
	}

	template<typename T>
	constexpr Currency operator*(const T& other) const
	{
		return Currency(static_cast<MoneyInt_t>(cents * other));
	}

	Currency operator/(const Currency& other) const
	{
		return Currency(cents / other.cents);
	}

	template<typename T>
	constexpr Currency operator/(const T& other) const
	{
		return Currency(static_cast<MoneyInt_t>(cents / other));
	}
};

#endif // !LEMONADE_GAME_SRC_CURRENCY_H
