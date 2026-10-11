#include "lemonade_game.h"

#include "game_loop.h"
#include "raw_master.h"
#include "snowy_rng.h"

#include "initialize.h"

int main(int argc, const char* argv[])
{
	if (!initialize(argc, argv))
	{
		return EXIT_FAILURE;
	}

	snowy::rng::initialize_rng();
	RawMaster::load_raws();

	game_loop::new_game();
	game_loop::run();

	return EXIT_SUCCESS;
}