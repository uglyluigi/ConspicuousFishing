#ifndef BUBBLEC
#define BUBBLEC

#include <stdbool.h>

#include "vec.c"
#include "pd_api.h"
#include "pd_api/pd_api_gfx.h"
#include "pd_api/pd_api_sprite.h"
#include "util.c"

typedef struct {
	Vec2D* acceleration;
	Vec2D* velocity;
	LCDSprite* sprite;
} BubbleEntity;

int first_available_bubble_slot_index = 0;
BubbleEntity* bubbles[MAX_BUBBLES] = {NULL, NULL, NULL, NULL, NULL};

// Does the bubble array have space for another bubble?
// Also sets the first_availble_bubble_slot_index
bool has_space(void) {
	for (int i = 0; i < MAX_BUBBLES; i++) {
		if (bubbles[i] == NULL) {
			first_available_bubble_slot_index = i;
			return true;
		}
	}
	
	first_available_bubble_slot_index = -1;
	return false;
}

// Spawn a bubble using the provided sprite pointer as its initial position.
bool spawn_bubble(PlaydateAPI* pd, LCDSprite* sprite) {
	if (has_space()) {
		float sprite_x;
		float sprite_y;
		pd->sprite->getPosition(sprite, &sprite_x, &sprite_y);
		
		BubbleEntity* bubble = (BubbleEntity*) malloc(sizeof(BubbleEntity));
		
		bubble->acceleration = vec2d_new(0.0f, 0.0f);
		bubble->velocity = vec2d_new(0.0f, -10.0f);
		
		LCDSprite* sprite = pd->sprite->newSprite();
		pd->sprite->setImage(sprite, alloc_bitmap(pd, "img/bubble.png"), kBitmapUnflipped);
		pd->sprite->moveTo(sprite, sprite_x, sprite_y);
		pd->sprite->addSprite(sprite);
		bubble->sprite = sprite;
		
		// This is safe to do since has_space will set this with the first
		// available index in bubbles, and it is always called before doing this
		bubbles[first_available_bubble_slot_index] = bubble;
		return true;
	} else {
		pd->system->logToConsole("Ignoring bubble spawn (max on screen already)");
	}
	
	return false;
}

void destroy_bubble(PlaydateAPI* pd, BubbleEntity* bubble) {
	pd->sprite->removeSprite(bubble->sprite);
	free(bubble->acceleration);
	free(bubble->velocity);
	pd->sprite->freeSprite(bubble->sprite);
	free(bubble);
}

void do_bubble_ticks(PlaydateAPI* pd, float dt) {
	for (int i = 0; i < MAX_BUBBLES; i++) {
		BubbleEntity* bubble = bubbles[i];
		
		if (bubble != NULL) {
			pd->sprite->moveBy(bubble->sprite, bubble->velocity->x * dt, bubble->velocity->y * dt);
			
			float sprite_y;
			pd->sprite->getPosition(bubble->sprite, NULL, &sprite_y);

			if (sprite_y < -5) {
				pd->system->logToConsole("Destroying bubble #%d", i);
				destroy_bubble(pd, bubble);
				bubbles[i] = NULL;
			}
		}
	}
}
#endif