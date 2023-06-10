#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pd_api.h"
#include "pd_api/pd_api_gfx.h"
#include "pd_api/pd_api_sprite.h"

#include "vec.h"
#include "util.h"
#include "bubble.h"
#include "alloc.h"
#include "fish.h"
#include "const.h"
#include "storage.h"
#include "world.h"

const char *get_sprite_for_fish_type(enum FishType type)
{
	return "img/moorish_idol.png";
}

static int id_pool = 0;

FishEntity *alloc_fish(PlaydateAPI *pd, const char *sprite_path, float world_x, float world_y)
{
	FishEntity *fish = MALLOC(1, FishEntity);

	fish->id = id_pool;
	id_pool++;
	fish->max_velocity = vec2d->new (100.0f, 0.0f);
	fish->clamp_velocity_x = true;
	fish->clamp_velocity_y = true;
	fish->velocity = vec2d->new (100.0f, 0.0f);
	fish->acceleration = vec2d->new (0.0f, 0.0f);
	fish->fishDirection = FacingRight;
	fish->fishType = Goldfish;
	fish->world_pos = vec2d->new (world_x, world_y);
	fish->active = false;

	LCDSprite *sprite = util->new_sprite(pd);
	pd->sprite->setImage(sprite, alloc_bitmap(pd, sprite_path), kBitmapUnflipped);
	// pd->sprite->moveTo(sprite, fish->world_pos->x, fish->world_pos->y);
	pd->sprite->setCollideRect(sprite, pd->sprite->getBounds(sprite));
	// pd->sprite->addSprite(sprite);
	fish->sprite = sprite;
	fish->does_bob = true;

	Entity e = {.fish = fish};
	storage->entity->add(e, kFishEntity);
	pd->sprite->addSprite(sprite);
	pd->sprite->moveTo(fish->sprite, world_x, world_y);
	pd->sprite->setVisible(sprite, false);

	return fish;
}

void do_movement(PlaydateAPI *pd, float dt, FishEntity *fish)
{
	float sprite_x;
	float sprite_y;

	pd->sprite->getPosition(fish->sprite, &sprite_x, &sprite_y);

	// Update velocity
	fish->velocity->x += fish->acceleration->x * dt;
	fish->velocity->y += fish->acceleration->y * dt;

	switch (fish->fishType)
	{
	case Goldfish:
	{
		// Perform the fish's automatic, side-to-side movement
		pd->sprite->moveBy(fish->sprite, fish->velocity->x * dt, fish->velocity->y * dt);

		if (LCD_COLUMNS - sprite_x < 50)
		{
			fish->acceleration->x = -150.0f;
		}
		else if (sprite_x < 50)
		{
			fish->acceleration->x = 150.0f;
		}
		else
		{
			fish->acceleration->x = 0.0f;
		}
	}
	break;
	case Jellyfish:
	case Guppy:
	case Betta:
	case Catfish:
		break;
	}

	// Apply a periodic "bobbing" effect to the fish if necessary
	// TODO fix this as it does not work with the new world scrolling
	if (fish->does_bob)
	{
		float time = pd->system->getElapsedTime();
		float offset = sinf(1.5f * time) * 0.33f;
		pd->sprite->moveBy(fish->sprite, 0, offset);
	}

	// Clamp velocity values to maxes specified in the max_velocity vector
	if (fish->clamp_velocity_x)
	{
		if (fish->velocity->x > fish->max_velocity->x)
		{
			fish->velocity->x = fish->max_velocity->x;
		}
		else if (-fish->velocity->x < -fish->max_velocity->x)
		{
			fish->velocity->x = -fish->max_velocity->x;
		}
	}

	if (fish->clamp_velocity_y)
	{
		if (fish->velocity->y > fish->max_velocity->y)
		{
			fish->velocity->y = fish->max_velocity->y;
		}
		else if (-fish->velocity->y < -fish->max_velocity->y)
		{
			fish->velocity->y = -fish->max_velocity->y;
		}
	}
}

// Do you like fish ticks?
void fish_tick(PlaydateAPI *pd, float dt, const WorldInfo *world, FishEntity *fish)
{
	// Update the fish's orientation based on the velocity
	if (fish->velocity->x > 0)
	{
		fish->fishDirection = FacingRight;
	}
	else if (fish->velocity->x < 0)
	{
		fish->fishDirection = FacingLeft;
	}

	// For now this assumes all fish sprites face right by default
	switch (fish->fishDirection)
	{
	case FacingLeft:
		pd->sprite->setImageFlip(fish->sprite, kBitmapFlippedX);
		break;

	case FacingRight:
		pd->sprite->setImageFlip(fish->sprite, kBitmapUnflipped);
		break;
	}

	// pd->sprite->addSprite(fish->sprite);

	// Perform fish-based movement
	do_movement(pd, dt, fish);
	do_bubble_ticks(pd, dt);

	// Update collision for this fish
	pd->sprite->setCollideRect(fish->sprite, pd->sprite->getBounds(fish->sprite));

	// Draw hitboxes if enabled
	if (DRAW_HITBOXES)
	{
		draw_hitbox(pd, fish->sprite);
	}

	// Spawn a bubble maybe
	srand((unsigned)pd->system->getCurrentTimeMilliseconds());
	if (rand() % 1000 <= 5)
	{
		spawn_bubble(pd, fish->sprite);
	}

	// Update the fish's sprite using the info in its world position
	// this basically makes it so the fish will scroll onto/off of the
	// screen after the player scrolls
	// TODO clean this up, x values do not need to be updated here
	float x, y;
	pd->sprite->getPosition(fish->sprite, &x, &y);
	pd->sprite->moveTo(fish->sprite, x, -fish->world_pos->y);
	pd->sprite->getPosition(fish->sprite, &x, &y);
	// pd->system->logToConsole("fish sprite is at (%f, %f)", fish->world_pos->x, fish->world_pos->y);
}

void destroy_fish(PlaydateAPI *pd, FishEntity *entity)
{
	Entity e = {.fish = entity};
	storage->entity->deregister(e, kFishEntity);
	pd->sprite->removeSprite(entity->sprite);
	free(entity->acceleration);
	free(entity->velocity);
	free(entity->world_pos);
	pd->sprite->freeSprite(entity->sprite);
	free(entity);
}