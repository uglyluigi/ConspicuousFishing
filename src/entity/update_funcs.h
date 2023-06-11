#ifndef UPDATE_FUNCSH
#define UPDATE_FUNCSH

#include "pd_api.h"
#include "../world.h"
#include "entity.h"

void do_movement(PlaydateAPI *pd, float dt, Entity *);
void fish_tick(PlaydateAPI *pd, float dt, const WorldInfo *world, Entity *);
void do_bubble_ticks(PlaydateAPI *pd, float dt, Entity *ent);
void do_fish_hook_ticks(PlaydateAPI *pd, float dt, Entity *hook);

#endif