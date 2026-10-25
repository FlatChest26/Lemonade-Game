#pragma once

#ifndef LEMONADE_ENGINE_SRC_CONFIG_H
#define LEMONADE_ENGINE_SRC_CONFIG_H

#include "file_management.h"
#include <fstream>
#include <libtcod.hpp>

#if defined CONFIG_DEBUG || defined _DEBUG
#define CONFIG_COUT(...) COUT(__VA_ARGS__)

#else
#define CONFIG_COUT(...)

#endif // CONFIG_DEBUG || DEBUG

namespace cfg
{
	inline FilePath init_path;

	namespace init
	{
		inline std::string RENDERER = "SDL";

		inline int CONSOLE_WIDTH = 96, CONSOLE_HEIGHT = 24;
		inline int SCREEN_WIDTH = 1200, SCREEN_HEIGHT = 800;

		inline bool FULLSCREEN = false;
		inline int FULLSCREEN_WIDTH = 1280, FULLSCREEN_HEIGHT = 1024;

		inline std::string TILESET = "(8x12) curses_640x300.png";

		inline bool SHOW_FPS = false;
		inline bool USE_CONSOLE_SIZE = true;
	}

	inline const char* REQUIRED_INIT_KEYS[] =
	{
		"renderer",

		"screen_width",
		"screen_height",

		"tileset",

		"fullscreen",
		"fullscreen_width",
		"fullscreen_height",
	};

	inline bool parse_init_cfg(const char* filename)
	{
		TCODParser parser{};
		auto init_struct = parser.newStructure("init");

		init_struct->addProperty("renderer",			TCOD_TYPE_STRING,	true);
		init_struct->addProperty("console_width",		TCOD_TYPE_INT,		false);
		init_struct->addProperty("console_height",		TCOD_TYPE_INT,		false);
		init_struct->addProperty("screen_width",		TCOD_TYPE_INT,		false);
		init_struct->addProperty("screen_height",		TCOD_TYPE_INT,		false);
		init_struct->addProperty("use_console_size",	TCOD_TYPE_BOOL,		false);
		init_struct->addProperty("tileset",				TCOD_TYPE_STRING,	true);
		init_struct->addProperty("fullscreen",			TCOD_TYPE_BOOL,		false);
		init_struct->addProperty("fullscreen_width",	TCOD_TYPE_INT,		true);
		init_struct->addProperty("fullscreen_height",	TCOD_TYPE_INT,		true);

		init_struct->addProperty("show_fps",			TCOD_TYPE_BOOL,		false);

		parser.run(filename, NULL);

		// Checking Keys Keys

		for (const char* required_key : REQUIRED_INIT_KEYS)
		{
			if (!parser.hasProperty(((std::string)"init." + required_key).c_str()))
			{
				CONFIG_COUT((std::string)"Error: Parsing settings failed. No 'init." + required_key + "' key found in settings.cfg.");
				return false;
			}
		}

		// Console Resolution
		init::CONSOLE_WIDTH = parser.getIntProperty("init.console_width");
		init::CONSOLE_HEIGHT = parser.getIntProperty("init.console_height");

		CONFIG_COUT("CONSOLE_WIDTH: " << init::CONSOLE_WIDTH);
		CONFIG_COUT("CONSOLE_HEIGHT: " << init::CONSOLE_HEIGHT);

		// Screen Resolution
		init::SCREEN_WIDTH = parser.getIntProperty("init.screen_width");
		init::SCREEN_HEIGHT = parser.getIntProperty("init.screen_height");

		CONFIG_COUT("SCREEN_WIDTH: " << init::SCREEN_WIDTH);
		CONFIG_COUT("SCREEN_HEIGHT: " << init::SCREEN_HEIGHT);

		// Use Console Size
		init::USE_CONSOLE_SIZE = parser.getBoolProperty("init.use_console_size");

		CONFIG_COUT("USE_CONSOLE_SIZE: " << (init::USE_CONSOLE_SIZE ? "true" : "false"));

		// Tileset
		init::TILESET = parser.getStringProperty("init.tileset");

		CONFIG_COUT("TILESET: " << init::TILESET);

		// Renderer
		init::RENDERER = parser.getStringProperty("init.renderer");

		CONFIG_COUT("RENDERER: " << init::RENDERER);

		// Fullscreen
		init::FULLSCREEN = parser.getBoolProperty("init.fullscreen");
		init::FULLSCREEN_WIDTH = parser.getIntProperty("init.fullscreen_width");
		init::FULLSCREEN_HEIGHT = parser.getIntProperty("init.fullscreen_height");

		CONFIG_COUT("FULLSCREEN: " << (init::FULLSCREEN ? "true" : "false"));
		CONFIG_COUT("FULLSCREEN_WIDTH: " << init::FULLSCREEN_WIDTH);
		CONFIG_COUT("FULLSCREEN_HEIGHT: " << init::FULLSCREEN_HEIGHT);

		// Settings
		init::SHOW_FPS = parser.getBoolProperty("init.show_fps");

		CONFIG_COUT("SHOW_FPS: " << (init::SHOW_FPS ? "true" : "false"));

		return true;
	}

	inline bool load_init_file()
	{
		// Find config/init.cfg
		init_path = files::config_directory / "init.cfg";

		if (!std::filesystem::exists(init_path)) // Failed to find config/init.cfg
		{
			CONFIG_COUT("Error: Could not find init.cfg file " + init_path.generic_string());
			return false;
		}
		// Open config/init.cfg
		auto parse_result = parse_init_cfg("data/config/init.cfg");

		return true;
	}

	inline bool initialize()
	{
		VALIDATE_OR_RET(load_init_file(), "Failed to load settings file.");

		return true;
	}
}

inline bool config_init()
{
	VALIDATE_OR_RET(cfg::initialize(), "Failed to initialize.");

	return true;
}

#endif // !LEMONADE_ENGINE_SRC_CONFIG_H