#pragma once

#ifndef _LEMONADE_ENGINE_H
#define _LEMONADE_ENGINE_H

#include "snowy_utilities.h"

#include "snowy_macros.h"
#include "snowy_typedefs.h"

#include "file_management.h"
#include "config.h"
#include "input_map.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <libtcod.hpp>

#include "output.h"
#include "initialize.h"

#include "camera.h"

#include "lemonade_engine_data.h"

#endif // !_LEMONADE_ENGINE_H