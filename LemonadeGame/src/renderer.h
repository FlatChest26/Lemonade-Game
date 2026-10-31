#pragma once

#ifndef LEMONADE_GAME_SRC_WORLD_RENDERER_H
#define LEMONADE_GAME_SRC_WORLD_RENDERER_H

#include "render_params.h"

class GameState;
class WorldGameState;
class World;
class Camera;
class Entity;
class TileMap;

class Renderer
{
protected:
	GameState* m_game_state;

public:
	Renderer(GameState* game_state = nullptr);

	// -- Getters -- //
	GameState* get_game_state() const;

public:
	// -- Utilities -- //

	virtual void render(const RenderParams& params) const = 0;
};

// Renders things in thew world
class WorldRenderer : public Renderer
{
public:
	WorldRenderer(GameState* game_state = nullptr) :
		Renderer(game_state)
	{
	}

	// -- Getters -- //
	WorldGameState* get_world_game_state() const;
	World* get_world() const;
	Camera* get_camera() const;
	Entity* get_player() const;
	TileMap* get_tile_map() const;

public:
	// -- Utilities -- //

	virtual void render(const RenderParams& params) const;

protected:
	// -- Render Functions -- //
	virtual void render_entities(const RenderParams& params) const;
	virtual void render_entity(const RenderParams& params, const Entity* entity = nullptr) const;

	virtual void render_tile_map(const RenderParams& params) const;

	virtual void render_debug(const RenderParams& params) const;
};

#endif // !LEMONADE_GAME_SRC_RENDERER_H
