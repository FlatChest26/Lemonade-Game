#pragma once

#ifndef LEMONADE_GAME_SRC_OUTPUT_H
#define LEMONADE_GAME_SRC_OUTPUT_H

#include <SDL.h>
#include <libtcod.hpp>

#include <memory>
#include <string>
#include "color.h"

#define DEFAULT_BKGND_FLAG TCOD_BKGND_DEFAULT
#define DEFAULT_RECT_FRAME { 218, 196, 191, 179, ' ', 179, 192, 196, 217 }

namespace output
{
	extern bool setup_context(int argc, const char* argv[]);
	extern void convert_event(SDL_Event& event);
}

namespace output
{
	// -- Window Functions -- //

	/* Resizes the main console to fit the window.*/
	extern void update_window();

	/*Set the title of a window. This string is expected to be in UTF-8 encoding.*/
	extern void set_window_title(const char* window_title);
	inline void set_window_title(std::string window_title) { return set_window_title(window_title.c_str()); }

	// -- Console Functions -- //

	/* Converts the given char into its code page 437 mapping.*/
	extern constexpr glyph_t to_cp437(int c);

	/* Converts the given char into its unicode mapping.*/
	extern constexpr int from_cp437(glyph_t c);

	// Color //

	/*Return a character code of a console at x, y.*/
	extern glyph_t get_char(int x, int y, bool to_cp437 = false);
	extern color_t get_fg(int x, int y);
	extern color_t get_bg(int x, int y);
	extern TCOD_bkgnd_flag_t get_bg_flag(int x, int y);

	extern void set_char(int x, int y, glyph_t glyph = DEFAULT_GLPYH, bool from_cp437 = false);
	extern void set_fg(int x, int y, color_t fg = DEFAULT_FG_COLOR);
	extern void set_bg(int x, int y, color_t bg = DEFAULT_BG_COLOR);
	extern void set_bg_flag(int x, int y, TCOD_bkgnd_flag_t flag = DEFAULT_BKGND_FLAG);
	extern void set_color(int x, int y, color_t fg = DEFAULT_FG_COLOR, color_t bg = DEFAULT_BG_COLOR);

	// Printing //

	extern void console_print(int x, int y, const char* fmt);
	extern void console_print(int x, int y, std::string str);

	extern void console_printf(int x, int y, const char* fmt, ...);
	extern void console_printf(int x, int y, std::string fmt, ...);
	extern void console_printf_ex(int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char* fmt, ...);
	extern void console_printf_ex(int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, std::string fmt, ...);

	extern int console_printf_rect(int x, int y, int width, int height, const char* fmt, ...);
	extern int console_printf_rect(int x, int y, int width, int height, std::string fmt, ...);
	extern int console_printf_rect_ex(int x, int y, int width, int height, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char* fmt, ...);
	extern int console_printf_rect_ex(int x, int y, int width, int height, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, std::string fmt, ...);

	/*Printing RGB*/

	extern void console_print_rgb(int x, int y, const color_t& fg, const color_t& bg, const char* fmt);
	extern void console_print_rgb(int x, int y, const color_t& fg, const color_t& bg, std::string str);

	extern void console_printf_rgb(int x, int y, const color_t& fg, const color_t& bg, const char* fmt, ...);
	extern void console_printf_rgb(int x, int y, const color_t& fg, const color_t& bg, std::string fmt, ...);
	extern void console_printf_rgb_ex(int x, int y, const color_t& fg, const color_t& bg, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char* fmt, ...);
	extern void console_printf_rgb_ex(int x, int y, const color_t& fg, const color_t& bg, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, std::string fmt, ...);

	extern int console_printf_rgb_rect(int x, int y, int width, int height, const color_t& fg, const color_t& bg, const char* fmt, ...);
	extern int console_printf_rgb_rect(int x, int y, int width, int height, const color_t& fg, const color_t& bg, std::string fmt, ...);

	extern int console_printf_rgb_rect_ex(
		int x, int y, int width, int height,
		const color_t& fg, const color_t& bg,
		TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment,
		const char* fmt, ...
	);

	extern int console_printf_rgb_rect_ex(
		int x, int y, int width, int height,
		const color_t& fg, const color_t& bg, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment,
		std::string fmt, ...
	);

	// Drawing //

	extern void put_char(int x, int y, glyph_t glyph, TCOD_bkgnd_flag_t flag = DEFAULT_BKGND_FLAG);
	extern void put_char_ex(int x, int y, glyph_t glyph, color_t fg, color_t bg);

	extern void put_rgb(const int& x, const int& y, const glyph_t& ch, const color_t& fg, const color_t& bg, TCOD_bkgnd_flag_t flag = DEFAULT_BKGND_FLAG);

	extern void draw_rect(int x, int y, int width, int height, bool clear = true, TCOD_bkgnd_flag_t flag = DEFAULT_BKGND_FLAG);
	extern void draw_frame_rgb(
		int x, int y, int width, int height,
		const color_t& fg = DEFAULT_FG_COLOR, const color_t& bg = DEFAULT_BG_COLOR,
		const std::array<int, 9>& decoration = {
			218, 196, 191,
			179, 0, 179,
			192, 196, 217
		},
		bool clear = true,
		TCOD_bkgnd_flag_t flag = DEFAULT_BKGND_FLAG
	);

	// Misc //

	extern int get_console_width();
	extern int get_console_height();

	extern void clear_console(const TCOD_ConsoleTile& tile = { 0x20, {255, 255, 255, 255}, {0, 0, 0, 255} });
	extern void present(tcod::Console* console = nullptr, TCOD_ViewportOptions* viewport_options = nullptr);
}

class SAIOutput
{
private:
	tcod::Console m_console;
	tcod::Context m_context;

	TCOD_ViewportOptions m_viewport_options
	{
		.tcod_version = TCOD_COMPILEDVERSION,
		.keep_aspect = true,
		.integer_scaling = true,
		.clear_color = TCOD_ColorRGBA{0, 0, 0, 255},
		.align_x = 0.5,
		.align_y = 0.5
	};

public:
	SAIOutput(const TCOD_ContextParams& params, const tcod::Console& console);
	SAIOutput(const TCOD_ContextParams& params);
	~SAIOutput();

public:
	// -- Getters -- //

	tcod::Console& get_console() { return m_console; }
	tcod::Context& get_context() { return m_context; }
	TCOD_ViewportOptions& get_viewport_options() { return m_viewport_options; }

	// -- Misc -- //

	void update_window();
};

namespace output
{
	extern SAIOutput* get_output();
}

#endif // !LEMONADE_GAME_SRC_OUTPUT_H