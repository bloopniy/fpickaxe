// Copyright (C) 2026 bloop
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-2.0

#pragma once

typedef enum {
	GRASS         = 0,
	DIRT          = 1,
	STONE         = 2,
	GOLD_ORE      = 3,
	IRON_ORE      = 4,
	COAL_ORE      = 5,
	DIAMOND_ORE   = 6,
	NETHERITE_ORE = 7,
} Block_Type;

const int BLOCK_TYPE_DURABILITY[] = {
	10, // GRASS
	10, // DIRT
	20, // STONE
	30, // GOLD_ORE
	30, // IRON_ORE
	35, // COAL_ORE
	40, // DIAMOND_ORE
	50, // NETHERITE_ORE
};

const int BLOCK_TYPE_WEIGHT[] = {
	0,  // GRASS
	0,  // DIRT
	70, // STONE
	10, // GOLD_ORE
	15, // IRON_ORE
	30, // COAL_ORE
	5, // DIAMOND_ORE
	1, // NETHERITE_ORE
};

const int BLOCK_TYPE_POINTS[] = {
	1, // GRASS
	1, // DIRT
	2, // STONE
	3, // GOLD_ORE
	3, // IRON_ORE
	3, // COAL_ORE
	10, // DIAMOND_ORE
	30, // NETHERITE_ORE
};
static char* BLOCK_TYPE_IMGS[] = {
	"./assets/grass.png",  // GRASS
	"./assets/dirt.png",  // DIRT
	"./assets/stone.png", // STONE
	"./assets/gold_ore.png", // GOLD_ORE
	"./assets/iron_ore.png", // IRON_ORE
	"./assets/coal_ore.png", // COAL_ORE
	"./assets/diamond_ore.png", // DIAMOND_ORE
	"./assets/ancient_debris.png", // NETHERITE_ORE
};


