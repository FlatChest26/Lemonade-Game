#include "tile_map.h"
#include "tile_type.h"
#include "fov.h"
#include "world.h"

#include <cmath>

#include "raw_master.h"



TileMap::TileMap(World* world, Length width, Length height, Length depth) :
	m_world(world), m_width(width), m_height(height), m_depth(depth)
{
	m_void_tile = Tile(RawMaster::get<TileType>("TILE_VOID"));

	m_null_material = Material(Material::initializer{ 
		.material_type = RawMaster::get<MaterialType>("null"), 
		.starting_temperature = 60.0_degF,
		.starting_amount = 0,
	});

	m_tcod_map_layers.reserve(depth);

	for (Coord z = 0; (Length)z < depth; ++z)
	{
		m_tcod_map_layers.emplace_back(std::make_unique<TCODMap>(m_width, m_height));
	}

	auto fill_tile = RawMaster::get<TileType>("TILE_VOID");

	if (RawMaster::has<TileType>("TILE_AIR"))
	{
		fill_tile = RawMaster::get<TileType>("TILE_AIR");
	}

	m_tiles.resize(size_t(width * height * depth), Tile(fill_tile));
	m_materials.resize(size_t(width * height * depth), m_null_material);

	for (Coord x = 0; (Length)x < m_width; ++x) for (Coord y = 0; (Length)y < m_height; ++y) for (Coord z = 0; (Length)z < m_depth; ++z)
	{
		update_tile_at(x, y, z);
	}

	request_fov_update();
}

TileMap::~TileMap()
{
}

World* TileMap::get_world()
{
	return m_world;
}

void TileMap::set_world(World* world)
{
	m_world = world;
}

Tile* TileMap::get_tile(Coord x, Coord y, Coord z)
{
	if (!in_bounds(x, y, z))
	{
		return &m_void_tile; // Return void if tile out of bounds
	}

	return &m_tiles[get_index(x, y, z)];
}

const Tile* TileMap::get_tile(Coord x, Coord y, Coord z) const
{
	if (!in_bounds(x, y, z))
	{
		return &m_void_tile; // Return void if tile out of bounds
	}

	return &m_tiles[get_index(x, y, z)];
}

constexpr bool TileMap::in_bounds(Coord x, Coord y, Coord z) const
{
	return
		0 <= x && static_cast<Length>(x) < m_width &&
		0 <= y && static_cast<Length>(y) < m_height &&
		0 <= z && static_cast<Length>(z) < m_depth;
}

constexpr size_t TileMap::get_index(Coord x, Coord y, Coord z) const
{
	return static_cast<size_t>(x) * static_cast<size_t>(get_height()) * static_cast<size_t>(get_depth())
		+ static_cast<size_t>(y) * static_cast<size_t>(get_depth())
		+ static_cast<size_t>(z);
}


Material& TileMap::get_material(Coord x, Coord y, Coord z)
{
	if (!in_bounds(x, y, z))
	{
		return m_null_material; // Return null if material out of bounds
	}

	return m_materials[get_index(x, y, z)];
}

const Material& TileMap::get_material(Coord x, Coord y, Coord z) const
{
	if (!in_bounds(x, y, z))
	{
		return m_null_material; // Return null if material out of bounds
	}

	return m_materials[get_index(x, y, z)];
}

void TileMap::set_tile(Coord x, Coord y, Coord z, RawMaster::Ref<TileType> type, bool update)
{
	if (!in_bounds(x, y, z))
	{
		return; // Do nothing if tile out of bounds
	}

	m_tiles[get_index(x, y, z)] = Tile(type);

	if (!update)
	{
		return;
	}

	update_tile_at(x, y, z);

	// Update adjacent tiles
	for (auto& pos : transforms::get_adjacent_locations(Position(x, y, z)))
	{
		update_tile_at(pos);
	}

	request_fov_update();
}

void TileMap::set_material(Coord x, Coord y, Coord z, RawMaster::Ref<MaterialType> type, MaterialAmount amount, units::Temperature temperature, bool update)
{
	if (!in_bounds(x, y, z))
	{
		return; // Do nothing if tile out of bounds
	}

#ifdef _DEBUG
	if (amount > DEFAULT_MAX_MATERIAL_AMOUNT)
	{
		CERR("input amount greater than max material amount! " << amount);
	}
#endif // _DEBUG

	m_materials[get_index(x, y, z)] = Material(Material::initializer{ 
		.material_type = type, 
		.starting_temperature = temperature, 
		.starting_amount = std::clamp<MaterialAmount>(amount, 0, DEFAULT_MAX_MATERIAL_AMOUNT)
	});

	if (!update)
	{
		return;
	}

	update_tile_at(x, y, z);

	// Update adjacent tiles
	for (auto& pos : transforms::get_adjacent_locations(Position(x, y, z)))
	{
		update_tile_at(pos);
	}

	request_fov_update();
}

void TileMap::set_temperature(Coord x, Coord y, Coord z, units::Temperature temperature, bool update)
{
	if (!in_bounds(x, y, z))
	{
		return; // Do nothing if tile out of bounds
	}

	m_materials[get_index(x, y, z)].temperature() = temperature;

	if (!update)
	{
		return;
	}

	update_tile_at(x, y, z);

	// Update adjacent tiles
	for (auto& pos : transforms::get_adjacent_locations(Position(x, y, z)))
	{
		update_tile_at(pos);
	}

	request_fov_update();
}

void TileMap::add_material(Coord x, Coord y, Coord z, RawMaster::Ref<MaterialType> type, MaterialAmount amount, units::Temperature temperature, bool update)
{
	if (!in_bounds(x, y, z))
	{
		return; // Do nothing if tile out of bounds
	}

	auto& material = m_materials[get_index(x, y, z)];

	if (*material.material_type() == *type)
	{
		material.temperature() = temperature;
		material.amount() = std::clamp<MaterialAmount>(material.amount() + amount, 0, DEFAULT_MAX_MATERIAL_AMOUNT);
	}
	else
	{
		return set_material(x, y, z, type, amount, temperature, update);
	}

	if (!update)
	{
		return;
	}

	update_tile_at(x, y, z);

	// Update adjacent tiles
	for (auto& pos : transforms::get_adjacent_locations(Position(x, y, z)))
	{
		update_tile_at(pos);
	}

	request_fov_update();
}

void TileMap::update_tile_at(Coord x, Coord y, Coord z)
{
	if (!in_bounds(x, y, z))
	{
		return; // Do nothing if tile out of bounds
	}

	auto tile = get_tile(x, y, z);

	auto tcod_map = get_tcod_map_layer((size_t)z);
	tcod_map->setProperties(x, y, !is_opaque(x, y, z), !is_blocked(x, y, z));

	// Update wall textures
	if (tile->is_wall_texture())
	{
		int mask_value = 0;

		auto tile_above = get_tile(x, y - 1, z);
		auto tile_below = get_tile(x, y + 1, z);
		auto tile_left = get_tile(x - 1, y, z);
		auto tile_right = get_tile(x + 1, y, z);

		if (tile_above && (tile->can_connect_with(tile_above) || tile_above->can_connect_with(tile)))
		{
			mask_value += 1;
		}

		if (tile_below && (tile->can_connect_with(tile_below) || tile_below->can_connect_with(tile)))
		{
			mask_value += 2;
		}

		if (tile_left && (tile->can_connect_with(tile_left) || tile_left->can_connect_with(tile)))
		{
			mask_value += 4;
		}

		if (tile_right && (tile->can_connect_with(tile_right) || tile_right->can_connect_with(tile)))
		{
			mask_value += 8;
		}

		if (mask_value >= 0 && mask_value <= 16)
		{
			auto decoration_index = DECORATION_MASK[mask_value];

			tile->set_texture_idx(decoration_index);
		}
		else
		{
			tile->set_texture_idx(0);
		}
	}

	add_dirty_tile(x, y, z);
}



void TileMap::compute_fov(Coord pov_x, Coord pov_y, Coord pov_z, Coord radius)
{
	for (Coord x = 0; (Length)x < get_width(); ++x) for (Coord y = 0; (Length)y < get_height(); ++y) for (Coord z = 0; (Length)z < get_depth(); ++z)
	{
		fov_mark_visible(x, y, z, false);
	}

	FOV::compute_fov(this, Position(pov_x, pov_y, pov_z), radius);

	for (size_t z_layer = 0; z_layer < (size_t)get_depth(); z_layer++)
	{
		auto tcod_map = get_tcod_map_layer(z_layer);
		for (Coord x = 0; (Length)x < get_width(); ++x) for (Coord y = 0; (Length)y < get_height(); ++y)
		{
			fov_mark_visible(x, y, Coord(z_layer), tcod_map->isInFov(x, y));
		}
	}

	m_should_update_fov = false;
}

void TileMap::fov_mark_visible(Coord x, Coord y, Coord z, bool visible)
{
	if (!in_bounds(x, y, z))
	{
		return;
	}

	if (auto tcod_map = get_tcod_map_layer((size_t)z))
	{
		tcod_map->setInFov(x, y, visible);
	}

	get_tile(x, y, z)->set_visible(visible);
}

bool TileMap::is_visible(Coord x, Coord y, Coord z) const
{
	if (!in_bounds(x, y, z))
	{
		return false;
	}

	return get_tile(x, y, z)->is_visible();
}

bool TileMap::is_explored(Coord x, Coord y, Coord z) const
{
	if (!in_bounds(x, y, z))
	{
		return false;
	}

	return get_tile(x, y, z)->is_explored();
}

bool TileMap::is_blocked(Coord x, Coord y, Coord z) const
{
	if (!in_bounds(x, y, z))
	{
		return true;
	}

	auto& material = get_material(x, y, z);

	if (material.amount() > 3 && material.current_state_definition()->is_blocked())
	{
		return true;
	}

	return get_tile(x, y, z)->is_blocked();
}

bool TileMap::is_opaque(Coord x, Coord y, Coord z) const
{
	if (!in_bounds(x, y, z))
	{
		return true;
	}

	auto& material = get_material(x, y, z);

	if (material.amount() > 3 && material.current_state_definition()->is_opaque())
	{
		return true;
	}
		
	return get_tile(x, y, z)->is_opaque();
}

void TileMap::add_dirty_tile(const Position& position)
{
	if (!m_already_added_dirty_tiles.contains(position))
	{
		m_already_added_dirty_tiles.insert(position);
		m_dirty_tiles.push_back(position);
	}
}

void TileMap::step(const TimeUnit& time_delta)
{
	auto world = get_world();

	// Update materials

	std::vector<Position> current_dirty_tiles = std::move(m_dirty_tiles);

	m_already_added_dirty_tiles.clear();

	for (auto& pos : current_dirty_tiles)
	{
		auto tile = get_tile(pos);
		auto tile_below = get_tile(pos + Position(0, 0, -1));

		auto& material = get_material(pos);
		material.update_state();

		material.current_step() += time_delta;
		TimeUnit spill_time = TimeUnit::from_seconds(0.5);

		bool stay_dirty = false;

		auto can_spill_to_tile = [&](const Position& other_pos, bool ignore_time = false)
			{
				auto dz = other_pos.z - pos.z;

				if (get_world()->is_blocked(other_pos, nullptr, dz))
					return false;

				if (!ignore_time && material.current_step() < spill_time)
					return false;

				auto other_material = get_material(other_pos);

				if (other_material.amount() >= material.amount())
					return false;

				if (dz < 0 || material.current_state() == MaterialStateType::GAS)
					return material.amount() > 0;
				
				return material.amount() > 1;
			};

		switch (material.current_state())
		{
		case MaterialStateType::GAS:
		case MaterialStateType::LIQUID:
		{
			auto spread_positions = transforms::get_surrounding_locations(pos, material.current_state() == MaterialStateType::GAS, material.current_state() == MaterialStateType::LIQUID);
			
			while (material.current_step() >= spill_time)
			{
				std::shuffle(spread_positions.begin(), spread_positions.end(), snowy::rng::generator);
				for (auto& p : spread_positions)
				{
					if (can_spill_to_tile(p))
					{
						add_material(p, material.get_material_type(), 1, material.temperature(), true);
						material.amount()--;
						break;
					}
				}

				material.current_step() -= spill_time;
			}

			for (auto& p : spread_positions)
			{
				if (can_spill_to_tile(p, true))
				{
					stay_dirty = true;
					break;
				}
			}

			break;
		}
		}

		if (material.current_state() == MaterialStateType::GAS)
		{
			bool dissipate = true;
			auto spread_positions = transforms::get_surrounding_locations(pos, true, true);

			for (auto& p : spread_positions)
			{
				auto material_at = get_material(p);
				if (material_at.amount() > 1 && material_at.get_material_type() == material.get_material_type())
				{
					dissipate = false;
					break;
				}
			}

			if (dissipate)
			{
				material.amount() = 0;
			}
		}

		if (stay_dirty)
		{
			add_dirty_tile(pos);
		}
	}
}