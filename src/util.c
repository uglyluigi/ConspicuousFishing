#ifndef UTILC
#define UTILC

#include <stdlib.h>

#include "pd_api/pd_api_gfx.h"
#include "vec.c"
#include "pd_api.h"

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

#endif