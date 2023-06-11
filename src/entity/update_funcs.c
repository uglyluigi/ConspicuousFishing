#include "../storage.h"
#include "pd_api.h"
#include "../world.h"
#include "../const.h"
#include "../util.h"

void do_movement(PlaydateAPI *pd, float dt, Entity *fish)
{
	float sprite_x;
	float sprite_y;

	pd->sprite->getPosition(fish->sprite, &sprite_x, &sprite_y);

	// Update velocity
	fish->velocity->x += fish->acceleration->x * dt;
	fish->velocity->y += fish->acceleration->y * dt;

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

	// Apply a periodic "bobbing" effect to the fish if necessary
	// TODO fix this as it does not work with the new world scrolling
	if (false)
	{
		float time = pd->system->getElapsedTime();
		float offset = sinf(1.5f * time) * 0.33f;
		pd->sprite->moveBy(fish->sprite, 0, offset);
	}

	// Clamp velocity values to maxes specified in the max_velocity vector
}

// Do you like fish ticks?
void fish_tick(PlaydateAPI *pd, float dt, const WorldInfo *world, Entity *fish)
{
	// Update the fish's orientation based on the velocity
	if (fish->velocity->x > 0)
	{
		pd->sprite->setImageFlip(fish->sprite, kBitmapFlippedX);
	}
	else if (fish->velocity->x < 0)
	{
		pd->sprite->setImageFlip(fish->sprite, kBitmapUnflipped);
	}

	// pd->sprite->addSprite(fish->sprite);

	// Perform fish-based movement
	do_movement(pd, dt, fish);

	// Update collision for this fish
	pd->sprite->setCollideRect(fish->sprite, pd->sprite->getBounds(fish->sprite));

	// Draw hitboxes if enabled
	if (DRAW_HITBOXES)
	{
		util->draw_hitbox(pd, fish->sprite);
	}

	// Spawn a bubble maybe
	srand((unsigned)pd->system->getCurrentTimeMilliseconds());
	if (rand() % 1000 <= 5)
	{
		float x, y;
		pd->sprite->getPosition(fish->sprite, &x, &y);
		spawn_bubble(pd, x, y);
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

void do_bubble_ticks(PlaydateAPI *pd, float dt, const WorldInfo *world, Entity *ent)
{
	for (int i = 0; i < entity_storage->size; i++)
	{
		Entity *bubble = linked_list->get(entity_storage, i);

		if (bubble->entityType == Entity_Bubble)
		{
			pd->sprite->moveBy(bubble->sprite, bubble->velocity->x * dt, bubble->velocity->y * dt);

			float sprite_y;
			pd->sprite->getPosition(bubble->sprite, NULL, &sprite_y);

			if (sprite_y < -5)
			{
				pd->system->logToConsole("Destroying bubble #%d", i);
				destroy_entity(pd, bubble);
			}
		}
	}
}

void do_fish_hook_ticks(PlaydateAPI *pd, float dt, const WorldInfo *world, Entity *hook)
{
	PDButtons current;
	PDButtons pushed;
	PDButtons released;
	pd->system->getButtonState(&current, &pushed, &released);

	animation->advance_by(pd, hook->animation, dt);
	pd->sprite->setImage(hook->sprite, hook->animation->current_image, kBitmapUnflipped);

	hook->velocity->x += hook->acceleration->x * dt;
	hook->velocity->y += hook->acceleration->y * dt;

	pd->sprite->moveBy(hook->sprite, hook->velocity->x * dt, hook->velocity->y * dt);
	pd->sprite->setCollideRect(hook->sprite, pd->sprite->getBounds(hook->sprite));

	float sprite_x, sprite_y;
	float actual_x, actual_y;
	int len;

	pd->sprite->getPosition(hook->sprite, &sprite_x, &sprite_y);
	SpriteCollisionInfo *info = pd->sprite->checkCollisions(hook->sprite, sprite_x, sprite_y, &actual_x, &actual_y, &len);

	if (info != NULL && info->other != NULL)
	{
		if (sprite_x == actual_x && sprite_y == actual_y)
		{
		}
		else
		{
			pd->system->logToConsole("Collision w/sprite %p", info->other);
			Entity *retrieved_entity = storage->entity->get_by_sprite(info->other);

			if (retrieved_entity != NULL)
			{
				pd->system->logToConsole("Retrieved %p from storage", retrieved_entity);
				pd->system->logToConsole("Sprite @ %p", retrieved_entity->sprite);
				destroy_entity(pd, retrieved_entity);
			}
		}
	}

	if (current & kButtonLeft || pushed & kButtonLeft)
	{
		// pd->system->logToConsole("Left");
		hook->acceleration->x = -100.0f;
	}

	if (released & kButtonLeft)
	{
		// pd->system->logToConsole("Released left");

		if (hook->acceleration->x < 0)
		{
			hook->acceleration->x = 0;
		}
	}

	if (current & kButtonRight || pushed & kButtonRight)
	{
		// pd->system->logToConsole("Right");
		//  TODO no need to multiply by dt here; dt is applied
		//  when the velocity/position are updated using the
		//  current acceleration
		hook->acceleration->x = 100.0f;
	}

	if (released & kButtonRight)
	{
		// pd->system->logToConsole("Released right");

		if (hook->acceleration->x > 0)
		{
			hook->acceleration->x = 0;
		}
	}

	if (!(pushed & kButtonRight))
	{
		if (hook->velocity->x > 0)
		{
			hook->velocity->x *= 0.95f;
		}
	}

	if (!(pushed & kButtonLeft))
	{
		if (hook->velocity->x < 0)
		{
			hook->velocity->x *= 0.95f;
		}
	}

	float crank_change = pd->system->getCrankChange();

	if (crank_change != 0)
	{
		hook->velocity->y += crank_change * 0.3f;
	}
	else
	{
		hook->velocity->y *= 0.70f;
	}

	PDRect bounds = pd->sprite->getBounds(hook->sprite);

	float upper_bound = bounds.height * 0.5f;

	// Bump the sprite down if it's going too high
	if (sprite_y < upper_bound)
	{
		hook->velocity->y = 0;
		hook->acceleration->y = 0;
		pd->sprite->moveBy(hook->sprite, 0.0f, 1.0f);
	}

	// Bump it up if it's too low
	if (sprite_y > LCD_ROWS - upper_bound)
	{
		hook->velocity->y = 0;
		hook->acceleration->y = 0;
		pd->sprite->moveBy(hook->sprite, 0.0f, -1.0f);
	}

	if (DRAW_HITBOXES)
	{
		draw_hitbox(pd, hook->sprite);
	}
}