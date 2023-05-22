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

// Does the bubble array have space for another bubble?
// Also sets the first_availble_bubble_slot_index
bool has_space(void)
{
	return true;
}

// Spawn a bubble using the provided sprite pointer as its initial position.
bool spawn_bubble(PlaydateAPI *pd, LCDSprite *sprite)
{
	if (has_space())
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
		linked_list->add(bubbles, bubble);

		// This is safe to do since has_space will set this with the first
		// available index in bubbles, and it is always called before doing this
		return true;
	}
	else
	{
		pd->system->logToConsole("Ignoring bubble spawn (max on screen already)");
	}

	return false;
}

void destroy_bubble(PlaydateAPI *pd, BubbleEntity *bubble)
{
	deregister_entity(bubble);
	linked_list->remove(bubbles, bubble);
	pd->sprite->removeSprite(bubble->sprite);
	free(bubble->acceleration);
	free(bubble->velocity);
	pd->sprite->freeSprite(bubble->sprite);
	free(bubble);
}

void do_bubble_ticks(PlaydateAPI *pd, float dt)
{
	for (int i = 0; i < bubbles->size; i++)
	{
		BubbleEntity *bubble = linked_list->get_at(bubbles, i);

		if (bubble != NULL)
		{
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
}