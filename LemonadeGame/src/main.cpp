#include "lemonade_game.h"

#include "game_loop.h"
#include "keybinds.h"
#include "snowy_rng.h"

#include "initialize.h"

int main(int argc, const char* argv[])
{
	if (!initialize(argc, argv))
	{
		return EXIT_FAILURE;
	}

	output::console_printf_ex(
		output::get_console_center_x(), output::get_console_center_y(), TCOD_BKGND_DEFAULT, TCOD_CENTER,
		"Initializing..."
	);

	snowy::rng::initialize_rng();
	keybinds::initialize();

	game_loop::new_game();
	game_loop::run();

	return EXIT_SUCCESS;
}