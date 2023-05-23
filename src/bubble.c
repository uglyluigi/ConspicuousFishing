#include <stdbool.h>

#include "vec.h"
#include "pd_api.h"
#include "pd_api/pd_api_gfx.h"
#include "pd_api/pd_api_sprite.h"
#include "util.h"
#include "const.h"
#include "alloc.h"
#include "bubble.h"
#include "linkedlist.h"
#include "storage.h"

// Spawn a bubble using the provided sprite pointer as its initial position.
bool spawn_bubble(PlaydateAPI *pd, LCDSprite *sprite)
{
	float sprite_x;
	float sprite_y;
	pd->sprite->getPosition(sprite, &sprite_x, &sprite_y);

	BubbleEntity *bubble = MALLOC(1, BubbleEntity);

	bubble->acceleration = vec2d->new (0.0f, 0.0f);
	bubble->velocity = vec2d->new (0.0f, -10.0f);

	LCDSprite *sprite = util->new_sprite(pd);
	pd->sprite->setImage(sprite, alloc_bitmap(pd, "img/bubble.png"), kBitmapUnflipped);
	pd->sprite->moveTo(sprite, sprite_x, sprite_y);
	pd->sprite->addSprite(sprite);
	bubble->sprite = sprite;
	Entity e = {.bubble = bubble};
	register_entity(kBubbleEntity, e);
	storage->bubble->add(bubble);
}

void destroy_bubble(PlaydateAPI *pd, BubbleEntity *bubble)
{
	Entity e = {.bubble = bubble};
	storage->entity->deregister(e, kBubbleEntity);
	storage->bubble->remove(bubble);
	pd->sprite->removeSprite(bubble->sprite);
	free(bubble->acceleration);
	free(bubble->velocity);
	pd->sprite->freeSprite(bubble->sprite);
	free(bubble);
}

void do_bubble_ticks(PlaydateAPI *pd, float dt)
{
	for (int i = 0; i < bubble_storage->size; i++)
	{
		BubbleEntity *bubble = linked_list->get(bubble_storage, i);

		pd->sprite->moveBy(bubble->sprite, bubble->velocity->x * dt, bubble->velocity->y * dt);

		float sprite_y;
		pd->sprite->getPosition(bubble->sprite, NULL, &sprite_y);

		if (sprite_y < -5)
		{
			pd->system->logToConsole("Destroying bubble #%d", i);
			destroy_bubble(pd, bubble);
		}
	}
}