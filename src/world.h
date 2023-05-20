#ifndef WORLDC
#define WORLDC

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

WorldInfo *init_world(PlaydateAPI *pd, const char *path);
void destroy_world(PlaydateAPI *pd, WorldInfo *world);
float get_scroll_acceleration(float hook_y);
void update_world(PlaydateAPI *pd, float dt, WorldInfo *world, float hook_x, float hook_y);

#endif