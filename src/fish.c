#ifndef FISHC
#define FISHC

#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pd_api.h"
#include "pd_api/pd_api_gfx.h"
#include "pd_api/pd_api_sprite.h"
#include "vec.c"
#include "util.c"
#include "bubble.c"

#define DRAW_HITBOXES true

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
	// Entity's ID (not used for anything)
	int id;
	// The max velocity the result of applying acceleration 
	// over any number of frames is allowed to reach
	// The negated values are also checked (i.e. if -fish->velocity->x < -max_velocity->x)
	const Vec2D* max_velocity;
	// The displacement that is applied to a fish every frame
	Vec2D* velocity; 
	// Whether or not the do_movement function should clamp the
	// x or y values of the fish's velocity to the values set in
	// max_velocity
	bool clamp_velocity_x;
	bool clamp_velocity_y;
	// The change in acceleration to apply every frame
	Vec2D* acceleration;
	// The type of fish (affects sprite, AI)
	enum FishType fishType;
	// The direction the fish is facing (determines the flip)
	enum FishDirection fishDirection;
	// A pointer to the sprite object for this fish
	LCDSprite* sprite;
	// 
	float y_offset;
	bool does_bob;
} FishEntity;

const char* get_sprite_for_fish_type(enum FishType type) {
	return "img/goldfish.png";
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
	fish->max_velocity = vec2d_new(1.0f, 0.0f);
	fish->clamp_velocity_x = true;
	fish->clamp_velocity_y = true;
	fish->velocity = vec2d_new(1.0f, 0.0f);
	fish->acceleration = vec2d_new(0.0f, 0.0f);
	fish->fishDirection = FacingRight;
	fish->fishType = Goldfish;
	fish->y_offset = 0.0f;
	
	LCDSprite* sprite = pd->sprite->newSprite();
	pd->sprite->setImage(sprite, alloc_bitmap(pd, sprite_path), kBitmapUnflipped);
	pd->sprite->moveTo(sprite, 100.0f, 100.0f);
	pd->sprite->setCollideRect(sprite, pd->sprite->getBounds(sprite));
	fish->sprite = sprite;
	fish->does_bob = true;
		 
	pd->system->logToConsole("Allocated new entity with id %d (%d bytes)", fish->id, fish_size);
	
	return fish;
}

void do_movement(PlaydateAPI* pd, FishEntity* fish) {
	float sprite_x;
	float sprite_y;
	
	pd->sprite->getPosition(fish->sprite, &sprite_x, &sprite_y);
	
	// Update velocity
	fish->velocity->x += fish->acceleration->x;
	fish->velocity->y += fish->acceleration->y;
	
	switch (fish->fishType) {
		case Goldfish:
			pd->sprite->moveBy(fish->sprite, fish->velocity->x, fish->velocity->y);
		
			if (LCD_COLUMNS - sprite_x < 50) {
				fish->acceleration->x = -0.05f;
			} else if (sprite_x < 50) {
				fish->acceleration->x = 0.05f;
			} else {
				fish->acceleration->x = 0.0f;
			}
			break;
		case Jellyfish:
		case Guppy:
		case Betta:
		case Catfish:
				break;
	}
	
	
	// Apply a periodic "bobbing" effect to the fish if necessary
	if (fish->does_bob) {
		float time = pd->system->getElapsedTime();
		float offset = sinf(1.5f * time) * 0.33f;
		pd->sprite->moveBy(fish->sprite, 0, offset);
	}
	
	
	// Clamp velocity values to maxes specified in the max_velocity vector
	if (fish->clamp_velocity_x) {
		if (fish->velocity->x > fish->max_velocity->x) {
			fish->velocity->x = fish->max_velocity->x;
		} else if (-fish->velocity->x < -fish->max_velocity->x) {
			fish->velocity->x = -fish->max_velocity->x;
		}
	}
	
	if (fish->clamp_velocity_y) {
		if (fish->velocity->y > fish->max_velocity->y) {
			fish->velocity->y = fish->max_velocity->y;
		} else if (-fish->velocity->y < -fish->max_velocity->y) {
			fish->velocity->y = -fish->max_velocity->y;
		}
	}
}


// Do you like fish ticks?
void fish_tick(PlaydateAPI* pd, FishEntity* fishes[], const int num_fish) {
	for (int i = 0; i < num_fish; i++) {
		FishEntity* fish = fishes[i];
		
		if (fish->velocity->x > 0) {
			fish->fishDirection = FacingRight;
		} else if (fish->velocity->x < 0) {
			fish->fishDirection = FacingLeft;
		}
		
		// For now this assumes all fish sprites face right by default
		switch (fish->fishDirection) {
            case FacingLeft:
				pd->sprite->setImageFlip(fish->sprite, kBitmapFlippedX);
				break;
			
            case FacingRight:
				pd->sprite->setImageFlip(fish->sprite, kBitmapUnflipped);
                break;
        }

        pd->sprite->addSprite(fish->sprite);
		
		// Perform fish-based movement
		do_movement(pd, fish);
		do_bubble_ticks(pd);
		
		// Update collision for this fish
		pd->sprite->setCollideRect(fish->sprite, pd->sprite->getBounds(fish->sprite));
		
		// Draw hitboxes if enabled
		if (DRAW_HITBOXES) {
			PDRect hitbox = pd->sprite->getCollideRect(fish->sprite);
			pd->graphics->drawRect(hitbox.x, hitbox.y, hitbox.width, hitbox.height, kColorBlack);
		}
		
		srand((unsigned) pd->system->getCurrentTimeMilliseconds());
		if (rand() % 1000 <= 5) {
			spawn_bubble(pd, fish->sprite);
		}
	}
}

#endif