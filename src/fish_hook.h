#ifndef FISH_HOOKC
#define FISH_HOOKC

#include "pd_api.h"
#include "anim.h"

typedef struct FishHookEntity
{
	Vec2D *acceleration;
	Vec2D *velocity;
	LCDSprite *sprite;
	BitmapTableAnimation *hook_animation;
} FishHookEntity;

FishHookEntity *player;

void init_hook(PlaydateAPI *pd);
void do_fish_hook_ticks(PlaydateAPI *pd, float dt, FishHookEntity *);
void destroy_hook(PlaydateAPI *pd, FishHookEntity*);

#endif
