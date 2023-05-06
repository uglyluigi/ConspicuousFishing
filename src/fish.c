#ifndef FISHC
#define FISHC

#include <stddef.h>
#include <stdlib.h>
#include "pd_api.h"
#include "pd_api/pd_api_gfx.h"
#include "pd_api/pd_api_lua.h"
#include "vec.c"
#include "util.c"

enum FishDirection {
	FacingLeft,
	FacingRight
};

enum FishType {
	Goldfish,
	Jellyfish,
	Guppy,
	Betta,
	Catfish
};

typedef struct {
	int id;
	Vec2D* velocity;
	Vec2D* pos;
	enum FishType fishType;
	enum FishDirection fishDirection;
	LCDBitmap* sprite;
	LCDSprite* actual_sprite;
} FishEntity;

const char* get_sprite_for_fish_type(enum FishType type) {
	return "img/fish.png";
}

LCDBitmap* alloc_sprite(PlaydateAPI* pd, const char* sprite_path) {
	LCDBitmap* bmp = pd->graphics->loadBitmap(sprite_path, NULL);
	
	if (bmp != NULL) {
		pd->system->logToConsole("Loading bmp: %s", sprite_path);
		return bmp;
	} else {
		pd->system->error("Failed to load bmp: %s", sprite_path);
		exit(-1);
	}
}

static int id_pool = 0;

FishEntity* alloc_fish( PlaydateAPI* pd, const char* sprite_path ) {
	size_t fish_size = sizeof(FishEntity);
	FishEntity* fish = malloc(fish_size);
	
	if (fish == NULL) {
		pd->system->error("Allocation of %d bytes failed.", fish_size);
		exit(-1);
	}
		
	fish->id = id_pool;
	id_pool++;
	fish->pos = vec2d_new(0.0f, 0.0f);
	fish->velocity = vec2d_new(1.0f, 0.0f);
	fish->fishDirection = FacingRight;
	fish->fishType = Goldfish;
	fish->sprite = alloc_sprite(pd, sprite_path);
	 
	pd->system->logToConsole("Allocated new entity with id %d (%d bytes)", fish->id, fish_size);
	
	return fish;
}

void fish_tick(PlaydateAPI* pd, FishEntity* fishes[], const int num_fish) {
	for (int i = 0; i < num_fish; i++) {
		FishEntity* fish = fishes[i];
		
		pd->graphics->drawBitmap(fish->sprite, fish->pos->x, fish->pos->y, kBitmapUnflipped);
		
		Vec2D* scaled_velocity = vec2d_new(fish->velocity->x, fish->velocity->y);
		vec2d_scale(scaled_velocity, 2.0f, scaled_velocity);
		vec2d_add(fish->pos, fish->velocity, fish->pos);
		free(scaled_velocity);
		
		if (fish->pos->x == 0 || fish->pos->x > LCD_COLUMNS) {
			vec2d_negate_x(fish->velocity, fish->velocity);
		} else if (fish->pos->y == 0 || fish->pos->y > LCD_ROWS) {
			vec2d_negate_y(fish->velocity, fish->velocity);
		}
	}
}

#endif