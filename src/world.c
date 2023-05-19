#ifndef WORLDC
#define WORLDC

#include "pd_api.h"
#include "util.c"
#include "vec.c"
#include "math.h"

#define MAP_LENGTH 2000
#define MAX_Y MAP_LENGTH - LCD_ROWS

typedef struct
{
	// The acceleration of the background bitmap
	Vec2D *scroll_acceleration;
	// The velocity of the background bitmap
	Vec2D *scroll_velocity;
	// The position of the bitmap (really operates more as an offset)
	Vec2D *world_pos;
	// Dimensions of the world bitmap according to the playdate api
	const Vec2D *world_dimensions;
	// Pointer to the actual world background
	LCDBitmap *world_bg;
} WorldInfo;

WorldInfo *world;

WorldInfo *init_world(PlaydateAPI *pd, const char *path)
{
	WorldInfo *world = MALLOC(1, WorldInfo);
	world->world_bg = alloc_bitmap(pd, path);
	world->scroll_acceleration = vec2d->new(0.0f, 0.0f);
	world->scroll_velocity = vec2d->new(0.0f, 0.0f);
	world->world_pos = vec2d->new(0.0f, 0.0f);

	int width, height;
	pd->graphics->getBitmapData(world->world_bg, &width, &height, NULL, NULL, NULL);
	world->world_dimensions = vec2d->new((float)width, (float)height);

	return world;
}

void destroy_world(PlaydateAPI *pd, WorldInfo *world)
{
	free(world->scroll_acceleration);
	free(world->scroll_velocity);
	free(world->world_pos);
	pd->graphics->freeBitmap(world->world_bg);
	free(world->world_dimensions);
	free(world);
}

const ROWS_OVER_2 = LCD_ROWS / 2;

float get_scroll_acceleration(float hook_y)
{
	return 100.0f * (hook_y - ROWS_OVER_2);
}

void update_world(PlaydateAPI *pd, float dt, WorldInfo *world, float hook_x, float hook_y)
{
	world->scroll_velocity->x += world->scroll_acceleration->x * dt;
	world->scroll_velocity->y += world->scroll_acceleration->y * dt;

	world->world_pos->x += world->scroll_velocity->x * dt;
	world->world_pos->y += world->scroll_velocity->y * dt;

	// Prevent scrolling past the top and bottom of the world bitmap
	world->world_pos->y = clamp(world->world_pos->y, -(world->world_dimensions->y - LCD_ROWS), 0.0f);

	// Things move up when their y acceleration is < 0,
	// so it is necessary to negate here; without this
	// it would go up when it's supposed to go down,
	// and vice versa

	float new_acceleration = get_scroll_acceleration(hook_y);

	// pd->system->logToConsole("Hook: (%f, %f) new_accel: %f", hook_x, hook_y, new_acceleration);

	if (fabsf(new_acceleration) > 80.0f)
	{
		world->scroll_acceleration->y = -new_acceleration;
	}
	else
	{
		world->scroll_velocity->y = 0.0f;
	}

	// Reset the scroll speed if the hook passes through the middle of the screen
	if (signf(world->scroll_acceleration->y) != signf(new_acceleration))
	{
		world->scroll_velocity->y = 0.0f;
	}

	pd->graphics->setDrawMode(kDrawModeNXOR);
	pd->graphics->drawBitmap(world->world_bg, world->world_pos->x, world->world_pos->y, kBitmapUnflipped);
}

#endif