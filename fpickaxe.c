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

#include <raylib.h>
#include <stdlib.h>
#include "block_type.h"
#include "pickaxe_type.h"

#define WIDTH 1000
#define HEIGHT 1000
#define GRAVITY 0.3
#define FORCE GRAVITY * 100 
#define SPEED_UP_GRAVITY GRAVITY * 2

#define LEVEL_HEIGHT 20
#define PICKAXETYPE IRON_PICKAXE 

#define BLOCK_SIZE 100
#define BLOCK_COLS (WIDTH / BLOCK_SIZE)
#define PICKAXE_SIZE 100 
#define DIRT_LAYERS 2

typedef struct {
	Rectangle hitbox;
	Vector2 vel;
	Pickaxe_Type type;
	Texture2D texture;
	float rotation; // degrees
} Pickaxe;

typedef struct {
	Rectangle hitbox;
	int durability;	
	Block_Type type;
	Texture2D texture;
} Block;

typedef struct {
	Block* blocks;

	int width;
	int height;
} Level;

static void init_pickaxe(Pickaxe* pick) {
	Image img = LoadImage(PICKAXE_IMGS[PICKAXETYPE]);
	ImageResizeNN(&img, PICKAXE_SIZE, PICKAXE_SIZE);	
	Texture2D texture = LoadTextureFromImage(img);	
	UnloadImage(img);	

	*pick = (Pickaxe){
		.hitbox = (Rectangle){ .x = (int)(WIDTH / 2) - 50, .y = 100, .width = PICKAXE_SIZE, .height = PICKAXE_SIZE},
		.vel = (Vector2){ .x = 0, .y = 0},
		.type = PICKAXETYPE,
		.texture = texture,
		.rotation = 0.0f,
	};
}

static Block_Type random_block_type() {
	int sum = 0; for (size_t i = 0; i < sizeof(BLOCK_TYPE_WEIGHT) / sizeof(int); ++i) sum+=BLOCK_TYPE_WEIGHT[i];

	int att = arc4random() % sum;
	for (size_t i = 0; i < sizeof(BLOCK_TYPE_WEIGHT) / sizeof(int); ++i) {
		if (att < BLOCK_TYPE_WEIGHT[i])
			return (Block_Type)i;
		att -= BLOCK_TYPE_WEIGHT[i];
	}
	
	return (Block_Type)0;
}


static void init_level(Level* level, const int height) {
	const int len = BLOCK_COLS*height;

	Block* blocks = malloc(sizeof(Block) * len);

	Image img_grass = LoadImage(BLOCK_TYPE_IMGS[GRASS]);
	ImageResizeNN(&img_grass, BLOCK_SIZE, BLOCK_SIZE);
	Texture texture_grass = LoadTextureFromImage(img_grass);
	UnloadImage(img_grass);
	// GRASS GENERATION
	
	for (int grass = 0; grass < BLOCK_COLS; ++grass) {
		blocks[grass] = (Block){
			.hitbox = (Rectangle){.x = grass*BLOCK_SIZE, .y = 300, .width = BLOCK_SIZE, .height = BLOCK_SIZE },
			.durability = BLOCK_TYPE_DURABILITY[GRASS],
			.type = GRASS,
			.texture = texture_grass,
		};
	}

	int dirt_blocks = BLOCK_COLS*(DIRT_LAYERS+1);

	int dirt_row_counter = 0;
	int dirt_col_counter = 1;

	Image img_dirt = LoadImage(BLOCK_TYPE_IMGS[DIRT]);
	ImageResizeNN(&img_dirt, BLOCK_SIZE, BLOCK_SIZE);
	Texture texture_dirt = LoadTextureFromImage(img_dirt);
	UnloadImage(img_dirt);
	// DIRT GENERATION
	for (int dirt = BLOCK_COLS; dirt < dirt_blocks; dirt++) {
		blocks[dirt] = (Block){		
			.hitbox = (Rectangle){ .x = BLOCK_SIZE*dirt_row_counter, .y = 300 + (BLOCK_SIZE*dirt_col_counter), .width = BLOCK_SIZE, .height = BLOCK_SIZE },
			.durability = BLOCK_TYPE_DURABILITY[DIRT],
			.type = DIRT,
			.texture = texture_dirt,
		};
		dirt_row_counter++;	
		if (dirt_row_counter == BLOCK_COLS) {
			dirt_row_counter = 0;
			dirt_col_counter++;
		}
	}
	// EVERYTHING ELSE GENERATION
	
	int row_counter = 0;
	int col_counter = dirt_col_counter;
	
	for (int i = dirt_blocks; i < len; ++i) {
		Block_Type type = random_block_type();
		Image img = LoadImage(BLOCK_TYPE_IMGS[type]);
		ImageResizeNN(&img, BLOCK_SIZE, BLOCK_SIZE);
		Texture2D block_texture = LoadTextureFromImage(img);
		UnloadImage(img);
		blocks[i] = (Block){		
			.hitbox = (Rectangle){ .x = BLOCK_SIZE*row_counter, .y = 300 + (BLOCK_SIZE*col_counter), .width = BLOCK_SIZE, .height = BLOCK_SIZE },
			.durability = BLOCK_TYPE_DURABILITY[type],
			.type = type,
			.texture = block_texture, 
		};

		row_counter++;	
		if (row_counter == BLOCK_COLS) {
			row_counter = 0;
			col_counter++;
		}
	}

	*level = (Level){
		.blocks = blocks,
		.width = BLOCK_COLS,
		.height = height,
	};
}

static void destroy_level(Level *level) {
	if (level->blocks != NULL) {
		free(level->blocks);
		level->blocks = NULL;
	}
	level->width  = 0;
	level->height = 0;
}

static void draw_pickaxe(Pickaxe* pick, bool hitbox) {
	Rectangle rect = (Rectangle){ pick->hitbox.x + PICKAXE_SIZE/2, pick->hitbox.y + PICKAXE_SIZE/2, PICKAXE_SIZE, PICKAXE_SIZE};
	DrawTexturePro(pick->texture,
			(Rectangle){0, 0, PICKAXE_SIZE, PICKAXE_SIZE}, 
			rect,
			(Vector2){ PICKAXE_SIZE / 2, PICKAXE_SIZE / 2},
			pick->rotation,
			WHITE
	);

	if (hitbox)
		DrawRectangleLinesEx(pick->hitbox, 5, RED);
}

static void draw_block(Block* block, bool hitbox) {	
	DrawTexture(block->texture, block->hitbox.x, block->hitbox.y, WHITE);
	if (hitbox)
		DrawRectangleLinesEx(block->hitbox, 5, RED);
}

static void draw_level(Level* level, bool hitbox) {
	for (int i = 0; i < level->width*level->height; ++i) {
		draw_block(&level->blocks[i], hitbox);
	}
}

static void break_block(Block* block, int* points) {
	*points += BLOCK_TYPE_POINTS[block->type];
	*block = (Block){0};
}

static bool update_state(Pickaxe* pick, Level* level, Camera2D *camera, int* points) {
	pick->vel.y += IsMouseButtonDown(MOUSE_BUTTON_LEFT) ||
				   IsKeyDown(KEY_DOWN)
				   ? SPEED_UP_GRAVITY : GRAVITY;
		
	bool is_collide_y = false;
	bool is_collide_x = false;
	for (int i = 0; i < level->width*level->height; ++i) {
		if (CheckCollisionRecs(pick->hitbox, level->blocks[i].hitbox)) {
			is_collide_y = true;
	
			break_block(&level->blocks[i], points);
			break;
		}
		if (CheckCollisionRecs(pick->hitbox, (Rectangle){ 0, 0, 0, HEIGHT*LEVEL_HEIGHT })) {
			pick->vel.x = GetRandomValue(1, 3);
			is_collide_x = true;
		} else if (CheckCollisionRecs(pick->hitbox, (Rectangle){ WIDTH, 0, 0, HEIGHT*LEVEL_HEIGHT })) {
			pick->vel.x = GetRandomValue(-3, -1);	
			is_collide_x = true;
		}
	}

	if (is_collide_y && !is_collide_x) {
		pick->vel.x = GetRandomValue(-2, 2);
		while (pick->vel.x == 0)
			pick->vel.x = GetRandomValue(-2, 2);
	}
	if (is_collide_y)
		pick->vel.y = -GRAVITY * FORCE;

	pick->hitbox.y += pick->vel.y;
	pick->hitbox.x += pick->vel.x;

	pick->rotation += pick->vel.x;

	camera->target.y = pick->hitbox.y + pick->hitbox.height / 2;	
	camera->target.x = WIDTH / 2;

	return pick->hitbox.y <= BLOCK_SIZE * (LEVEL_HEIGHT + 5);
}


int main(void) {
	InitWindow(WIDTH, HEIGHT, "Falling pickaxe");
	SetTargetFPS(60);
	
	Level level = {0};
	init_level(&level, LEVEL_HEIGHT);
	
	Pickaxe pickaxe = {0};
	init_pickaxe(&pickaxe);

	int player_points = 0;

	Camera2D camera = (Camera2D) {
		.offset  = (Vector2) { .x = GetRenderWidth() / 2.0f, .y = GetRenderHeight() / 2.0f},
    	.target  = (Vector2) { .x = pickaxe.hitbox.x, .y = pickaxe.hitbox.y },
    	.rotation = 0,
    	.zoom     = 1.0f, 
	};

	while (!WindowShouldClose()) {
		BeginDrawing();

			ClearBackground(BLUE);

				BeginMode2D(camera);
					draw_level(&level,     false);
					draw_pickaxe(&pickaxe, false);
				EndMode2D();
				const char* text = TextFormat("%d", player_points);
				DrawText(text, WIDTH / 2 - MeasureText(text, 100) / 2, 0, 100, WHITE);
				if (!update_state(&pickaxe, &level, &camera, &player_points)) break; 

		EndDrawing();
	}
	
	destroy_level(&level);
	CloseWindow();
	return 0;
}
