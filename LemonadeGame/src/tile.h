#pragma once

#ifndef LEMONADE_GAME_SRC_TILE_H
#define LEMONADE_GAME_SRC_TILE_H

#include "thing.h"
#include "tile_type.h"
#include "raw_master.h"

#include "snowy_rng.h"
#include <random>
#include <string>
#include <set>

class Tile : public Thing
{
protected:
	RawMaster::Ref<TileType> m_type;
	size_t m_texture_idx{ 0 };

	bool m_visible = false;
	bool m_explored = false;

	//std::unordered_set<Tile*> m_connected_tiles;

public:

	Tile(RawMaster::Ref<TileType> type = nullptr);

	const TileType* get_type() const { return m_type.get(); }

	// -- Thing -- //

	virtual ThingID ID() const { return m_type->ID; }
	virtual std::string description() const { return m_type->description; }

	virtual Noun noun() const { return m_type->noun; }

	virtual Renderable renderable() const { return m_type->texture[m_texture_idx]; }

	// Flag Checks //

	bool is_walkable() const { return m_type->flags & TileFlag::IS_WALKABLE; }
	bool is_blocked() const { return !is_walkable(); }

	bool is_transparent() const { return m_type->flags & TileFlag::IS_TRANSPARENT; }
	bool is_opaque() const { return !is_transparent(); }

	bool has_floor() const { return m_type->flags & TileFlag::HAS_FLOOR; }
	bool has_ceiling() const { return m_type->flags & TileFlag::HAS_CEILING; }

	bool can_ascend() const { return m_type->flags & TileFlag::CAN_ASCEND; }
	bool can_descend() const { return m_type->flags & TileFlag::CAN_DESCEND; }

	bool can_open() const { return m_type->flags & TileFlag::CAN_OPEN; }
	bool can_close() const { return m_type->flags & TileFlag::CAN_CLOSE; }

	bool is_air() const { return m_type->flags & TileFlag::IS_AIR; }

	bool opens_neighbors() const { return m_type->flags & TileFlag::OPENS_NEIGHBORS; }
	bool closes_neighbors() const { return m_type->flags & TileFlag::CLOSES_NEIGHBORS; }

	bool has_transparent_floor() const { return m_type->flags & TileFlag::HAS_TRANSPARENT_FLOOR; }
	bool has_opaque_floor() const { return !has_transparent_floor(); }

	bool has_transparent_ceiling() const { return m_type->flags & TileFlag::HAS_TRANSPARENT_CEILING; }
	bool has_opaque_ceiling() const { return !has_transparent_ceiling(); }

	// -- Setters & Getters -- //

	bool is_visible() const { return m_visible; }
	bool is_explored() const { return m_explored; }

	void set_visible(bool visible = true) { m_visible = visible; if (visible) set_explored(true); }
	void set_explored(bool explored = true) { m_explored = explored; }

	void set_texture_idx(size_t idx)
	{
		if (get_texture().size() > 0)
		{
			m_texture_idx = idx % get_texture().size();
		}
		else
		{
			m_texture_idx = 0;
		}
	}

	const TileTexture& get_texture() const { return m_type->texture; }

	bool is_random_texture() const { return m_type->random_texture; }
	bool is_wall_texture() const { return m_type->wall_texture; }

	double get_move_cost() const { return m_type->move_cost; }

	bool can_connect_with(Tile* other) const
	{
		if (!other)
		{
			return false;
		}

		for (const auto& connect_id : m_type->connects_with)
		{
			for (const auto& other_connect_id : other->get_type()->tags)
			{
				if (connect_id == other_connect_id)
				{
					return true;
				}
			}
		}

		return false;
	}

	constexpr bool operator==(TileType* other)
	{
		return m_type->ID == other->ID;
	}

	constexpr bool operator==(Tile* other)
	{
		return m_type->ID == other->m_type->ID;
	}

	constexpr bool operator!=(TileType* other)
	{
		return m_type->ID != other->ID;
	}

	constexpr bool operator!=(Tile* other)
	{
		return m_type->ID != other->m_type->ID;
	}
};

#endif // !LEMONADE_GAME_SRC_TILE_H