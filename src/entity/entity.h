#ifndef ENTITYH
#define ENTITYH

#include "../vec.h"
#include "pd_api.h"
#include "../anim.h"
#include "../world.h"

#include <stdbool.h>

typedef enum EntityType
{
	Entity_Fish,
	Entity_Hook,
	Entity_Bubble
} EntityType;

typedef struct Entity
{
	// Entity's ID (not used for anything)
	short id;
	// The displacement that is applied to a fish every frame
	Vec2D *velocity;
	// Whether or not the do_movement function should clamp the
	// x or y values of the fish's velocity to the values set in
	// max_velocity
	// The change in acceleration to apply every frame
	Vec2D *acceleration;
	// The type of fish (affects sprite, AI)
	EntityType entityType;
	// A pointer to the sprite object for this entity
	LCDSprite *sprite;
	// This entity's position in the world
	Vec2D *world_pos;
	// Whether or not to process updates for this entity
	// on a given frame
	bool active;
	// The callback for updating the entity
	void (*update)(PlaydateAPI *, float, const WorldInfo *, struct Entity *);
	// The BTA for this sprite. Optional.
	// Just don't update it in your callback
	// if it's set to null.
	BitmapTableAnimation *animation;
} Entity;

Entity *add_entity(PlaydateAPI *pd,
				   EntityType type,
				   void (*on_update)(PlaydateAPI *pd, float dt, Entity *ent),
				   const char *sprite_path,
				   BitmapTableAnimation *bta,
				   float world_x,
				   float world_y);
Entity *spawn_hook(PlaydateAPI *pd, float x, float y);
Entity *spawn_bubble(PlaydateAPI *pd, float x, float y);
void destroy_entity(PlaydateAPI *pd, Entity *ent);
Entity *spawn_fish(PlaydateAPI *pd, float x, float y);

#endif