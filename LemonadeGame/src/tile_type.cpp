#include "tile_type.h"
#include "snowy_grammar.h"
#include "renderable.h"

#include "snowy_macros.h"
#include "snowy_utilities.h"
#include <string>

std::shared_ptr<RawObjectChecker> TileType::get_checker()
{
	static std::shared_ptr<RawObjectChecker> checker = std::make_shared<RawObjectChecker>();

	checker->set_type_name("tile_type");

	checker->add_property("ID", RawValueType::RAW_String, true);
	checker->add_property("noun", RawValueType::RAW_Custom, true, Noun::get_checker());

	checker->add_property("renderable", RawValueType::RAW_Custom, false, Renderable::get_checker());

	checker->add_property("random_glyph", RawValueType::RAW_Array, false);
	checker->add_property("random_fg", RawValueType::RAW_Array, false);
	checker->add_property("random_bg", RawValueType::RAW_Array, false);

	checker->add_property("random_texture", RawValueType::RAW_Array, false);
	checker->add_property("wall_texture", RawValueType::RAW_Array, false);

	checker->add_property("description", RawValueType::RAW_String, false);

	checker->add_property("tags", RawValueType::RAW_Array, false);
	checker->add_property("flags", RawValueType::RAW_Array, false);
	checker->add_property("connects_to", RawValueType::RAW_Array, false);

	checker->add_property("transforms_to", RawValueType::RAW_String, false);
	checker->add_property("move_cost", RawValueType::RAW_Number, false);

	return checker;
}

TileType TileType::from_json(const json& j)
{
	auto checker = TileType::get_checker();

	if (!checker->check_json(j, true))
	{
		return {};
	}

	TileType t;

	t.ID = j.at("ID").get<std::string>();
	t.noun = Noun::from_json(j.at("noun"));

	if (checker->is_valid_property("renderable", j))
	{
		t.texture = { Renderable::from_json(j.at("renderable")) };
	}
	else if (checker->is_valid_property("random_glyph", j) || checker->is_valid_property("random_fg", j) || checker->is_valid_property("random_bg", j))
	{

		std::vector<glyph_t> random_glyph{ DEFAULT_GLPYH };
		std::vector<color_t> random_fg{ DEFAULT_FG_COLOR };
		std::vector<color_t> random_bg{ DEFAULT_BG_COLOR };

		if (checker->is_valid_property("random_glyph", j))
		{
			random_glyph.clear();

			for (const auto& json_glyph : j.at("random_glyph"))
			{
				if (json_glyph.is_number())
				{
					random_glyph.push_back(json_glyph.get<glyph_t>());
				}
				else if (json_glyph.is_string())
				{
					std::string glyph_str = json_glyph.get<std::string>();

					if (glyph_str.empty())
					{
						continue;
					}

					random_glyph.push_back(static_cast<glyph_t>(glyph_str[0]));
				}
				else
				{
					CERR("Invalid glyph type: " << json_glyph);
				}
			}
		}

		if (checker->is_valid_property("random_fg", j))
		{
			random_fg.clear();
			for (const auto& json_fg : j.at("random_fg"))
			{
				random_fg.push_back(color::from_json(json_fg));
			}
		}

		if (checker->is_valid_property("random_bg", j))
		{
			random_bg.clear();
			for (const auto& json_bg : j.at("random_bg"))
			{
				random_bg.push_back(color::from_json(json_bg));
			}
		}

		t.texture.clear();

		for (glyph_t g : random_glyph) for (color_t fg : random_fg) for (color_t bg : random_bg)
		{
			t.texture.push_back(Renderable(g, fg, bg));
		}

		t.random_texture = true;
	}
	else if (checker->is_valid_property("random_texture", j))
	{
		t.texture.clear();

		for (const auto& sub_renderable : j.at("random_texture"))
		{
			if (!sub_renderable.is_structured())
			{
				continue;
			}

			t.texture.push_back(std::move(Renderable::from_json(sub_renderable)));
		}

		t.random_texture = true;
	}
	else if (checker->is_valid_property("wall_texture", j))
	{
		t.texture.clear();

		for (const auto& sub_renderable : j.at("wall_texture"))
		{
			if (!sub_renderable.is_structured())
			{
				continue;
			}
			t.texture.push_back(std::move(Renderable::from_json(sub_renderable)));
		}

		t.wall_texture = true;
	}

	if (checker->is_valid_property("description", j))
	{
		t.description = j.at("description").get<std::string>();
	}

	if (checker->is_valid_property("tags", j))
	{
		for (const auto& tag_name : j.at("tags"))
		{
			if (!tag_name.is_string())
			{
				continue;
			}

			t.tags.push_back(tag_name.get<std::string>());
		}
	}

	if (checker->is_valid_property("flags", j))
	{
		for (const auto& flag_name : j.at("flags"))
		{
			if (!flag_name.is_string())
			{
				CERR("Invalid tile flag name in species JSON object: " << flag_name);
				continue;
			}

			if (!AllTileFlags.contains(flag_name))
			{
				CERR(flag_name << " is not a valid tile flag name!");
				continue;
			}

			if (static_cast<int>(AllTileFlags[flag_name]) > 0)
			{
				t.flags |= AllTileFlags[flag_name];
			}
			else
			{
				t.flags &= AllTileFlags[flag_name];
			}
		}
	}

	if (checker->is_valid_property("connects_to", j))
	{
		for (const auto& connect_tag : j.at("connects_to"))
		{
			if (!connect_tag.is_string())
			{
				continue;
			}

			t.connects_with.push_back(connect_tag.get<std::string>());
		}
	}

	if (checker->is_valid_property("transforms_to", j))
	{
		t.transforms_to = j.at("transforms_to").get<std::string>();
	}

	if (checker->is_valid_property("move_cost", j))
	{
		t.move_cost = j.at("move_cost").get<double>();
	}

	return t;
}