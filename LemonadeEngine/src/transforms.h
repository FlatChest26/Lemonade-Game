#pragma once

#ifndef LEMONADE_ENGINE_SRC_TRANSFORMS_H
#define LEMONADE_ENGINE_SRC_TRANSFORMS_H

#include <cstdint>
#include <iostream>
#include <tuple>
#include <math.h>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

#include "enum_traits.h"


template <typename T>
concept TransformsNumeric = std::is_arithmetic_v<T>;

template<TransformsNumeric T>
struct Vec2D
{
public:
	using IntType = T;
	using TupleVec2D = std::tuple<IntType, IntType>;
	using Vec2DType = Vec2D<IntType>;

public:
	// -- Variables -- //

	IntType x{ 0 }, y{ 0 };

public:
	// -- Constructors -- //

	constexpr Vec2D() :
		x(static_cast<IntType>(0)), y(static_cast<IntType>(0))
	{
	}

	constexpr Vec2D(const IntType& x, const IntType& y) :
		x(x), y(y)
	{
	}

	constexpr Vec2D(const TupleVec2D& vec2d) :
		Vec2D(std::get<0>(vec2d), std::get<1>(vec2d))
	{
	}

	constexpr Vec2D(const IntType& u) :
		x(u), y(u)
	{
	}

	constexpr Vec2D(const Vec2DType& other) :
		x(other.x), y(other.y)
	{
	}

	constexpr Vec2D(Vec2DType&& other) noexcept :
		x(other.x), y(other.y)
	{
	}
public:
	// -- Getters -- //

	constexpr TupleVec2D as_tuple() const
	{
		return TupleVec2D(x, y);
	}

	constexpr bool empty() const
	{
		return x == 0 && y == 0;
	}

	constexpr double length() const
	{
		return std::sqrt(static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(y) * static_cast<double>(y));
	}

	constexpr double length_squared() const
	{
		return static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(y) * static_cast<double>(y);
	}

	constexpr Vec2DType normalized() const
	{
		double _length = length();
		if (_length > 0.0f)
		{
			return Vec2DType(static_cast<IntType>(static_cast<double>(x) / _length), static_cast<IntType>(static_cast<double>(y) / _length));
		}

		return Vec2DType(x, y);
	}

	constexpr Vec2DType unit() const
	{
		return Vec2DType(std::clamp(x, -1, 1), std::clamp(y, -1, 1));
	}

public:
	// Casts //

	constexpr operator TupleVec2D() const
	{
		return as_tuple();
	}

	constexpr operator Vec2D<int32_t>() const
	{
		return { static_cast<int32_t>(x), static_cast<int32_t>(y) };
	}
	constexpr operator Vec2D<uint32_t>() const
	{
		return { static_cast<uint32_t>(x), static_cast<uint32_t>(y) };
	}

public:
	// Equality //

	constexpr bool operator==(const Vec2DType& other) const
	{
		return x == other.x && y == other.y;
	}

public:
	// Not Equals //

	constexpr bool operator!=(const Vec2DType& other) const
	{
		return x != other.x && y != other.y;
	}

public:
	// Equals Assignment //

	constexpr Vec2DType& operator=(const Vec2DType& other)
	{
		x = other.x; y = other.y;
		return *this;
	}

	template<typename T, typename U>
	constexpr Vec2D<T>& operator=(const U& other)
	{
		x = static_cast<T>(other);
		y = static_cast<T>(other);

		return *this;
	}

public:
	// Addition //

	constexpr Vec2DType operator+(const Vec2DType& other)
	{
		return  { x + other.x, y + other.y };
	}

	template<typename T, typename U>
	constexpr Vec2D<T> operator+(const U& other)
	{
		return  { x + static_cast<T>(other), y + static_cast<T>(other) };
	}

public:
	// Addition Assignment //

	constexpr Vec2DType& operator+=(const Vec2DType& other)
	{
		x += other.x; y += other.y;
		return *this;
	}

	template<typename T, typename U>
	constexpr Vec2D<T>& operator+=(const U& other)
	{
		x += static_cast<T>(other);
		y += static_cast<T>(other);

		return *this;
	}

public:
	// Subtraction //

	constexpr Vec2DType operator-(const Vec2DType& other)
	{
		return  { x - other.x, y - other.y };
	}

	template<typename T, typename U>
	constexpr Vec2D<T> operator-(const U& other)
	{
		return  { x - static_cast<T>(other), y - static_cast<T>(other) };
	}

public:
	// Subtraction Assignment //

	constexpr Vec2DType& operator-=(const Vec2DType& other)
	{
		x -= other.x; y -= other.y;
		return *this;
	}

	template<typename T, typename U>
	constexpr Vec2D<T>& operator-=(const U& other)
	{
		x -= static_cast<T>(other);
		y -= static_cast<T>(other);

		return *this;
	}

public:
	// Multiplication //

	constexpr Vec2DType operator*(const Vec2DType& other)
	{
		return  { x * other.x, y * other.y };
	}

	template<typename T, typename U>
	constexpr Vec2D<T> operator*(const U& other)
	{
		return  { x * static_cast<T>(other), y * static_cast<T>(other) };
	}

public:
	// Multiplication Assignment //

	constexpr Vec2DType& operator*=(const Vec2DType& other)
	{
		x *= other.x; y *= other.y;
		return *this;
	}

	template<typename T, typename U>
	constexpr Vec2D<T>& operator*=(const U& other)
	{
		x *= static_cast<T>(other);
		y *= static_cast<T>(other);

		return *this;
	}

public:
	// Division / //

	constexpr Vec2DType operator/(const Vec2DType& other)
	{
		return  { x / other.x, y / other.y };
	}

	template<typename T, typename U>
	constexpr Vec2D<T> operator/(const U& other)
	{
		return  { x / static_cast<T>(other), y / static_cast<T>(other) };
	}

public:
	// Division Assignment /= //

	constexpr Vec2DType& operator/=(const Vec2DType& other)
	{
		x /= other.x; y /= other.y;
		return *this;
	}

	template<typename T, typename U>
	constexpr Vec2D<T>& operator/=(const U& other)
	{
		x /= static_cast<T>(other);
		y /= static_cast<T>(other);

		return *this;
	}

public:
	// Modulus % //

	constexpr Vec2DType operator%(const Vec2DType& other)
	{
		return Vec2DType{
			static_cast<IntType>(static_cast<int>(x) % static_cast<int>(other.x)),
			static_cast<IntType>(static_cast<int>(y) % static_cast<int>(other.y)),
		};
	}

	template<typename T, typename U>
	constexpr Vec2D<T> operator%(const U& other)
	{
		return Vec2D<T>{
			static_cast<T>(static_cast<int>(x) % static_cast<int>(other.x)),
			static_cast<T>(static_cast<int>(y) % static_cast<int>(other.y)),
		};
	}

public:
	// Modulus Assignment %= //

	constexpr Vec2DType& operator%=(const Vec2DType& other)
	{
		x = static_cast<IntType>(static_cast<int>(x) % static_cast<int>(other.x));
		y = static_cast<IntType>(static_cast<int>(y) % static_cast<int>(other.y));
		return *this;
	}

	template<typename T, typename U>
	constexpr Vec2D<T>& operator%=(const U& other)
	{
		x %= static_cast<T>(other);
		y %= static_cast<T>(other);

		return *this;
	}

public:
	// Comparison //

	constexpr bool operator<(const Vec2DType& other)
	{
		return  x < other.x && y < other.y;
	}

	constexpr bool operator<=(const Vec2DType& other)
	{
		return x <= other.x && y <= other.y;
	}

	constexpr bool operator>(const Vec2DType& other)
	{
		return  x > other.x && y > other.y;
	}

	constexpr bool operator>=(const Vec2DType& other)
	{
		return x >= other.x && y >= other.y;
	}

	template<typename T>
	constexpr operator Vec2D<T>()
	{
		return Vec2D<T>(static_cast<T>(x), static_cast<T>(y));
	}
};

template <typename VEC_TYPE = int32_t>
inline float vec2d_length(const Vec2D<VEC_TYPE>& vec2d, bool squared = false)
{
	if (squared)
	{
		return std::fabs(static_cast<float>(vec2d.x * vec2d.x) + static_cast<float>(vec2d.y * vec2d.y));
	}
	else
	{
		return std::fabs(std::sqrtf(static_cast<float>(vec2d.x * vec2d.x) + static_cast<float>(vec2d.y * vec2d.y)));
	}
}

namespace std 
{
    template<typename T>
    struct hash<Vec2D<T>> {
        std::size_t operator()(const Vec2D<T>& p) const noexcept {
            // Combine the hashes of the individual members
            std::size_t h1 = std::hash<T>{}(p.x);
            std::size_t h2 = std::hash<T>{}(p.y);
            
            // Simple bit shifting to combine hashes (prevents collision for symmetric values like 2,1 and 1,2)
            return h1 ^ (h2 << 1); 
        }
    };
}


template<TransformsNumeric T>
struct Vec3D
{
public:
	using IntType = T;
	using TupleVec3D = std::tuple<IntType, IntType, IntType>;
	using TupleVec2D = std::tuple<IntType, IntType>;
	using Vec3DType = Vec3D<IntType>;

public:
	// -- Variables -- //

	IntType x{ 0 }, y{ 0 }, z{ 0 };

public:
	// -- Constructors -- //

	constexpr Vec3D() :
		x(static_cast<IntType>(0)), y(static_cast<IntType>(0)), z(static_cast<IntType>(0))
	{
	}

	constexpr Vec3D(const IntType& x, const IntType& y, const IntType& z) :
		x(x), y(y), z(z)
	{
	}

	constexpr Vec3D(const TupleVec3D& vec3d) :
		Vec3D(std::get<0>(vec3d), std::get<1>(vec3d), std::get<2>(vec3d))
	{
	}

	constexpr Vec3D(const IntType& u) :
		x(u), y(u), z(u)
	{
	}

	constexpr Vec3D(const Vec3DType& other) :
		x(other.x), y(other.y), z(other.z)
	{
	}

	constexpr Vec3D(Vec3DType&& other) noexcept :
		x(other.x), y(other.y), z(other.z)
	{
	}
public:
	// -- Getters -- //

	constexpr TupleVec3D as_tuple() const
	{
		return TupleVec3D(x, y, z);
	}
	constexpr TupleVec2D as_2d_tuple() const
	{
		return TupleVec2D(x, y);
	}

	constexpr bool empty() const
	{
		return x == 0 && y == 0 && z == 0;
	}

	constexpr double length() const
	{
		return std::sqrt(static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(y) * static_cast<double>(y) + static_cast<double>(z) * static_cast<double>(z));
	}

	constexpr double length_squared() const
	{
		return static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(y) * static_cast<double>(y) + static_cast<double>(z) * static_cast<double>(z);
	}

	constexpr Vec3DType normalized() const
	{
		double _length = length();
		if (_length > 0.0f)
		{
			return Vec3DType(
				static_cast<IntType>(static_cast<double>(x) / _length), 
				static_cast<IntType>(static_cast<double>(y) / _length),
				static_cast<IntType>(static_cast<double>(z) / _length)
			);
		}

		return Vec3DType(x, y, z);
	}

	constexpr Vec3DType unit() const
	{
		return Vec3DType(std::clamp(x, -1, 1), std::clamp(y, -1, 1), std::clamp(z, -1, 1));
	}

public:
	// Casts //

	constexpr operator TupleVec3D() const
	{
		return as_tuple();
	}
	constexpr operator TupleVec2D() const
	{
		return as_2d_tuple();
	}
	constexpr operator Vec3D<int32_t>() const
	{
		return { static_cast<int32_t>(x), static_cast<int32_t>(y), static_cast<int32_t>(z) };
	}
	constexpr operator Vec3D<uint32_t>() const
	{
		return { static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(z) };
	}

public:
	// Equality //

	constexpr bool operator==(const Vec3DType& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

public:
	// Not Equals //

	constexpr bool operator!=(const Vec3DType& other) const
	{
		return x != other.x && y != other.y && z != other.z;
	}

public:
	// Equals Assignment //

	constexpr Vec3DType& operator=(const Vec3DType& other)
	{
		x = other.x; y = other.y; z = other.z;
		return *this;
	}

	template<typename T, typename U>
	constexpr Vec3D<T>& operator=(const U& other)
	{
		x = static_cast<T>(other);
		y = static_cast<T>(other);
		z = static_cast<T>(other);

		return *this;
	}

public:
	// Addition //

	constexpr Vec3DType operator+(const Vec3DType& other)
	{
		return  { x + other.x, y + other.y, z + other.z };
	}

	template<typename T, typename U>
	constexpr Vec3D<T> operator+(const U& other)
	{
		return  { x + static_cast<T>(other), y + static_cast<T>(other), z + static_cast<T>(other) };
	}

public:
	// Addition Assignment //

	constexpr Vec3DType& operator+=(const Vec3DType& other)
	{
		x += other.x; y += other.y; z += other.z;
		return *this;
	}

	template<typename T, typename U>
	constexpr Vec3D<T>& operator+=(const U& other)
	{
		x += static_cast<T>(other);
		y += static_cast<T>(other);
		z += static_cast<T>(other);

		return *this;
	}

public:
	// Subtraction //

	constexpr Vec3DType operator-(const Vec3DType& other)
	{
		return  { x - other.x, y - other.y, z - other.z };
	}

	template<typename T, typename U>
	constexpr Vec3D<T> operator-(const U& other)
	{
		return  { x - static_cast<T>(other), y - static_cast<T>(other), z - static_cast<T>(other) };
	}

public:
	// Subtraction Assignment //

	constexpr Vec3DType& operator-=(const Vec3DType& other)
	{
		x -= other.x; y -= other.y; z -= other.z;
		return *this;
	}

	template<typename T, typename U>
	constexpr Vec3D<T>& operator-=(const U& other)
	{
		x -= static_cast<T>(other);
		y -= static_cast<T>(other);
		z -= static_cast<T>(other);

		return *this;
	}

public:
	// Multiplication //

	constexpr Vec3DType operator*(const Vec3DType& other)
	{
		return  { x * other.x, y * other.y, z * other.z };
	}

	template<typename T, typename U>
	constexpr Vec3D<T> operator*(const U& other)
	{
		return  { x * static_cast<T>(other), y * static_cast<T>(other), z * static_cast<T>(other) };
	}

public:
	// Multiplication Assignment //

	constexpr Vec3DType& operator*=(const Vec3DType& other)
	{
		x *= other.x; y *= other.y; z *= other.z;
		return *this;
	}

	template<typename T, typename U>
	constexpr Vec3D<T>& operator*=(const U& other)
	{
		x *= static_cast<T>(other);
		y *= static_cast<T>(other);
		z *= static_cast<T>(other);

		return *this;
	}

public:
	// Division / //

	constexpr Vec3DType operator/(const Vec3DType& other)
	{
		return  { x / other.x, y / other.y, z / other.z };
	}

	template<typename T, typename U>
	constexpr Vec3D<T> operator/(const U& other)
	{
		return  { x / static_cast<T>(other), y / static_cast<T>(other), z / static_cast<T>(other) };
	}

public:
	// Division Assignment /= //

	constexpr Vec3DType& operator/=(const Vec3DType& other)
	{
		x /= other.x; y /= other.y; z /= other.z;
		return *this;
	}

	template<typename T, typename U>
	constexpr Vec3D<T>& operator/=(const U& other)
	{
		x /= static_cast<T>(other);
		y /= static_cast<T>(other);
		z /= static_cast<T>(other);

		return *this;
	}

public:
	// Modulus % //

	constexpr Vec3DType operator%(const Vec3DType& other)
	{
		return Vec3DType{ 
			static_cast<IntType>(static_cast<int>(x) % static_cast<int>(other.x)),
			static_cast<IntType>(static_cast<int>(y) % static_cast<int>(other.y)),
			static_cast<IntType>(static_cast<int>(z) % static_cast<int>(other.z))
		};
	}

	template<typename T, typename U>
	constexpr Vec3D<T> operator%(const U& other)
	{
		return Vec3D<T>{
			static_cast<T>(static_cast<int>(x) % static_cast<int>(other.x)),
			static_cast<T>(static_cast<int>(y) % static_cast<int>(other.y)),
			static_cast<T>(static_cast<int>(z) % static_cast<int>(other.z)),
		};
	}

public:
	// Modulus Assignment %= //

	constexpr Vec3DType& operator%=(const Vec3DType& other)
	{
		x = static_cast<IntType>(static_cast<int>(x) % static_cast<int>(other.x));
		y = static_cast<IntType>(static_cast<int>(y) % static_cast<int>(other.y));
		z = static_cast<IntType>(static_cast<int>(z) % static_cast<int>(other.z));
		return *this;
	}

	template<typename T, typename U>
	constexpr Vec3D<T>& operator%=(const U& other)
	{
		x %= static_cast<T>(other);
		y %= static_cast<T>(other);
		z %= static_cast<T>(other);

		return *this;
	}

public:
	// Comparison //

	constexpr bool operator<(const Vec3DType& other)
	{
		return  x < other.x && y < other.y && z < other.z;
	}

	constexpr bool operator<=(const Vec3DType& other)
	{
		return x <= other.x && y <= other.y && z <= other.z;
	}

	constexpr bool operator>(const Vec3DType& other)
	{
		return  x > other.x && y > other.y && z > other.z;
	}

	constexpr bool operator>=(const Vec3DType& other)
	{
		return x >= other.x && y >= other.y && z >= other.z;
	}

public:

	template<typename T>
	constexpr operator Vec3D<T>()
	{
		return Vec3D<T>(static_cast<T>(x), static_cast<T>(y), static_cast<T>(z));
	}
};

template <typename VEC_TYPE = int32_t>
inline float vec3d_length(const Vec3D<VEC_TYPE>& vec3d, bool squared = false)
{
	if (squared)
	{
		return std::fabs(static_cast<float>(vec3d.x * vec3d.x) + static_cast<float>(vec3d.y * vec3d.y) + static_cast<float>(vec3d.z * vec3d.z));
	}
	else
	{
		return std::fabs(std::sqrtf(static_cast<float>(vec3d.x * vec3d.x) + static_cast<float>(vec3d.y * vec3d.y) + static_cast<float>(vec3d.z * vec3d.z)));
	}
}

namespace std {
	template<typename T>
	struct hash<Vec3D<T>> 
	{
		std::size_t operator()(const Vec3D<T>& p) const noexcept {
			// Combine the hashes of the individual members
			std::size_t h1 = std::hash<T>{}(p.x);
			std::size_t h2 = std::hash<T>{}(p.y);
			std::size_t h3 = std::hash<T>{}(p.y);

			// Simple bit shifting to combine hashes (prevents collision for symmetric values like 2,1 and 1,2)
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
}


// Absolute coordinate. 1 Coord = 1/24 ChunkCoord.
using Coord = int32_t;
using CoordF = double;

using Position = Vec3D<Coord>;
using PositionF = Vec3D<CoordF>;
using TuplePosition = std::tuple<Coord, Coord, Coord>;
using TuplePositionF = std::tuple<CoordF, CoordF, CoordF>;

using Length = uint32_t;
using LengthF = double;

using Size = Vec3D<Length>;
using SizeF = Vec3D<LengthF>;
using TupleSize = std::tuple<Length, Length, Length>;
using TupleSizeF = std::tuple<LengthF, LengthF, LengthF>;

namespace std
{
	template <typename VEC_TYPE = int32_t>
	inline string to_string(const Vec3D<VEC_TYPE>& vec3d)
	{
		return "(" + to_string(vec3d.x) + ", " + to_string(vec3d.y) + ", " + to_string(vec3d.z) + ")";
	}

	template <typename VEC_TYPE = int32_t>
	inline wstring to_wstring(const Vec3D<VEC_TYPE>& vec3d)
	{
		return "(" + to_wstring(vec3d.x) + ", " + to_wstring(vec3d.y) + ", " + to_wstring(vec3d.z) + ")";
	}
}

template <typename VEC_TYPE = int32_t>
inline std::ostream& operator<<(std::ostream& os, const Vec3D<VEC_TYPE>& vec3d)
{
	return os << std::to_string<VEC_TYPE>(vec3d);
}

template <typename VEC_TYPE = int32_t>
inline std::wostream& operator<<(std::wostream& os, const Vec3D<VEC_TYPE>& vec3d)
{
	return os << std::to_wstring<VEC_TYPE>(vec3d);
};

enum Direction : uint8_t
{
	NO_DIRECTION = 0,

	NORTH = 1 << 0,
	SOUTH = 1 << 1,

	EAST = 1 << 2,
	WEST = 1 << 3,

	ABOVE = 1 << 4,
	BELOW = 1 << 5,

	NORTHEAST = NORTH | EAST,
	NORTHWEST = NORTH | WEST,

	SOUTHEAST = SOUTH | EAST,
	SOUTHWEST = SOUTH | WEST,

	NORTH_ABOVE = NORTH | ABOVE,
	NORTH_BELOW = NORTH | BELOW,
	NORTHEAST_ABOVE = NORTH | EAST | ABOVE,
	NORTHWEST_ABOVE = NORTH | WEST | ABOVE,
	NORTHEAST_BELOW = NORTH | EAST | BELOW,
	NORTHWEST_BELOW = NORTH | WEST | BELOW,

	SOUTH_ABOVE = SOUTH | ABOVE,
	SOUTH_BELOW = SOUTH | BELOW,
	SOUTHEAST_ABOVE = SOUTH | EAST | ABOVE,
	SOUTHWEST_ABOVE = SOUTH | WEST | ABOVE,
	SOUTHEAST_BELOW = SOUTH | EAST | BELOW,
	SOUTHWEST_BELOW = SOUTH | WEST | BELOW,

	EAST_ABOVE = EAST | ABOVE,
	EAST_BELOW = EAST | BELOW,

	WEST_ABOVE = WEST | ABOVE,
	WEST_BELOW = WEST | BELOW,
};

IS_FLAG_ENUM(Direction)

namespace transforms
{
	inline constexpr Position dir_as_pos(const Direction& dir)
	{
		return Position{
			(dir & WEST) ? -1 : (dir & EAST) ? 1 : 0,
			(dir & NORTH) ? -1 : (dir & SOUTH) ? 1 : 0,
			(dir & BELOW) ? -1 : (dir & ABOVE) ? 1 : 0
		};
	}

	inline constexpr Direction pos_as_dir(const Position& pos)
	{
		return Direction{
			(pos.x < 0) ? WEST : (pos.x > 0) ? EAST : NO_DIRECTION |
			(pos.y < 0) ? NORTH : (pos.y > 0) ? SOUTH : NO_DIRECTION |
			(pos.z < 0) ? BELOW : (pos.z > 0) ? ABOVE : NO_DIRECTION
		};
	}

	inline constexpr Direction pos_as_dir(const Coord& x, const Coord& y, const Coord& z)
	{
		return pos_as_dir(Position(x, y, z));
	}
}

namespace std
{
	inline string to_string(Direction dir)
	{
		return
			string(dir & Direction::NORTH ? "north " : "") +
			string(dir & Direction::SOUTH ? "south " : "") +
			string(dir & Direction::WEST ? "west " : "") +
			string(dir & Direction::EAST ? "east " : "") +
			string(dir & Direction::ABOVE ? "above" : "") +
			string(dir & Direction::BELOW ? "below" : "");
	}

	inline ostream& operator<<(ostream& os, Direction dir)
	{
		return os << to_string(dir);
	}
}

inline constexpr Position DEFAULT_POSITION = Position{ 0, 0, 0 };
inline constexpr PositionF DEFAULT_POSITIONF = PositionF{ 0.0, 0.0, 0.0 };
inline constexpr Size DEFAULT_SIZE = Size{ 1, 1, 1 };
inline constexpr SizeF DEFAULT_SIZEF = SizeF{ 1.0, 1.0, 1.0 };
inline constexpr Direction DEFAULT_DIRECTION = NO_DIRECTION;



template<typename T, typename K>
struct Transform
{
	using PositionType = T;
	using SizeType = K;

public:
	// -- Variables -- //
	PositionType position{ DEFAULT_POSITION };
	SizeType size{ DEFAULT_SIZE };
	Direction direction{ DEFAULT_DIRECTION };

	constexpr Transform(const PositionType& position = DEFAULT_POSITION, const SizeType& size = DEFAULT_SIZE, const Direction& direction = DEFAULT_DIRECTION) :
		position(position), size(size), direction(direction)
	{
	}

	constexpr bool operator==(const Transform& other) const
	{
		return position == other.position && size == other.size && direction == other.direction;
	}

	constexpr Transform& operator=(const Transform& other)
	{
		position = other.position;
		size = other.size;
		direction = other.direction;

		return *this;
	}

	constexpr operator Transform<Position, Size>()
	{
		return Transform<Position, Size>(static_cast<Position>(position), static_cast<Size>(size), direction);
	}

	constexpr operator Transform<PositionF, SizeF>()
	{
		return Transform<PositionF, SizeF>(static_cast<PositionF>(position), static_cast<SizeF>(size), direction);
	}
};

namespace transforms
{
	inline std::vector<Position> get_surrounding_locations(Position origin = Position(0, 0, 0), bool above = false, bool below = false)
	{
		std::vector<Position> positions;

		positions.reserve(size_t(8 + (above ? 8 : 0) + (below ? 8 : 0)));

		positions.push_back(origin + Position(1, 0, 0));
		positions.push_back(origin + Position(1, 1, 0));
		positions.push_back(origin + Position(0, 1, 0));
		positions.push_back(origin + Position(-1, 1, 0));
		positions.push_back(origin + Position(-1, 0, 0));
		positions.push_back(origin + Position(-1, -1, 0));
		positions.push_back(origin + Position(0, -1, 0));
		positions.push_back(origin + Position(1, -1, 0));

		if (above)
		{
			positions.push_back(origin + Position(1, 0, 1));
			positions.push_back(origin + Position(1, 1, 1));
			positions.push_back(origin + Position(0, 1, 1));
			positions.push_back(origin + Position(-1, 1, 1));
			positions.push_back(origin + Position(-1, 0, 1));
			positions.push_back(origin + Position(-1, -1, 1));
			positions.push_back(origin + Position(0, -1, 1));
			positions.push_back(origin + Position(1, -1, 1));
		}

		if (below)
		{
			positions.push_back(origin + Position(1, 0, -1));
			positions.push_back(origin + Position(1, 1, -1));
			positions.push_back(origin + Position(0, 1, -1));
			positions.push_back(origin + Position(-1, 1, -1));
			positions.push_back(origin + Position(-1, 0, -1));
			positions.push_back(origin + Position(-1, -1, -1));
			positions.push_back(origin + Position(0, -1, -1));
			positions.push_back(origin + Position(1, -1, -1));
		}
		
		return positions;
	}

	inline std::vector<Position> get_adjacent_locations(Position origin = Position(0, 0, 0), bool above = false, bool below = false)
	{
		std::vector<Position> positions;

		positions.reserve(size_t(4 + (above ? 4 : 0) + (below ? 4 : 0)));

		positions.push_back(origin + Position(1, 0, 0));
		positions.push_back(origin + Position(0, 1, 0));
		positions.push_back(origin + Position(-1, 0, 0));
		positions.push_back(origin + Position(0, -1, 0));

		if (above)
		{
			positions.push_back(origin + Position(1, 0, 1));
			positions.push_back(origin + Position(0, 1, 1));
			positions.push_back(origin + Position(-1, 0, 1));
			positions.push_back(origin + Position(0, -1, 1));
		}

		if (below)
		{
			positions.push_back(origin + Position(1, 0, -1));
			positions.push_back(origin + Position(0, 1, -1));
			positions.push_back(origin + Position(-1, 0, -1));
			positions.push_back(origin + Position(0, -1, -1));
		}

		return positions;
	}

	inline std::vector<Position> get_diagonal_locations(Position origin = Position(0, 0, 0), bool above = false, bool below = false)
	{
		std::vector<Position> positions;

		positions.reserve(size_t(4 + (above ? 4 : 0) + (below ? 4 : 0)));

		positions.push_back(origin + Position(1, 1, 0));
		positions.push_back(origin + Position(-1, 1, 0));
		positions.push_back(origin + Position(-1, -1, 0));
		positions.push_back(origin + Position(1, -1, 0));

		if (above)
		{
			positions.push_back(origin + Position(1, 1, 1));
			positions.push_back(origin + Position(-1, 1, 1));
			positions.push_back(origin + Position(-1, -1, 1));
			positions.push_back(origin + Position(1, -1, 1));
		}

		if (below)
		{
			positions.push_back(origin + Position(1, 1, -1));
			positions.push_back(origin + Position(-1, 1, -1));
			positions.push_back(origin + Position(-1, -1, -1));
			positions.push_back(origin + Position(1, -1, -1));
		}

		return positions;
	}
}

using TileTransform = Transform<Position, Size>;
using PreciseTransform = Transform<PositionF, SizeF>;

#endif // !LEMONADE_ENGINE_SRC_TRANSFORMS_H