#pragma once

#ifndef LEMONADE_GAME_SRC_STAT_VALUES_H
#define LEMONADE_GAME_SRC_STAT_VALUES_H

#include <cstdint>
#include <algorithm>

using StatInt_t = int32_t;

template<StatInt_t _MinValue = -1, StatInt_t _MaxValue = -1>
class StatValue
{
protected:
	StatInt_t m_current_value = 0;

public:
	StatValue(StatInt_t current = 0) :
		m_current_value(current)
	{
		m_current_value = has_min() ? std::max(m_current_value, min_val()) : m_current_value;
		m_current_value = has_max() ? std::min(m_current_value, max_val()) : m_current_value;
	}

	StatInt_t& current() { return m_current_value; }
	const StatInt_t& current() const { return m_current_value; }

	constexpr int min_val() { return _MinValue; }
	constexpr int max_val() { return _MaxValue; }

	constexpr bool has_min() const { return _MinValue != -1; }
	constexpr bool has_max() const { return _MaxValue != -1; }

public:
	// -- Operators -- //

	constexpr operator StatInt_t() const { return m_current_value; }
	constexpr operator float() const { return static_cast<float>(m_current_value); }

	template<typename T>
	constexpr bool operator<(const T& other) const
	{
		return m_current_value < (StatInt_t)other;
	}

	template<typename T>
	constexpr bool operator<=(const T& other) const
	{
		return m_current_value <= (StatInt_t)other;
	}

	template<typename T>
	constexpr bool operator>(const T& other) const
	{
		return m_current_value > (StatInt_t)other;
	}

	template<typename T>
	constexpr bool operator>=(const T& other) const
	{
		return m_current_value >= (StatInt_t)other;
	}

	template<typename T>
	constexpr bool operator==(const T& other) const
	{
		return m_current_value == (StatInt_t)other;
	}

	template<typename T>
	constexpr bool operator!=(const T& other) const
	{
		return m_current_value != (StatInt_t)other;
	}

	template<typename T>
	constexpr StatInt_t operator+(const T& other) const
	{
		return StatInt_t(m_current_value + (StatInt_t)other);
	}

	template<typename T>
	constexpr StatInt_t operator-(const T& other) const
	{
		return StatInt_t(m_current_value - (StatInt_t)other);
	}

	template<typename T>
	constexpr StatInt_t operator*(const T& other) const
	{
		return StatInt_t(m_current_value * other);
	}

	template<typename T>
	constexpr StatInt_t operator/(const T& other) const
	{
		return StatInt_t(m_current_value / other);
	}

	template<typename T>
	constexpr StatInt_t operator%(const T& other) const
	{
		return StatInt_t(m_current_value % other);
	}

	StatValue operator-() const
	{
		return StatValue(-m_current_value);
	}

	StatValue operator+() const
	{
		return StatValue(+m_current_value);
	}

	template<typename T>
	constexpr StatValue<_MinValue, _MaxValue>& operator=(const T& other)
	{
		m_current_value = (StatInt_t)other;
		m_current_value = has_min() ? std::max(m_current_value, min_val()) : m_current_value;
		m_current_value = has_max() ? std::min(m_current_value, max_val()) : m_current_value;
		return *this;
	}

	template<typename T>
	constexpr StatValue<_MinValue, _MaxValue>& operator+=(const T& other)
	{
		m_current_value += (StatInt_t)other;
		m_current_value = has_min() ? std::max(m_current_value, min_val()) : m_current_value;
		m_current_value = has_max() ? std::min(m_current_value, max_val()) : m_current_value;
		return *this;
	}

	template<typename T>
	constexpr StatValue<_MinValue, _MaxValue>& operator-=(const T& other)
	{
		m_current_value -= (StatInt_t)other;
		m_current_value = has_min() ? std::max(m_current_value, min_val()) : m_current_value;
		m_current_value = has_max() ? std::min(m_current_value, max_val()) : m_current_value;
		return *this;
	}

	template<typename T>
	constexpr StatValue<_MinValue, _MaxValue>& operator*=(const T& other)
	{
		m_current_value *= other;
		m_current_value = has_min() ? std::max(m_current_value, min_val()) : m_current_value;
		m_current_value = has_max() ? std::min(m_current_value, max_val()) : m_current_value;
		return *this;
	}

	template<typename T>
	constexpr StatValue<_MinValue, _MaxValue>& operator/=(const T& other)
	{
		m_current_value /= other;
		m_current_value = has_min() ? std::max(m_current_value, min_val()) : m_current_value;
		m_current_value = has_max() ? std::min(m_current_value, max_val()) : m_current_value;
		return *this;
	}

	template<typename T>
	constexpr StatValue<_MinValue, _MaxValue>& operator%=(const T& other)
	{
		m_current_value %= other;
		m_current_value = has_min() ? std::max(m_current_value, min_val()) : m_current_value;
		m_current_value = has_max() ? std::min(m_current_value, max_val()) : m_current_value;
		return *this;
	}

	constexpr StatValue<_MinValue, _MaxValue>& operator++()
	{
		m_current_value++;
		m_current_value = has_min() ? std::max(m_current_value, min_val()) : m_current_value;
		m_current_value = has_max() ? std::min(m_current_value, max_val()) : m_current_value;
		return *this;
	}

	constexpr StatValue<_MinValue, _MaxValue>& operator--()
	{
		m_current_value--;
		m_current_value = has_min() ? std::max(m_current_value, min_val()) : m_current_value;
		m_current_value = has_max() ? std::min(m_current_value, max_val()) : m_current_value;
		return *this;
	}
};

#endif // !LEMONADE_GAME_SRC_BODY_STAT_VALUES_H
