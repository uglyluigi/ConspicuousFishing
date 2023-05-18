#ifndef UTILC
#define UTILC

#include <stdlib.h>
#include <stdbool.h>

#include "pd_api/pd_api_gfx.h"
#include "vec.c"
#include "pd_api.h"
#include "linkedlist.c"
#include "storage.c"

#define DRAW_HITBOXES true

#define MAX_FISH 1
#define MAX_BUBBLES 5

typedef struct
{
	int (*rand_btwn)(int, int);
	LCDBitmapTable *(*alloc_bitmap_table)(PlaydateAPI *, const char *);
	LCDBitmap *(*alloc_bitmap)(PlaydateAPI *, const char *);
	void (*draw_hitbox)(PlaydateAPI *, LCDSprite *);
	int (*signf)(float);
	float (*clamp)(float, float, float);
	void (*cleanup)(void);
	LCDSprite* (*new_sprite)(PlaydateAPI*);
} _util_api;

_util_api *util;

int rand_btwn(int nMin, int nMax)
{
	return rand() % ((nMax + 1) - nMin) + nMin;
}

Vec2D *rand_pos()
{
	Vec2D *pos = vec2d->new ((float)rand_btwn(0, LCD_COLUMNS), (float)rand_btwn(0, LCD_ROWS));

	return pos;
}

LCDSprite *new_sprite(PlaydateAPI *pd)
{
	LCDSprite *sprite = pd->sprite->newSprite();
	linked_list->add(sprite_storage, sprite);
	return sprite;
}

LCDBitmap *alloc_bitmap(PlaydateAPI *pd, const char *sprite_path)
{
	LCDBitmap *bmp = pd->graphics->loadBitmap(sprite_path, NULL);

	if (bmp != NULL)
	{
		pd->system->logToConsole("Bmp %s loaded (addr: %p)", sprite_path, bmp);
		return bmp;
	}
	else
	{
		pd->system->error("Failed to load bmp: %s", sprite_path);
		exit(-1);
	}
}

LCDBitmapTable *alloc_bitmap_table(PlaydateAPI *pd, const char *table_path)
{
	char outerror[50];

	LCDBitmapTable *table = pd->graphics->loadBitmapTable(table_path, &outerror);

	if (table != NULL)
	{
		return table;
	}
	else
	{
		pd->system->logToConsole("Error loading bitmap table %s: %s", table_path, outerror);
		exit(-1);
	}
}

void draw_hitbox(PlaydateAPI *pd, LCDSprite *sprite)
{
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

void cleanup_util_api()
{
	free(util);
}

void init_util_api()
{
	util = (_util_api *)malloc(sizeof(_util_api));
	util->alloc_bitmap_table = &alloc_bitmap_table;
	util->rand_btwn = &rand_btwn;
	util->alloc_bitmap = &alloc_bitmap;
	util->clamp = &clamp;
	util->signf = &signf;
	util->rand_btwn = &rand_btwn;
	util->cleanup = &cleanup_util_api;
	util->new_sprite = &new_sprite;
}

#endif