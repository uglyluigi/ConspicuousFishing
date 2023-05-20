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

FishEntity *fishes[MAX_FISH];

const char *get_sprite_for_fish_type(enum FishType type)
{
	return "img/moorish_idol.png";
}

static int id_pool = 0;

FishEntity *alloc_fish(PlaydateAPI *pd, const char *sprite_path)
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
	fish->y_offset = 0.0f;

	LCDSprite *sprite = util->new_sprite(pd);
	pd->sprite->setImage(sprite, alloc_bitmap(pd, sprite_path), kBitmapUnflipped);
	pd->sprite->moveTo(sprite, 200.0f, 200.0f);
	pd->sprite->setCollideRect(sprite, pd->sprite->getBounds(sprite));
	pd->sprite->addSprite(sprite);
	fish->sprite = sprite;
	fish->does_bob = true;

	Entity e = {.fish = fish};
	register_entity(kFishEntity, e);
	store_fish(fish);

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
		break;
	case Jellyfish:
	case Guppy:
	case Betta:
	case Catfish:
		break;
	}

	// Apply a periodic "bobbing" effect to the fish if necessary
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
void fish_tick(PlaydateAPI *pd, float dt, FishEntity *fishes[], const int num_fish)
{
	for (int i = 0; i < fish_entities->size; i++)
	{
		FishEntity *fish = linked_list->get(fish_entities, i);

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

		pd->sprite->addSprite(fish->sprite);

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

		srand((unsigned)pd->system->getCurrentTimeMilliseconds());
		if (rand() % 1000 <= 5)
		{
			spawn_bubble(pd, fish->sprite);
		}
	}
}

void destroy_fish(PlaydateAPI *pd, FishEntity *entity)
{
	pd->sprite->removeSprite(entity->sprite);
	free(entity->acceleration);
	free(entity->velocity);
	pd->sprite->freeSprite(entity->sprite);
	free(entity);
	deregister_entity(entity);
	linked_list->remove(fish_entities, entity);
}