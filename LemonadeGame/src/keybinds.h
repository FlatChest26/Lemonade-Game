#pragma once

#ifndef LEMONADE_GAME_SRC_KEYBINDS_H
#define LEMONADE_GAME_SRC_KEYBINDS_H

#include "input_map.h"

enum class Keybind : uint64_t
{
	None,

	Quit,

	MenuUp,
	MenuDown,
	MenuLeft,
	MenuRight,
	MenuConfirm,
	MenuBack,

	MoveCenter,
	MoveNorth,
	MoveNorthEast,
	MoveEast,
	MoveSouthEast,
	MoveSouth,
	MoveSouthWest,
	MoveWest,
	MoveNorthWest,

	MoveAbove,
	MoveBelow,

	MoveCenter_Above,
	MoveNorth_Above,
	MoveNorthEast_Above,
	MoveEast_Above,
	MoveSouthEast_Above,
	MoveSouth_Above,
	MoveSouthWest_Above,
	MoveWest_Above,
	MoveNorthWest_Above,

	MoveCenter_Below,
	MoveNorth_Below,
	MoveNorthEast_Below,
	MoveEast_Below,
	MoveSouthEast_Below,
	MoveSouth_Below,
	MoveSouthWest_Below,
	MoveWest_Below,
	MoveNorthWest_Below,

	WaitShort,
	WaitLong,

	Look,
	Open,
	Close,
	Inventory,
	Examine,
	Grab,
	Grapple,
	Drop,
	Interact,
	Sleep,
	Consume,

	SetToTurnBasedSpeed,
	SetToRealTimeSpeed,
	SetToRealTime2xSpeed,
	SetToRealTime4xSpeed,

	PinSidebarMenu,
};

using InputMap = GenericInputMap<Keybind>;

namespace keybinds
{
	extern void initialize();
}

#endif // !LEMONADE_GAME_SRC_KEYBINDS_H
