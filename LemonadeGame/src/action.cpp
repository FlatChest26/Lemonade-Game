#include "action.h"
#include "snowy_macros.h"
#include "entity.h"
#include "world.h"
#include "tile_map.h"
#include "world_state.h"

ActionResult Action::perform()
{
	CERR("Invalid action performed!");
	return ActionResult{ false };
}

Entity* Action::get_actor() const
{
	return m_actor;
}

World* Action::get_world() const
{
	if (auto actor = get_actor())
	{
		return actor->get_world();
	}

	return {};
}

TileMap* Action::get_tile_map() const
{
	if (auto world = get_world())
	{
		return world->get_tile_map();
	}

	return {};
}

WorldGameState* Action::get_world_game_state() const
{
	if (auto world = get_world())
	{
		return world->get_world_game_state();
	}

	return {};
}