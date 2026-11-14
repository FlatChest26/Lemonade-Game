#include "furniture_def.h"

RawObjectChecker* FurnitureTile::get_checker()
{
	static RawObjectChecker checker = RawObjectChecker();

	checker.set_type_name("furniture_tile");

	checker.add_property("position", RawValueType::RAW_Array, true);
	checker.add_property("renderable", RawValueType::RAW_Custom, true, Renderable::get_checker());

	return &checker;
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

RawObjectChecker* FurnitureDefinition::get_checker()
{
	static RawObjectChecker checker = RawObjectChecker();

	checker.set_type_name("furniture_definition");

	checker.add_property("ID", RawValueType::RAW_String, true);
	checker.add_property("item_definition", RawValueType::RAW_String, true);
	checker.add_property("size", RawValueType::RAW_Array, true);
	checker.add_property("tiles", RawValueType::RAW_Array, true);

	checker.add_property("flags", RawValueType::RAW_Array, false);
	checker.add_property("sleep_quality", RawValueType::RAW_Number, false);

	return &checker;
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

	if (checker->is_valid_property("flags", j))
	{
		f.flags = FurnitureFlag::NONE;

		for (const auto& flag : j.at("flags"))
		{
			auto it = AllFurnitureFlags.find(flag.get<std::string>());

			if (it != AllFurnitureFlags.end())
			{
				f.flags |= it->second;
			}
			else
			{
				CERR("Invalid furniture flag name : " << flag);
			}
		}
	}

	if (checker->is_valid_property("sleep_quality", j))
	{
		f.sleep_quality = j.at("sleep_quality").get<double>();
	}

	return f;
}

Renderable FurnitureDefinition::renderable_at(Coord x, Coord y, Coord z) const
{
	for (auto& tile : tiles)
	{
		if (tile.position == Position(x, y, z))
			return tile.renderable;
	}

	return Renderable();
}