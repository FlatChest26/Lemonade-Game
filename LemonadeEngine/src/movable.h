#pragma once

#ifndef LEMONADE_ENGINE_SRC_MOVABLE_H
#define LEMONADE_ENGINE_SRC_MOVABLE_H

#include "transforms.h"

/* A class that represents an object with a transform. Also provides useful utilities for working with said transform. */

template<typename T = Position, typename K = Size>
class Movable
{
public:
	using TransformType = Transform<T, K>;

	using PositionType = T;
	using SizeType = K;

	using CoordType = PositionType::IntType;
	using LengthType = SizeType::IntType;

protected:
	TransformType m_transform{};

public:
	Movable() :
		m_transform({ PositionType(0, 0, 0), SizeType(1, 1, 1), NORTH })
	{
	}

	Movable(TransformType transform) :
		m_transform(transform)
	{
	}

	virtual ~Movable()
	{
	}

public:
	// -- Getters -- //

	// Transform //

	/* Returns a reference to this thing's transform.*/
	virtual TransformType& transform() { return m_transform; }
	/* Returns a constant reference to this thing's transform.*/
	virtual const TransformType& transform() const { return m_transform; }
	/* Returns a copy of this thing's transform.*/
	virtual TransformType get_transform() const { return m_transform; }

	// Position

	/* Returns a reference to this thing's position.*/
	virtual PositionType& pos() { return m_transform.position; }
	/* Returns a constant reference to this thing's position.*/
	virtual const PositionType& pos() const { return m_transform.position; }
	/* Returns a copy of this thing's position.*/
	virtual PositionType get_pos() const { return m_transform.position; }

	/* Returns a reference to this thing's x position. */
	virtual CoordType& posx() { return m_transform.position.x; }
	/* Returns a constant reference to this thing's x position. */
	virtual const CoordType& posx() const { return m_transform.position.x; }
	/* Returns a copy of this thing's x position. */
	virtual CoordType get_posx() const { return m_transform.position.x; }

	/* Returns a reference to this thing's y position. */
	virtual CoordType& posy() { return m_transform.position.y; }
	/* Returns a constant reference to this thing's y position. */
	virtual const CoordType& posy() const { return m_transform.position.y; }
	/* Returns a copy of this thing's y position. */
	virtual CoordType get_posy() const { return m_transform.position.y; }

	/* Returns a reference to this thing's z position. */
	virtual CoordType& posz() { return m_transform.position.z; }
	/* Returns a constant reference to this thing's z position. */
	virtual const CoordType& posz() const { return m_transform.position.z; }
	/* Returns a copy of this thing's z position. */
	virtual CoordType get_posz() const { return m_transform.position.z; }

	// Size

	/* Returns a reference to this thing's size.*/
	virtual SizeType& size() { return m_transform.size; }
	/* Returns a constant reference to this thing's size.*/
	virtual const SizeType& size() const { return m_transform.size; }
	/* Returns a copy of this thing's size.*/
	virtual SizeType get_size() const { return m_transform.size; }

	/* Returns a reference to this thing's width.*/
	virtual LengthType& width() { return m_transform.size.x; }
	/* Returns a constant reference to this thing's width.*/
	virtual const LengthType& width() const { return m_transform.size.x; }
	/* Returns a copy of this thing's width.*/
	virtual LengthType get_width() const { return m_transform.size.x; }

	/* Returns a reference to this thing's height.*/
	virtual LengthType& height() { return m_transform.size.y; }
	/* Returns a constant reference to this thing's height.*/
	virtual const LengthType& height() const { return m_transform.size.y; }
	/* Returns a copy of this thing's height.*/
	virtual LengthType get_height() const { return m_transform.size.y; }

	/* Returns a reference to this thing's depth.*/
	virtual LengthType& depth() { return m_transform.size.z; }
	/* Returns a constant reference to this thing's depth.*/
	virtual const LengthType& depth() const { return m_transform.size.z; }
	/* Returns a copy of this thing's depth.*/
	virtual LengthType get_depth() const { return m_transform.size.z; }

	// Direction

	/* Returns a reference to this thing's direction.*/
	virtual Direction& direction() { return transform().direction; }
	/* Returns a constant reference to this thing's direction.*/
	virtual const Direction& direction() const { return transform().direction; }
	/* Returns a copy of this thing's direction.*/
	virtual Direction get_direction() const { return direction(); }

public:
	// -- Setters -- //

	// Transform //

	/* Sets this thing's transform. */
	template<typename T, typename K>
	void set_transform(const Transform<T, K>& t) { m_transform = static_cast<TransformType>(t); }
	/* Sets this thing's transform. */
	template<typename T, typename K>
	void set_transform(const T& p, const K& s, const Direction& dir = Direction::NORTH) { return set_transform(TransformType(static_cast<PositionType>(p), static_cast<SizeType>(s), dir)); }

	// Position

	/* Sets this thing's position. */
	template<typename T>
	void set_position(const Vec3D<T>& position) { m_transform.position = static_cast<PositionType>(position); }
	/* Sets this thing's position. */
	template<typename T>
	void set_position(const T& x, const T& y, const T& z) { return set_position(PositionType(static_cast<CoordType>(x), static_cast<CoordType>(y), static_cast<CoordType>(z))); }

	// Size

	/* Sets this thing's size. */
	template<typename T>
	void set_size(const Vec3D<T>& size) { m_transform.size = static_cast<SizeType>(size); }
	/* Sets this thing's size. */
	template<typename T>
	void set_size(const T& width, const T& height, const T& depth) { return set_size(SizeType(static_cast<LengthType>(width), static_cast<LengthType>(height), static_cast<LengthType>(depth))); }

	// Direction

	/* Faces this thing in the specified direction. */
	virtual void face_direction(const Direction& dir) { m_transform.direction = dir; }
	/* Faces this thing in the specified direction. */
	template<typename T>
	void face_direction(const Vec3D<T>& pos) { face_direction(transforms::pos_as_dir(static_cast<Position>(pos))); }

	/* Faces this thing in the specified direction. */
	template<typename T>
	void face_direction(const T& x, const T& y, const T& z) { face_direction(transforms::pos_as_dir(static_cast<Coord>(x), static_cast<Coord>(y), static_cast<Coord>(z))); }

public:

	// -- Utility -- //

	/* Changes this thing's position by the specified amount. Returns true if the move is valid. */
	template<typename T>
	bool move(const T& dx, const T& dy, const T& dz)
	{
		m_transform.position.x += static_cast<CoordType>(dx);
		m_transform.position.y += static_cast<CoordType>(dy);
		m_transform.position.z += static_cast<CoordType>(dz);

		return true;
	}

	/* Changes this thing's position by the specified amount. Returns true if the move is valid. */
	template<typename T>
	bool move(const Vec3D<T>& delta) { return move(delta.x, delta.y, delta.z); }

	/* Changes this thing's position by the specified amount. Returns true if the move is valid. */
	template<typename T>
	bool move(const Direction& dir, const T& distance = 1) { return move(static_cast<Vec3D<T>>(transforms::dir_as_pos(dir)) * distance); }

	/* Returns true if the point is inside this thing. */
	template<typename T>
	bool contains_point(const T& x, const T& y, const T& z) const
	{
		return
			m_transform.position.x <= static_cast<CoordType>(x) && static_cast<CoordType>(x) < m_transform.position.x + static_cast<CoordType>(m_transform.size.x) &&
			m_transform.position.y <= static_cast<CoordType>(y) && static_cast<CoordType>(y) < m_transform.position.y + static_cast<CoordType>(m_transform.size.y) &&
			m_transform.position.z <= static_cast<CoordType>(z) && static_cast<CoordType>(z) < m_transform.position.z + static_cast<CoordType>(m_transform.size.z);
	}

	/* Returns true if the point is inside this thing. */
	template<typename T>
	bool contains_point(Vec3D<T> pos) const { return contains_point(pos.x, pos.y, pos.z); }

	/* Returns true if the point is inside this thing. */
	template<typename T>
	bool contains_offset(const T& x, const T& y, const T& z) const
	{
		return
			0 <= static_cast<CoordType>(x) && static_cast<CoordType>(x) < static_cast<CoordType>(m_transform.size.x) &&
			0 <= static_cast<CoordType>(y) && static_cast<CoordType>(y) < static_cast<CoordType>(m_transform.size.y) &&
			0 <= static_cast<CoordType>(z) && static_cast<CoordType>(z) < static_cast<CoordType>(m_transform.size.z);
	}

	/* Returns true if the point is inside this thing. */
	template<typename T>
	bool contains_offset(Vec3D<T> pos) const { return contains_offset(pos.x, pos.y, pos.z); }

	/* Returns the points inside this thing, with the top-left-lower corner at (0, 0, 0). */
	virtual std::vector<Position> get_offsets() const
	{
		if (m_transform.size == SizeType{ 1, 1, 1 })
			return { { 0, 0, 0 } };

		std::vector<Position> offsets;
		offsets.reserve(static_cast<size_t>(m_transform.size.x * m_transform.size.y * m_transform.size.z));

		for (Coord offset_z = 0; offset_z < static_cast<Coord>(m_transform.size.z); offset_z++)
			for (Coord offset_y = 0; offset_y < static_cast<Coord>(m_transform.size.y); offset_y++)
				for (Coord offset_x = 0; offset_x < static_cast<Coord>(m_transform.size.x); offset_x++)
					offsets.emplace_back(offset_x, offset_y, offset_z);

		return offsets;
	}

	/* Returns the points inside this thing. */
	virtual std::vector<Position> get_points() const
	{
		if (m_transform.size == SizeType{ 1, 1, 1 })
			return { static_cast<Position>(m_transform.position) };

		std::vector<Position> points;
		points.reserve(static_cast<size_t>(m_transform.size.x * m_transform.size.y * m_transform.size.z));

		for (Coord offset_z = 0; offset_z < static_cast<Coord>(m_transform.size.z); offset_z++)
			for (Coord offset_y = 0; offset_y < static_cast<Coord>(m_transform.size.y); offset_y++)
				for (Coord offset_x = 0; offset_x < static_cast<Coord>(m_transform.size.x); offset_x++)
					points.emplace_back(m_transform.position.x + offset_x, m_transform.position.y + offset_y, m_transform.position.z + offset_z);

		return points;
	}

	constexpr operator Movable<Position, Size>()
	{
		return Movable<Position, Size>(static_cast<Position>(m_transform.position), static_cast<Size>(m_transform.size), m_transform.direction);
	}

	constexpr operator Movable<PositionF, SizeF>()
	{
		return Movable<PositionF, SizeF>(static_cast<PositionF>(m_transform.position), static_cast<SizeF>(m_transform.size), m_transform.direction);
	}
};

using TileMovable = Movable<Position, Size>;
using PreciseMovable = Movable<PositionF, SizeF>;

#endif // !LEMONADE_ENGINE_SRC_MOVABLE_H