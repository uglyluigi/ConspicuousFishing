#ifndef UTILC
#define UTILC

#include "pd_api.h"
#include "vec.h"
#include "stdbool.h"
#include "world.h"

typedef struct
{
	int (*rand_btwn)(int, int);
	LCDBitmapTable *(*alloc_bitmap_table)(PlaydateAPI *, const char *);
	LCDBitmap *(*alloc_bitmap)(PlaydateAPI *, const char *);
	void (*draw_hitbox)(PlaydateAPI *, LCDSprite *);
	int (*signf)(float);
	float (*clamp)(float, float, float);
	void (*cleanup)(void);
	LCDSprite *(*new_sprite)(PlaydateAPI *);
} _util_api;

_util_api *util;

int rand_btwn(int nMin, int nMax);
Vec2D *rand_pos();
LCDSprite *new_sprite(PlaydateAPI *pd);
LCDBitmap *alloc_bitmap(PlaydateAPI *pd, const char *sprite_path);
LCDBitmapTable *alloc_bitmap_table(PlaydateAPI *pd, const char *table_path);
void draw_hitbox(PlaydateAPI *pd, LCDSprite *sprite);
int signf(float x);
float clamp(float d, float min, float max);
void cleanup_util_api();
void init_util_api();
bool is_visible(float y, const WorldInfo *world);

#endif