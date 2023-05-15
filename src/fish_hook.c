#ifndef FISH_HOOKC
#define FISH_HOOKC

#include "pd_api.h"
#include "pd_api/pd_api_sys.h"
#include "pd_api/pd_api_gfx.h"
#include "pd_api/pd_api_sprite.h"
#include "vec.c"
#include "util.c"

typedef struct {
	Vec2D* acceleration;
	Vec2D* velocity;
	LCDBitmapTable* frames;
	LCDSprite* sprite;
	float animation_timer;
	int table_idx;
} FishHookEntity;

FishHookEntity* player;

void update_anim_frame(PlaydateAPI* pd, FishHookEntity* hook) {
	hook->table_idx += 1;
	if (hook->table_idx == 3)
	{
		hook->table_idx = 0;
		return;
	}
	pd->sprite->setImage(hook->sprite, pd->graphics->getTableBitmap(hook->frames, hook->table_idx), kBitmapUnflipped);
	hook->animation_timer = 0.0f;
}
// TODO: make hook stop at upper and lower bounds of screen

void init_hook(PlaydateAPI* pd) {
	FishHookEntity* fish_hook = (FishHookEntity*) malloc(sizeof(FishHookEntity));
	fish_hook->acceleration = vec2d_new(0.0f, 0.0f);
	fish_hook->velocity = vec2d_new(0.0f, 0.0f);
	LCDBitmapTable *frames = alloc_bitmap_table(pd, "img/hook/rusty_hook/rusty_hook");
	fish_hook->frames = frames;
	LCDSprite* sprite = pd->sprite->newSprite();
	pd->sprite->setImage(sprite, pd->graphics->getTableBitmap(fish_hook->frames, 2), kBitmapUnflipped);
	pd->sprite->moveTo(sprite, 100.0f, 100.0f);
	pd->sprite->addSprite(sprite);
	pd->sprite->setCollideRect(sprite, pd->sprite->getBounds(sprite));
	fish_hook->sprite = sprite;
	fish_hook->animation_timer = 0.0f;
	fish_hook->table_idx = 0;
	player = fish_hook;
}

void do_fish_hook_ticks(PlaydateAPI* pd, float dt, FishHookEntity* hook) {
	PDButtons current;
	PDButtons pushed;
	PDButtons released;
	pd->system->getButtonState(&current, &pushed, &released);
	hook->animation_timer += dt;

	if (hook->animation_timer > 0.5f) {
		update_anim_frame(pd, hook);
	}
	
	hook->velocity->x += hook->acceleration->x * dt;
	hook->velocity->y += hook->acceleration->y * dt;
	
	pd->sprite->moveBy(hook->sprite, hook->velocity->x * dt, hook->velocity->y * dt);
	pd->sprite->setCollideRect(hook->sprite, pd->sprite->getBounds(hook->sprite));
	
	if (current & kButtonLeft || pushed & kButtonLeft) {
		pd->system->logToConsole("Left");
		hook->acceleration->x = -10000.0f * dt;
	} 
	
	if (released & kButtonLeft) {
		pd->system->logToConsole("Released left");

		if (hook->acceleration->x < 0) {
			hook->acceleration->x = 0;
		}
	}
	
	if (current & kButtonRight || pushed & kButtonRight) {
		pd->system->logToConsole("Right");
		// TODO no need to multiply by dt here; dt is applied
		// when the velocity/position are updated using the
		// current acceleration
		hook->acceleration->x = 10000.0f * dt;
	}
	
	if (released & kButtonRight) {
		pd->system->logToConsole("Released right");

		if (hook->acceleration->x > 0) {
			hook->acceleration->x = 0;
		}
	}
	
	if (!(pushed & kButtonRight)) {
		if (hook->velocity->x > 0) {
			hook->velocity->x *= 0.95f;
		}
	}
	
	if (!(pushed & kButtonLeft)) {
		if (hook->velocity->x < 0) {
			hook->velocity->x *= 0.95f;
		}
	}
	
	float crank_change = pd->system->getCrankChange();
	
	if (crank_change != 0) {
		hook->velocity->y += crank_change * 0.3f;	
	} else {
		hook->velocity->y *= 0.70f;
	}
	
	float sprite_y;
	
	pd->sprite->getPosition(hook->sprite, NULL, &sprite_y);
	
	PDRect bounds = pd->sprite->getBounds(hook->sprite);
		
	float upper_bound = bounds.height * 0.5f;
	
	// Bump the sprite down if it's going too high
	if (sprite_y < upper_bound) {
		hook->velocity->y = 0;
		hook->acceleration->y = 0;
		pd->sprite->moveBy(hook->sprite, 0.0f, 1.0f);
	}

	// Bump it up if it's too low
	if (sprite_y > LCD_ROWS - upper_bound) {
		hook->velocity->y = 0;
		hook->acceleration->y = 0;
		pd->sprite->moveBy(hook->sprite, 0.0f, -1.0f);
	}
	
	if (DRAW_HITBOXES) {
		draw_hitbox(pd, hook->sprite);
	}
}

#endif
