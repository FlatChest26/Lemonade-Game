#include "furniture_def.h"

std::shared_ptr<RawObjectChecker> FurnitureTile::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("furniture_tile");

	checker->add_property("position", RawValueType::RAW_Array, true);
	checker->add_property("renderable", RawValueType::RAW_Custom, true, Renderable::get_checker());

	return checker;
}

FurnitureTile FurnitureTile::from_json(const json& j)
{
	auto checker = FurnitureTile::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	FurnitureTile f;

	f.position = Position(j.at("position").at(0).get<Coord>(), j.at("position").at(1).get<Coord>(), j.at("position").at(2).get<Coord>());
	f.renderable = Renderable::from_json(j.at("renderable"));

	return f;
}

std::shared_ptr<RawObjectChecker> FurnitureDefinition::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("furniture_definition");

	checker->add_property("ID", RawValueType::RAW_String, true);
	checker->add_property("item_definition", RawValueType::RAW_String, true);
	checker->add_property("size", RawValueType::RAW_Array, true);
	checker->add_property("tiles", RawValueType::RAW_Array, true);

	checker->add_property("is_blocking", RawValueType::RAW_Bool, false);

	return checker;
}

FurnitureDefinition FurnitureDefinition::from_json(const json& j)
{
	auto checker = FurnitureDefinition::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	FurnitureDefinition f;

	f.ID = j.at("ID").get<std::string>();
	f.item_definition = RawMaster::get<ItemDefinition>(j.at("item_definition").get<std::string>());
	f.size = Size(j.at("size").at(0).get<Length>(), j.at("size").at(1).get<Length>(), j.at("size").at(2).get<Length>());

	for (auto& tile_json : j.at("tiles"))
	{
		if (!FurnitureTile::get_checker()->check_json(tile_json))
			continue;

		f.tiles.push_back(FurnitureTile::from_json(tile_json));
	}

	if (checker->is_valid_property("is_blocking", j))
	{
		f.is_blocking = j.at("is_blocking").get<bool>();
	}

	return f;
}

Renderable FurnitureDefinition::get_renderable_at(Coord x, Coord y, Coord z) const
{
	for (auto& tile : tiles)
	{
		if (tile.position == Position(x, y, z))
			return tile.renderable;
	}

	return Renderable();
}