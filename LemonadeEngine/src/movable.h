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
		m_transform({ PositionType(), SizeType(), NORTH})
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
	template<typename T>
	void set_transform(T t) { m_transform = static_cast<TransformType>(t); }
	virtual void set_transform(TransformType t) { m_transform = t; }
	/* Sets this thing's transform. */
	template<typename T, typename K>
	void set_transform(T p, K s, Direction dir = Direction::NORTH) { return set_transform(TransformType(static_cast<PositionType>(p), static_cast<SizeType>(s), dir)); }
	virtual void set_transform(PositionType p, SizeType s, Direction dir = Direction::NORTH) { return set_transform(TransformType(p, s, dir)); }

	// Position

	/* Sets this thing's position. */
	template<typename T>
	void set_position(T position) { m_transform.position = static_cast<PositionType>(position); }
	virtual void set_position(PositionType position) { m_transform.position = position; }
	/* Sets this thing's position. */
	template<typename T>
	void set_position(T x, T y, T z) { return set_position(PositionType(static_cast<CoordType>(x), static_cast<CoordType>(y), static_cast<CoordType>(z))); }
	virtual void set_position(CoordType x = 0, CoordType y = 0, CoordType z = 0) { return set_position(PositionType(x, y, z)); }

	// Size

	/* Sets this thing's size. */
	virtual void set_size(SizeType size) { m_transform.size = size; }
	/* Sets this thing's size. */
	virtual void set_size(LengthType width = 0, LengthType height = 0, LengthType depth = 0) { return set_size(SizeType(width, height, depth)); }

	// Direction

	/* Faces this thing in the specified direction. */
	virtual void face_direction(Direction dir) { m_transform.direction = dir; }
	/* Faces this thing in the specified direction. */
	virtual void face_direction(Position pos) { face_direction(transforms::pos_as_dir(pos)); }
	/* Faces this thing in the specified direction. */
	template<typename T>
	void face_direction(T x, T y, T z) { face_direction(static_cast<CoordType>(x), static_cast<CoordType>(y), static_cast<CoordType>(z)); }
	virtual void face_direction(CoordType x = 0, CoordType y = 0, CoordType z = 0) { face_direction(transforms::pos_as_dir(x, y, z)); }

public:

	// -- Utility -- //

	/* Changes this thing's position by the specified amount. Returns true if the move is valid. */
	template<typename T>
	bool move(T dx, T dy, T dz)
	{
		return move(static_cast<CoordType>(dx), static_cast<CoordType>(dy), static_cast<CoordType>(dz));
	}

	virtual bool move(CoordType dx = 0, CoordType dy = 0, CoordType dz = 0)
	{
		posx() += dx; posy() += dy; posz() += dz;
		return true;
	}

	/* Changes this thing's position by the specified amount. Returns true if the move is valid. */
	template<typename T>
	bool move(T delta) { return move(static_cast<PositionType>(delta)); }
	virtual bool move(PositionType delta) { return move(delta.x, delta.y, delta.z); }
	/* Changes this thing's position by the specified amount. Returns true if the move is valid. */
	virtual bool move(Direction dir, CoordType distance = 1) { return move((PositionType)transforms::dir_as_pos(dir) * distance); }

	/* Returns true if the point is inside this thing. */
	virtual bool contains_point(int x, int y, int z) const
	{
		return posx() <= x && x < posx() + (int)width() && posy() <= y && y < posy() + (int)height() && posz() <= z && z < posz() + (int)depth();
	}
	/* Returns true if the point is inside this thing. */
	template<typename T>
	bool contains_point(T pos) const { return contains_point(static_cast<PositionType>(pos)); }
	virtual bool contains_point(PositionType pos) const { return contains_point(pos.x, pos.y, pos.z); }

	/* Returns the points inside this thing, with the top-left-lower corner at (0, 0, 0). */
	virtual std::vector<Position> get_offsets() const
	{
		if (m_transform.size == SizeType{0, 0, 0}) return { {0, 0, 0} };

		std::vector<Position> offsets;
		offsets.reserve((size_t)width() * (size_t)height() * (size_t)depth());

		for (int offset_z = 0; offset_z < (int)depth(); offset_z++)
			for (int offset_y = 0; offset_y < (int)height(); offset_y++)
				for (int offset_x = 0; offset_x < (int)width(); offset_x++)
					offsets.emplace_back(offset_x, offset_y, offset_z);

		return offsets;
	}

	/* Returns the points inside this thing. */
	virtual std::vector<Position> get_points() const
	{
		if (m_transform.size == SizeType{ 0, 0, 0 }) return { m_transform.position };

		std::vector<Position> points;
		points.reserve((size_t)width() * (size_t)height() * (size_t)depth());

		for (int offset_z = 0; offset_z < (int)depth(); offset_z++)
			for (int offset_y = 0; offset_y < (int)height(); offset_y++)
				for (int offset_x = 0; offset_x < (int)width(); offset_x++)
					points.emplace_back(posx() + offset_x, posy() + offset_y, posz() + offset_z);

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