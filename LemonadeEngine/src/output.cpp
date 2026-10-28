#include "output.h"
#include "config.h"

SAIOutput::SAIOutput(const TCOD_ContextParams& params, const tcod::Console& console) :
	m_context(params), m_console(console)
{
}

SAIOutput::SAIOutput(const TCOD_ContextParams& params) : m_context(params)
{
	m_console = m_context.new_console();
}

SAIOutput::~SAIOutput()
{
	m_context.close();
}
void SAIOutput::update_window()
{
	m_console = m_context.new_console();
}

namespace output
{
	namespace
	{
		static std::unique_ptr<SAIOutput> _output;
	}

	SAIOutput* get_output()
	{
		return _output.get();
	}

	void convert_event(SDL_Event& event)
	{
		if (auto o = get_output())
			return o->get_context().convert_event_coordinates(event);
	}

	bool setup_context(int argc, const char* argv[])
	{
		using namespace cfg;
		TCOD_ContextParams params{};

		try
		{
			params.tcod_version = TCOD_COMPILEDVERSION;
			params.vsync = true;
			params.argc = argc;
			params.argv = argv;

			params.sdl_window_flags = SDL_WINDOW_RESIZABLE;

			// Renderer Type

			static std::unordered_map<std::string, TCOD_renderer_t> const
				renderer_types =
			{
				{ "SDL", TCOD_RENDERER_SDL },
				{ "SDL2", TCOD_RENDERER_SDL2 },
				{ "GLSL", TCOD_RENDERER_GLSL },
				{ "OPENGL", TCOD_RENDERER_OPENGL },
				{ "OPENGL2", TCOD_RENDERER_OPENGL2 }
			};

			auto renderer_type = renderer_types.find(init::RENDERER);
			if (renderer_type != renderer_types.end()) params.renderer_type = renderer_type->second;

			// Tileset
			auto tileset = tcod::load_tilesheet(FilePath{ files::graphics_directory / init::TILESET }, {16, 16}, tcod::CHARMAP_CP437);
			params.tileset = tileset.get();

			// Console
			params.columns = init::CONSOLE_WIDTH;
			params.rows = init::CONSOLE_HEIGHT;
	
			_output = std::make_unique<SAIOutput>(params);

		}
		catch (const std::exception& e)
		{
			CERR(e.what());
			return false;
		}

		return true;
	}
}

///////////////////
//
// Window Functions
//
///////////

namespace output
{
	void update_window()
	{
		if (auto o = get_output())
			return o->update_window();
	}

	void set_window_title(const char* window_title)
	{
		if (auto o = get_output())
		{
			auto window = o->get_context().get_sdl_window();
			if (window)
			{
				SDL_SetWindowTitle(window, window_title);
			}
		}
	}
}

///////////////////
//
// Console Functions
//
///////////

namespace output
{
	constexpr glyph_t to_cp437(int c)
	{
		for (glyph_t i = 0; (size_t)i < tcod::CHARMAP_CP437.size(); i++)
			if (c == tcod::CHARMAP_CP437[i]) return i;
		return c;
	}

	constexpr int from_cp437(glyph_t c)
	{
		if (0 <= c && c < tcod::CHARMAP_CP437.size())
			return tcod::CHARMAP_CP437[c];

		return c;
	}
}

namespace output // Color
{
	///////////////////
	// -- Getters -- //
	///////////////////

	glyph_t get_char(int x, int y, bool to_cp437)
	{
		if (auto o = get_output())
		{
			auto ch = TCOD_console_get_char(o->get_console().get(), x, y);

			return to_cp437 ? output::to_cp437(ch) : ch;
		}

		return DEFAULT_GLPYH;
	}

	color_t get_fg(int x, int y)
	{
		if (auto o = get_output())
		{
			return TCOD_console_get_char_foreground(o->get_console().get(), x, y);
		}

		return DEFAULT_FG_COLOR;
	}

	color_t get_bg(int x, int y)
	{
		if (auto o = get_output())
		{
			return TCOD_console_get_char_background(o->get_console().get(), x, y);
		}

		return DEFAULT_BG_COLOR;
	}

	TCOD_bkgnd_flag_t get_bg_flag(int x, int y)
	{
		return TCOD_BKGND_DEFAULT;
	}

	///////////////////
	// -- Setters -- //
	///////////////////

	void set_char(int x, int y, glyph_t glyph, bool from_cp437)
	{
		if (auto o = get_output())
		{
			TCOD_console_set_char(o->get_console().get(), x, y, from_cp437 ? output::from_cp437(glyph) : glyph);
		}
	}

	void set_fg(int x, int y, color_t fg)
	{
		if (auto o = get_output())
		{
			TCOD_console_set_char_foreground(o->get_console().get(), x, y, fg);
		}
	}

	void set_bg(int x, int y, color_t bg)
	{
		if (auto o = get_output())
		{
			TCOD_console_set_char_background(o->get_console().get(), x, y, bg, TCOD_BKGND_SET);
		}
	}

	void set_bg_flag(int x, int y, TCOD_bkgnd_flag_t flag)
	{
		if (auto o = get_output())
		{
			TCOD_console_set_char_background(o->get_console().get(), x, y, get_bg(x, y), flag);
		}
	}

	void set_color(int x, int y, color_t fg, color_t bg)
	{
		set_fg(x, y, fg);
		set_bg(x, y, bg);
	}
}

namespace output // Printing
{
	int console_print(int x, int y, const char* str)
	{
		return console_print_rgb(x, y, DEFAULT_FG_COLOR, DEFAULT_BG_COLOR, str);
	}

	int console_print(int x, int y, std::string str)
	{
		return console_print_rgb(x, y, DEFAULT_FG_COLOR, DEFAULT_BG_COLOR, str.c_str());
	}

	int console_printf(int x, int y, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		return console_printf_rgb(x, y, DEFAULT_FG_COLOR, DEFAULT_BG_COLOR, fmt, args);
	}

	int console_printf(int x, int y, std::string fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		return console_printf_rgb(x, y, DEFAULT_FG_COLOR, DEFAULT_BG_COLOR, fmt, args);
	}

	int console_printf_ex(int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		return console_printf_rgb_ex(x, y, DEFAULT_FG_COLOR, DEFAULT_BG_COLOR, flag, alignment, fmt, args);
	}

	int console_printf_ex(int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, std::string fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		return console_printf_rgb_ex(x, y, DEFAULT_FG_COLOR, DEFAULT_BG_COLOR, flag, alignment, fmt, args);
	}

	int console_printf_rect(int x, int y, int width, int height, const char* fmt, ...)
	{
		if (auto o = get_output())
		{
			va_list args;
			va_start(args, fmt);
			return TCOD_console_printf_rect(o->get_console().get(), x, y, width, height, fmt, args);
		}

		return 0;
	}

	int console_printf_rect(int x, int y, int width, int height, std::string fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		return console_printf_rect(x, y, width, height, fmt.c_str(), args);
	}

	int console_printf_rect_ex(int x, int y, int width, int height, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char* fmt, ...)
	{
		if (auto o = get_output())
		{
			va_list args;
			va_start(args, fmt);

			return TCOD_console_printf_rect_ex(o->get_console().get(), x, y, width, height, flag, alignment, fmt, args);
		}
		return 0;
	}

	int console_printf_rect_ex(int x, int y, int width, int height, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, std::string fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		return console_printf_rect_ex(x, y, width, height, flag, alignment, fmt.c_str(), args);
	}

	int console_print_rgb(int x, int y, const color_t& fg, const color_t& bg, const char* str)
	{
		return console_printf_rgb(x, y, fg, bg, str);
	}

	int console_print_rgb(int x, int y, const color_t& fg, const color_t& bg, std::string str)
	{
		return console_print_rgb(x, y, fg, bg, str.c_str());
	}

	int console_printf_rgb(int x, int y, const color_t& fg, const color_t& bg, const char* fmt, ...)
	{
		if (auto o = get_output())
		{
			va_list args;
			va_start(args, fmt);
			return TCOD_printf_rgb(
				o->get_console().get(),
				TCOD_PrintParamsRGB{
					.x = x, .y = y,
					.width = 0, .height = 0,
					.fg = &fg, .bg = &bg,
					.flag = DEFAULT_BKGND_FLAG,
					.alignment = TCOD_LEFT
				},
				fmt,
				args
			);
		}
		return 0;
	}

	int console_printf_rgb(int x, int y, const color_t& fg, const color_t& bg, std::string fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		return console_printf_rgb(x, y, fg, bg, fmt.c_str(), args);
	}

	int console_printf_rgb_ex(int x, int y, const color_t& fg, const color_t& bg, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char* fmt, ...)
	{
		if (auto o = get_output())
		{
			va_list args;
			va_start(args, fmt);
			return TCOD_printf_rgb(
				o->get_console().get(),
				TCOD_PrintParamsRGB{
					.x = x, .y = y,
					.width = 0, .height = 0,
					.fg = &fg, .bg = &bg,
					.flag = flag,
					.alignment = alignment
				},
				fmt, args
			);
		}

		return 0;
	}

	int console_printf_rgb_ex(int x, int y, const color_t& fg, const color_t& bg, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, std::string fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		return console_printf_rgb_ex(x, y, fg, bg, flag, alignment, fmt.c_str(), args);
	}

	int console_printf_rgb_rect(int x, int y, int width, int height, const color_t& fg, const color_t& bg, const char* fmt, ...)
	{
		if (auto o = get_output())
		{
			va_list args;
			va_start(args, fmt);
			return TCOD_printf_rgb(
				o->get_console().get(),
				TCOD_PrintParamsRGB{
					.x = x, .y = y,
					.width = width, .height = height,
					.fg = &fg, .bg = &bg,
					.flag = DEFAULT_BKGND_FLAG,
					.alignment = TCOD_LEFT
				},
				fmt,
				args
			);
		}

		return 0;
	}

	int console_printf_rgb_rect(
		int x, int y, int width, int height, const color_t& fg, const color_t& bg, std::string fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		return console_printf_rgb_rect(x, y, width, height, fg, bg, fmt.c_str(), args);
	}

	int console_printf_rgb_rect_ex(
		int x, int y, int width, int height, const color_t& fg, const color_t& bg, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char* fmt, ...)
	{
		if (auto o = get_output())
		{
			va_list args;
			va_start(args, fmt);
			return TCOD_printf_rgb(
				o->get_console().get(),
				TCOD_PrintParamsRGB{
					.x = x, .y = y,
					.width = width, .height = height,
					.fg = &fg, .bg = &bg,
					.flag = flag,
					.alignment = alignment
				},
				fmt,
				args
			);
		}

		return 0;
	}

	int console_printf_rgb_rect_ex(
		int x, int y, int width, int height, const color_t& fg, const color_t& bg, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, std::string fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		return console_printf_rgb_rect_ex(x, y, width, height, fg, bg, flag, alignment, fmt.c_str(), args);
	}
}

namespace output // Drawing
{
	void put_char(int x, int y, glyph_t glyph, TCOD_bkgnd_flag_t flag)
	{
		if (auto o = get_output())
		{
			TCOD_console_put_char(o->get_console().get(), x, y, output::from_cp437(glyph), flag);
		}
	}

	void put_char_ex(int x, int y, glyph_t glyph, color_t fg, color_t bg)
	{
		if (auto o = get_output())
		{
			TCOD_console_put_char_ex(o->get_console().get(), x, y, output::from_cp437(glyph), fg, bg);
		}
	}

	void put_rgb(const int& x, const int& y, const glyph_t& ch, const color_t& fg, const color_t& bg, TCOD_bkgnd_flag_t flag)
	{
		if (auto o = get_output())
		{
			TCOD_console_put_rgb(
				o->get_console().get(),
				x, y,
				output::from_cp437(ch), &fg, &bg, flag
			);
			set_color(x, y, fg, bg);
		}
	}

	void put_rgb(const int& x, const int& y, const Renderable& renderable, TCOD_bkgnd_flag_t flag)
	{
		put_rgb(x, y, renderable.glyph, renderable.fg, renderable.bg, flag);
	}

	void draw_rect(int x, int y, int width, int height, const glyph_t& ch, const color_t& fg, const color_t& bg, TCOD_bkgnd_flag_t flag)
	{
		if (auto o = get_output())
		{
			tcod::draw_rect(o->get_console(), { x, y, width, height }, output::from_cp437(ch), fg, bg, flag );
		}
	}
	void draw_frame_rgb(
		int x, int y, int width, int height, const color_t& fg, const color_t& bg, const std::array<int, 9>& decoration, bool clear, TCOD_bkgnd_flag_t flag
	)
	{
		if (auto o = get_output())
		{
			std::array<int, 9> converted_decoration{};
			for (int idx = 0; idx < 9; idx++)
			{
				auto glyph = decoration[idx];
				converted_decoration[idx] = output::from_cp437(glyph);
			}

			tcod::draw_frame(
				o->get_console(),
				{ x, y, width, height },
				converted_decoration, fg, bg,
				flag, clear
			);
		}
	}
	void draw_horizontal_line(int x, int y, int width, const color_t& fg, const color_t& bg, TCOD_bkgnd_flag_t flag)
	{
		if (auto o = get_output())
		{
			draw_rect(x, y, width, 1, HORIZONTAL_BAR, fg, bg, flag);
		}
	}

	void draw_vertical_line(int x, int y, int height, const color_t& fg, const color_t& bg, TCOD_bkgnd_flag_t flag)
	{
		if (auto o = get_output())
		{
			draw_rect(x, y, 1, height, VERTICAL_BAR, fg, bg, flag);
		}
	}
}

namespace output // Misc
{
	int get_console_width()
	{
		if (auto o = get_output())
		{
			return o->get_console().get_width();
		}

		return 1;
	}

	int get_console_height()
	{
		if (auto o = get_output())
		{
			return o->get_console().get_height();
		}

		return 1;
	}

	void clear_console(const TCOD_ConsoleTile& tile)
	{
		if (auto o = get_output())
		{
			return o->get_console().clear(tile);
		}
	}

	void present(tcod::Console* console, TCOD_ViewportOptions* viewport_options)
	{
		if (auto o = get_output())
		{
			if (!console)
			{
				console = &o->get_console();
			}

			if (!viewport_options)
			{
				viewport_options = &o->get_viewport_options();
			}

			return o->get_context().present(*console, *viewport_options);
		}
	}
}