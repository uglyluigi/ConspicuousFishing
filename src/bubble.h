#ifndef BUBBLEC
#define BUBBLEC

#include <stdbool.h>
#include "pd_api.h"
#include "vec.h"

typedef struct BubbleEntity
{
	Vec2D *acceleration;
	Vec2D *velocity;
	LCDSprite *sprite;
} BubbleEntity;

extern int first_available_bubble_slot_index;

bool has_space(void);
bool spawn_bubble(PlaydateAPI *pd, LCDSprite *sprite);
void destroy_bubble(PlaydateAPI *pd, BubbleEntity *bubble);
void do_bubble_ticks(PlaydateAPI *pd, float dt);
void do_bubble_ticks(PlaydateAPI *pd, float dt);

#endif