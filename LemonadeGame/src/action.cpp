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

void Action::set_actor(Entity* actor)
{

}

bool Action::has_actor() const
{
	return get_actor() != nullptr;
}

Entity* Action::get_actor() const
{
	return nullptr;
}

void Action::set_subject(Entity* subject)
{

}

bool Action::has_subject() const
{
	return get_subject() != nullptr;
}

Entity* Action::get_subject() const
{
	return nullptr;
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

WorldGameState* Action::get_world_state() const
{
	if (auto world = get_world())
	{
		return world->get_world_state();
	}

	return {};
}

bool EntityAction::check_action() const
{
	if (!Action::check_action())
		return false;

	if (!get_actor())
	{
		ACTION_CERR("Actor is null!");
		return false;
	}

	if (!get_actor()->can_do_actions())
	{
		ACTION_CERR("This actor cannot perform actions");
		return false;
	}

	return true;
}

void EntityAction::set_actor(Entity* actor)
{
	m_actor = actor;
}

Entity* EntityAction::get_actor() const
{
	return m_actor;
}