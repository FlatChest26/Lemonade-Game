#pragma once

#ifndef LEMONADE_GAME_SRC_TILE_H
#define LEMONADE_GAME_SRC_TILE_H

#include "thing.h"
#include "tile_type.h"

#include "snowy_rng.h"
#include <random>
#include <string>

#include <cassert>

class Tile
{
protected:
	const TileType* m_type{ nullptr };
	size_t m_texture_idx{ 0 };

	bool m_visible = false;
	bool m_explored = false;

public:

	Tile(const TileType* type, size_t idx = 0);

	constexpr const	TileType* get_type() const { return m_type; }

	// -- Thing -- //

	virtual ThingID ID() const { return m_type->ID(); }
	virtual std::string description() const { return m_type->description(); }

	virtual constexpr Noun get_noun() const { return m_type->get_noun(); }
	virtual constexpr Name get_name() const { return m_type->get_name(); }
	virtual constexpr Pronouns get_pronouns() const { return m_type->get_pronouns(); }

	virtual Renderable get_renderable() const { return m_type->get_texture()[m_texture_idx]; }
	virtual glyph_t glyph() const { return get_renderable().glyph; }
	virtual color_t fg() const { return get_renderable().fg; }
	virtual color_t bg() const { return get_renderable().bg; }

	constexpr std::string singular_noun() const { return get_noun().singular; }
	constexpr std::string plural_noun() const { return get_noun().plural; }

	constexpr bool is_named() const { return !get_name().empty(); }

	constexpr std::string nickname() const
	{
		if (is_named()) return prefix() + get_name().nickname + suffix();
		return prefix() + singular_noun() + suffix();
	}

	constexpr std::string full_name() const
	{
		if (is_named()) return prefix() + get_name().full_name + suffix();
		return prefix() + singular_noun() + suffix();
	}

	constexpr std::string they() const { return get_pronouns().subjective; }
	constexpr std::string them() const { return get_pronouns().objective; }
	constexpr std::string their() const { return get_pronouns().determiner; }
	constexpr std::string theirs() const { return get_pronouns().independent_possessive; }
	constexpr std::string themselves() const { return get_pronouns().reflexive; }

	virtual constexpr bool is_plural() const { return m_type->is_plural(); }
	virtual constexpr bool is_pronouns_plural() const { return m_type->is_pronouns_plural(); }

	virtual constexpr std::string prefix() const { return m_type->prefix(); }
	virtual constexpr std::string suffix() const { return m_type->suffix(); }

	// Misc //

	std::string get_display_name() const { return full_name(); }
	operator std::string() const { return get_display_name(); }

	// Flag Checks //

	bool is_walkable() const { return m_type->tile_flags() & TileType::Flag::IS_WALKABLE; }
	bool is_blocked() const { return !is_walkable(); }

	bool is_transparent() const { return m_type->tile_flags() & TileType::Flag::IS_TRANSPARENT; }
	bool is_opaque() const { return !is_transparent(); }

	bool has_floor() const { return m_type->tile_flags() & TileType::Flag::HAS_FLOOR; }
	bool has_ceiling() const { return m_type->tile_flags() & TileType::Flag::HAS_CEILING; }

	bool can_ascend() const { return m_type->tile_flags() & TileType::Flag::CAN_ASCEND; }
	bool can_descend() const { return m_type->tile_flags() & TileType::Flag::CAN_DESCEND; }

	bool can_open() const { return m_type->tile_flags() & TileType::Flag::CAN_OPEN; }
	bool can_close() const { return m_type->tile_flags() & TileType::Flag::CAN_CLOSE; }

	bool is_air() const { return m_type->tile_flags() & TileType::Flag::IS_AIR; }

	bool opens_neighbors() const { return m_type->tile_flags() & TileType::Flag::OPENS_NEIGHBORS; }
	bool closes_neighbors() const { return m_type->tile_flags() & TileType::Flag::CLOSES_NEIGHBORS; }

	bool is_half_tile() const { return m_type->tile_flags() & TileType::Flag::IS_HALF_TILE; }

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

	constexpr const TileTexture& get_texture() const { return m_type->get_texture(); }

	constexpr bool is_random_texture() const { return m_type->has_random_texture(); }
	constexpr bool is_wall_texture() const { return m_type->has_wall_texture(); }

	constexpr bool can_connect_with(Tile* other) const
	{
		if (!other)
		{
			return false;
		}

		for (const auto& connect_id : m_type->get_connect_with())
		{
			for (const auto& other_connect_id : other->get_type()->get_tags())
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
		return m_type == other;
	}

	constexpr bool operator==(Tile* other)
	{
		return m_type == other->get_type();
	}

	constexpr bool operator!=(TileType* other)
	{
		return m_type != other;
	}

	constexpr bool operator!=(Tile* other)
	{
		return m_type != other->get_type();
	}
};

#endif // !LEMONADE_GAME_SRC_TILE_H