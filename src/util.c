#ifndef UTILC
#define UTILC

#include <stdlib.h>

#include "pd_api/pd_api_gfx.h"
#include "vec.c"
#include "pd_api.h"
#include <stdbool.h>

#define DRAW_HITBOXES true

#define MAX_FISH 1
#define MAX_BUBBLES 5

int rand_btwn(int nMin, int nMax) {
  return rand() % ((nMax + 1) - nMin) + nMin;
}

Vec2D* rand_pos() {
	Vec2D* pos = vec2d_new((float)rand_btwn(0, LCD_COLUMNS), (float)rand_btwn(0, LCD_ROWS));
	
	return pos;
}

LCDBitmap* alloc_bitmap(PlaydateAPI* pd, const char* sprite_path) {
	LCDBitmap* bmp = pd->graphics->loadBitmap(sprite_path, NULL);
	
	if (bmp != NULL) {
		pd->system->logToConsole("Loading bmp: %s", sprite_path);
		return bmp;
	} else {
		pd->system->error("Failed to load bmp: %s", sprite_path);
		exit(-1);
	}
}

void draw_hitbox(PlaydateAPI* pd, LCDSprite* sprite) {
	PDRect hitbox = pd->sprite->getCollideRect(sprite);
	pd->graphics->drawRect(hitbox.x, hitbox.y, hitbox.width, hitbox.height, kColorBlack);
}

// https://stackoverflow.com/questions/14579920/fast-sign-of-integer-in-c
int signf(float x)
{
	return (x > 0.0f) - (x < 0.0f);
}

float clamp(float d, float min, float max)
{
	const float t = d < min ? min : d;
	return t > max ? max : t;
}

#endif