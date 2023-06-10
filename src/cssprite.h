#ifndef CSSPRITEH
#define CSSPRITEH

#include "vec.h"
#include "pd_api.h"

typedef struct CSSprite
{
	// The CSSprite's position in the world
	Vec2D* pos;
	LCDSprite* sprite;
} CSSprite;

#endif