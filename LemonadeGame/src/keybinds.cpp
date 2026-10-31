#include "keybinds.h"

void keybinds::initialize()
{
	InputMap::add_input_direction(Keybind::MoveBelow, { 0,  0, -1 });
	InputMap::add_input_direction(Keybind::MoveAbove, { 0,  0,  1 });

	InputMap::add_input_direction(Keybind::MoveNorth, { 0, -1,  0 });
	InputMap::add_input_direction(Keybind::MoveNorthEast, { 1, -1,  0 });
	InputMap::add_input_direction(Keybind::MoveEast, { 1,  0,  0 });
	InputMap::add_input_direction(Keybind::MoveSouthEast, { 1,  1,  0 });
	InputMap::add_input_direction(Keybind::MoveSouth, { 0,  1,  0 });
	InputMap::add_input_direction(Keybind::MoveSouthWest, { -1,  1,  0 });
	InputMap::add_input_direction(Keybind::MoveWest, { -1,  0,  0 });
	InputMap::add_input_direction(Keybind::MoveNorthWest, { -1, -1,  0 });

	InputMap::add_input_direction(Keybind::MoveNorth_Above, { 0, -1,  1 });
	InputMap::add_input_direction(Keybind::MoveNorthEast_Above, { 1, -1,  1 });
	InputMap::add_input_direction(Keybind::MoveEast_Above, { 1,  0,  1 });
	InputMap::add_input_direction(Keybind::MoveSouthEast_Above, { 1,  1,  1 });
	InputMap::add_input_direction(Keybind::MoveSouth_Above, { 0,  1,  1 });
	InputMap::add_input_direction(Keybind::MoveSouthWest_Above, { -1, 1,  1 });
	InputMap::add_input_direction(Keybind::MoveWest_Above, { -1, 0,  1 });
	InputMap::add_input_direction(Keybind::MoveNorthWest_Above, { -1, -1, 1 });

	InputMap::add_input_direction(Keybind::MoveNorth_Below, { 0, -1,  -1 });
	InputMap::add_input_direction(Keybind::MoveNorthEast_Below, { 1, -1,  -1 });
	InputMap::add_input_direction(Keybind::MoveEast_Below, { 1,  0,  -1 });
	InputMap::add_input_direction(Keybind::MoveSouthEast_Below, { 1,  1,  -1 });
	InputMap::add_input_direction(Keybind::MoveSouth_Below, { 0,  1,  -1 });
	InputMap::add_input_direction(Keybind::MoveSouthWest_Below, { -1, 1,  -1 });
	InputMap::add_input_direction(Keybind::MoveWest_Below, { -1, 0,  -1 });
	InputMap::add_input_direction(Keybind::MoveNorthWest_Below, { -1, -1, -1 });

	InputMap::add_input_action(Keybind::Quit, SDLK_ESCAPE);

	InputMap::add_input_action(Keybind::MoveBelow, SDLK_PERIOD, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveAbove, SDLK_COMMA, KMOD_SHIFT);

	InputMap::add_input_action(Keybind::MoveNorth, SDLK_UP, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveWest, SDLK_LEFT, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveEast, SDLK_RIGHT, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveSouth, SDLK_DOWN, KMOD_NONE, KMOD_SHIFT);

	InputMap::add_input_action(Keybind::MoveNorth, SDLK_KP_8, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveNorthEast, SDLK_KP_9, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveEast, SDLK_KP_6, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveSouthEast, SDLK_KP_3, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveSouth, SDLK_KP_2, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveSouthWest, SDLK_KP_1, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveWest, SDLK_KP_4, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveNorthWest, SDLK_KP_7, KMOD_NONE, KMOD_SHIFT);

	InputMap::add_input_action(Keybind::MoveNorth_Above, SDLK_UP, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveWest_Above, SDLK_LEFT, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveEast_Above, SDLK_RIGHT, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveSouth_Above, SDLK_DOWN, KMOD_SHIFT);

	InputMap::add_input_action(Keybind::MoveNorth_Above, SDLK_KP_8, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveNorthEast_Above, SDLK_KP_9, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveEast_Above, SDLK_KP_6, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveSouthEast_Above, SDLK_KP_3, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveSouth_Above, SDLK_KP_2, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveSouthWest_Above, SDLK_KP_1, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveWest_Above, SDLK_KP_4, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MoveNorthWest_Above, SDLK_KP_7, KMOD_SHIFT);

	InputMap::add_input_action(Keybind::MoveNorth_Below, SDLK_UP, KMOD_CTRL);
	InputMap::add_input_action(Keybind::MoveWest_Below, SDLK_LEFT, KMOD_CTRL);
	InputMap::add_input_action(Keybind::MoveEast_Below, SDLK_RIGHT, KMOD_CTRL);
	InputMap::add_input_action(Keybind::MoveSouth_Below, SDLK_DOWN, KMOD_CTRL);

	InputMap::add_input_action(Keybind::MoveNorth_Below, SDLK_KP_8, KMOD_CTRL);
	InputMap::add_input_action(Keybind::MoveNorthEast_Below, SDLK_KP_9, KMOD_CTRL);
	InputMap::add_input_action(Keybind::MoveEast_Below, SDLK_KP_6, KMOD_CTRL);
	InputMap::add_input_action(Keybind::MoveSouthEast_Below, SDLK_KP_3, KMOD_CTRL);
	InputMap::add_input_action(Keybind::MoveSouth_Below, SDLK_KP_2, KMOD_CTRL);
	InputMap::add_input_action(Keybind::MoveSouthWest_Below, SDLK_KP_1, KMOD_CTRL);
	InputMap::add_input_action(Keybind::MoveWest_Below, SDLK_KP_4, KMOD_CTRL);
	InputMap::add_input_action(Keybind::MoveNorthWest_Below, SDLK_KP_7, KMOD_CTRL);

	InputMap::add_input_action(Keybind::MenuUp, SDLK_UP, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MenuDown, SDLK_DOWN, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MenuLeft, SDLK_LEFT, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MenuRight, SDLK_RIGHT, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MenuConfirm, SDLK_RETURN, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::MenuBack, SDLK_ESCAPE, KMOD_NONE, KMOD_SHIFT);

	InputMap::add_input_action(Keybind::WaitShort, SDLK_PERIOD, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::WaitShort, SDLK_KP_5, KMOD_NONE, KMOD_SHIFT);

	InputMap::add_input_action(Keybind::WaitLong, SDLK_COMMA, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::WaitLong, SDLK_KP_5, KMOD_SHIFT);

	InputMap::add_input_action(Keybind::Look, SDLK_x, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::Open, SDLK_o, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::Close, SDLK_c, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::Examine, SDLK_r, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::Inventory, SDLK_i, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::Grab, SDLK_g, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::Drop, SDLK_d, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::Interact, SDLK_e, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::Sleep, SDLK_z, KMOD_SHIFT, KMOD_NONE);

	InputMap::add_input_action(Keybind::SetToTurnBasedSpeed, SDLK_F1, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::SetToRealTimeSpeed, SDLK_F2, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::SetToRealTime2xSpeed, SDLK_F3, KMOD_NONE, KMOD_SHIFT);
	InputMap::add_input_action(Keybind::SetToRealTime4xSpeed, SDLK_F4, KMOD_NONE, KMOD_SHIFT);

	InputMap::add_input_action(Keybind::PinSidebarMenu, SDLK_EQUALS, KMOD_SHIFT, KMOD_NONE);
}