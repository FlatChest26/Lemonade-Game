#include "tile_type.h"
#include "snowy_grammar.h"
#include "renderable.h"

#include "snowy_macros.h"
#include "snowy_utilities.h"
#include <string>

TileType::TileType(const initializer& init) :
	m_ID(init.ID),
	m_noun(init.noun),
	m_pronouns(init.pronouns),
	m_description(init.description),
	m_texture(init.texture),
	m_flags(init.flags),
	m_random_texture(init.random_texture),
	m_wall_texture(init.wall_texture),
	m_tags(init.tags),
	m_connects_with(init.connects_with),
	m_transforms_to(init.transforms_to),
	m_move_cost(init.move_cost)
{
}

TileType TileType::from_json(const json& j)
{
	TileType::initializer t;

	t.ID = j.at("ID").get<std::string>();

	if (j.contains("noun"))
	{
		t.noun = Noun::from_json(j.at("noun"));
	}

	if (j.contains("renderable"))
	{
		t.texture = { Renderable::from_json(j.at("renderable")) };
	}
	else if (j.contains("random_glyph") || j.contains("random_fg") || j.contains("random_bg"))
	{
		if (!j.at("random_glyph").is_array())
		{
			CERR("Invalid 'random_glyph' field in species JSON object: " << j);
		}

		if (!j.at("random_fg").is_array())
		{
			CERR("Invalid 'random_fg' field in species JSON object: " << j);
		}

		if (!j.at("random_bg").is_array())
		{
			CERR("Invalid 'random_bg' field in species JSON object: " << j);
		}

		std::vector<glyph_t> random_glyph{ DEFAULT_GLPYH };
		std::vector<color_t> random_fg{ DEFAULT_FG_COLOR };
		std::vector<color_t> random_bg{ DEFAULT_BG_COLOR };

		if (j.at("random_glyph").is_array())
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
		
		if (j.at("random_fg").is_array())
		{
			random_fg.clear();
			for (const auto& json_fg : j.at("random_fg"))
			{
				random_fg.push_back(color::from_json(json_fg));
			}
		}

		if (j.at("random_bg").is_array())
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
	else if (j.contains("random_texture"))
	{
		if (!j.at("random_texture").is_array())
		{
			CERR("Invalid 'random_texture' field in species JSON object: " << j);
		}
		else
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
	}
	else if (j.contains("wall_texture"))
	{
		if (!j.at("wall_texture").is_array())
		{
			CERR("Invalid 'wall_texture' field in species JSON object: " << j);
		}
		else
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
	}

	if (j.contains("description"))
	{
		if (!j.at("description").is_string())
		{
			CERR("Invalid 'description' field in species JSON object: " << j);
			t.description = "<invalid description>";
		}
		else
		{
			t.description = j.at("description").get<std::string>();
		}
	}

	if (j.contains("tags"))
	{
		if (!j.at("tags").is_array())
		{
			CERR("Invalid 'tags' field in species JSON object: " << j);
		}
		else
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
	}

	if (j.contains("flags"))
	{
		if (!j.at("flags").is_array())
		{
			CERR("Invalid 'flags' field in species JSON object: " << j);
		}
		else
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
	}

	if (j.contains("connects_to"))
	{
		if (!j.at("connects_to").is_array())
		{
			CERR("Invalid 'connects_to' field in species JSON object: " << j);
		}
		else
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
	}

	if (j.contains("transforms_to"))
	{
		if (!j.at("transforms_to").is_string())
		{
			CERR("Invalid 'transforms_to' field in species JSON object: " << j);
		}
		else
		{
			t.transforms_to = j.at("transforms_to").get<std::string>();
		}
	}

	if (j.contains("move_cost"))
	{
		if (!j.at("move_cost").is_number())
		{
			CERR("Invalid 'move_cost' field in species JSON object: " << j);

			if (t.flags & TileType::Flag::IS_WALKABLE)
			{
				t.move_cost = 1.0f;
			}
			else
			{
				t.move_cost = 0.0f;
			}
		}
		else
		{
			t.move_cost = j.at("move_cost").get<double>();
		}
	}

	return TileType(t);
}