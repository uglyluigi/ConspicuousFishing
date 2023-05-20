#include "pd_api.h"
#include "pd_api/pd_api_sys.h"
#include "pd_api/pd_api_gfx.h"
#include "pd_api/pd_api_sprite.h"
#include "vec.h"
#include "util.h"
#include "anim.h"
#include "fish_hook.h"
#include "alloc.h"
#include "linkedlist.h"
#include "const.h"
#include "storage.h"

void init_hook(PlaydateAPI *pd)
{
	FishHookEntity *fish_hook = MALLOC(1, FishHookEntity);
	fish_hook->acceleration = vec2d->new (0.0f, 0.0f);
	fish_hook->velocity = vec2d->new (0.0f, 0.0f);
	LCDSprite *sprite = util->new_sprite(pd);
	fish_hook->hook_animation = animation->new_consecutive(pd, "img/hook/rusty_hook/rusty_hook", 3, 0, 0.5f, true);
	pd->sprite->setImage(sprite, pd->graphics->getTableBitmap(fish_hook->hook_animation->frames, 0), kBitmapUnflipped);
	pd->sprite->moveTo(sprite, 100.0f, 100.0f);
	pd->sprite->addSprite(sprite);
	pd->sprite->setCollideRect(sprite, pd->sprite->getBounds(sprite));
	fish_hook->sprite = sprite;
	player = fish_hook;
	Entity e = {.hook = player};
	register_entity(kHookEntity, e);
	pd->system->logToConsole("Hook sprite @ %p", player->sprite);
}

void do_fish_hook_ticks(PlaydateAPI *pd, float dt)
{
	PDButtons current;
	PDButtons pushed;
	PDButtons released;
	pd->system->getButtonState(&current, &pushed, &released);

	animation->advance_by(pd, player->hook_animation, dt);
	pd->sprite->setImage(player->sprite, player->hook_animation->current_image, kBitmapUnflipped);

	player->velocity->x += player->acceleration->x * dt;
	player->velocity->y += player->acceleration->y * dt;

	pd->sprite->moveBy(player->sprite, player->velocity->x * dt, player->velocity->y * dt);
	pd->sprite->setCollideRect(player->sprite, pd->sprite->getBounds(player->sprite));

	float sprite_x, sprite_y;
	float actual_x, actual_y;
	int len;

	pd->sprite->getPosition(player->sprite, &sprite_x, &sprite_y);
	SpriteCollisionInfo *info = pd->sprite->checkCollisions(player->sprite, sprite_x, sprite_y, &actual_x, &actual_y, &len);

	if (info == NULL && sprite_x == actual_x && sprite_y == actual_y)
	{
	}
	else
	{
		pd->system->logToConsole("Collision w/sprite %p", info->other);
		FishEntity *retrieved_entity = (FishEntity *)get_entity_by_sprite(kFishEntity, info->other);
		if (retrieved_entity != NULL)
		{
			pd->system->logToConsole("Retrieved %p from storage", retrieved_entity);
			pd->system->logToConsole("Sprite @ %p", retrieved_entity->sprite);
			destroy_fish(pd,  retrieved_entity);
		}
	}

	if (current & kButtonLeft || pushed & kButtonLeft)
	{
		pd->system->logToConsole("Left");
		player->acceleration->x = -10000.0f * dt;
	}

	if (released & kButtonLeft)
	{
		pd->system->logToConsole("Released left");

		if (player->acceleration->x < 0)
		{
			player->acceleration->x = 0;
		}
	}

	if (current & kButtonRight || pushed & kButtonRight)
	{
		pd->system->logToConsole("Right");
		// TODO no need to multiply by dt here; dt is applied
		// when the velocity/position are updated using the
		// current acceleration
		player->acceleration->x = 10000.0f * dt;
	}

	if (released & kButtonRight)
	{
		pd->system->logToConsole("Released right");

		if (player->acceleration->x > 0)
		{
			player->acceleration->x = 0;
		}
	}

	if (!(pushed & kButtonRight))
	{
		if (player->velocity->x > 0)
		{
			player->velocity->x *= 0.95f;
		}
	}

	if (!(pushed & kButtonLeft))
	{
		if (player->velocity->x < 0)
		{
			player->velocity->x *= 0.95f;
		}
	}

	float crank_change = pd->system->getCrankChange();

	if (crank_change != 0)
	{
		player->velocity->y += crank_change * 0.3f;
	}
	else
	{
		player->velocity->y *= 0.70f;
	}

	PDRect bounds = pd->sprite->getBounds(player->sprite);

	float upper_bound = bounds.height * 0.5f;

	// Bump the sprite down if it's going too high
	if (sprite_y < upper_bound)
	{
		player->velocity->y = 0;
		player->acceleration->y = 0;
		pd->sprite->moveBy(player->sprite, 0.0f, 1.0f);
	}

	// Bump it up if it's too low
	if (sprite_y > LCD_ROWS - upper_bound)
	{
		player->velocity->y = 0;
		player->acceleration->y = 0;
		pd->sprite->moveBy(player->sprite, 0.0f, -1.0f);
	}

	if (DRAW_HITBOXES)
	{
		draw_hitbox(pd, player->sprite);
	}
}

void destroy_hook(PlaydateAPI *pd)
{
	free(player->acceleration);
	free(player->hook_animation);
	pd->sprite->freeSprite(player->sprite);
	animation->free(pd, player->hook_animation);
}