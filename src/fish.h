#ifndef FISHC
#define FISHC

#include <stdbool.h>

#include "pd_api.h"
#include "vec.h"

enum FishDirection
{
	FacingLeft,
	FacingRight
};

enum FishType
{
	Goldfish,
	Jellyfish,
	Guppy,
	Betta,
	Catfish
};

typedef struct FishEntity
{
	// Entity's ID (not used for anything)
	int id;
	// The max velocity the result of applying acceleration
	// over any number of frames is allowed to reach
	// The negated values are also checked (i.e. if -fish->velocity->x < -max_velocity->x)
	const Vec2D *max_velocity;
	// The displacement that is applied to a fish every frame
	Vec2D *velocity;
	// Whether or not the do_movement function should clamp the
	// x or y values of the fish's velocity to the values set in
	// max_velocity
	bool clamp_velocity_x;
	bool clamp_velocity_y;
	// The change in acceleration to apply every frame
	Vec2D *acceleration;
	// The type of fish (affects sprite, AI)
	enum FishType fishType;
	// The direction the fish is facing (determines the flip)
	enum FishDirection fishDirection;
	// A pointer to the sprite object for this fish
	LCDSprite *sprite;
	//
	float y_offset;
	bool does_bob;
} FishEntity;

extern FishEntity *fishes[];

const char *get_sprite_for_fish_type(enum FishType type);
FishEntity *alloc_fish(PlaydateAPI *pd, const char *sprite_path);
void do_movement(PlaydateAPI *pd, float dt, FishEntity *fish);
void fish_tick(PlaydateAPI *pd, float dt, FishEntity *fishes[], const int num_fish);

#endif