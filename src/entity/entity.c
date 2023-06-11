#include "entity.h"
#include "../alloc.h"
#include "stdbool.h"
#include "../util.h"
#include "../storage.h"
#include "update_funcs.h"
#include "../anim.h"

Entity *add_entity(PlaydateAPI *pd,
				   EntityType type,
				   void (*on_update)(PlaydateAPI *pd, float dt, Entity *ent),
				   const char *sprite_path,
				   BitmapTableAnimation *bta,
				   float world_x,
				   float world_y)
{
	Entity *ent = MALLOC(1, Entity);
	ent->id = 0;
	ent->entityType = type;
	ent->acceleration = vec2d->new (0.0f, 0.0f);
	ent->velocity = vec2d->new (0.0f, 0.0f);
	ent->world_pos = vec2d->new (world_x, world_y);
	ent->update = on_update;
	ent->active = false;
	ent->animation = bta;

	LCDSprite *sprite = util->new_sprite(pd);

	if (bta == NULL)
	{
		pd->sprite->setImage(sprite, util->alloc_bitmap(pd, sprite_path), kBitmapUnflipped);
	}
	else
	{
		pd->sprite->setImage(sprite, pd->graphics->getTableBitmap(bta->frames, 0), kBitmapUnflipped);
	}

	pd->sprite->setCollideRect(sprite, pd->sprite->getBounds(sprite));
	ent->sprite = sprite;

	// We add this entity's sprite to the sprite list
	// but set it to invisible, as the code that determines
	// whether or not an entity is on screen is what is
	// responsible for managing its visibility (world.c)
	pd->sprite->addSprite(sprite);
	pd->sprite->moveTo(sprite, world_x, world_y);
	pd->sprite->setVisible(sprite, false);

	storage->entity->add(ent);

	return ent;
}

Entity *spawn_bubble(PlaydateAPI *pd, float x, float y)
{
	return add_entity(pd, Entity_Bubble, &do_bubble_ticks, "img/bubble.png", NULL, x, y);
}

Entity *spawn_hook(PlaydateAPI *pd, float x, float y)
{
	return add_entity(pd,
					  Entity_Hook,
					  &do_fish_hook_ticks,
					  NULL,
					  animation->new_consecutive(pd, "img/hook/nice_hook/nice_hook", 3, 0, 0.5f, true),
					  x,
					  y);
}

Entity *spawn_fish(PlaydateAPI *pd, float x, float y)
{
	return add_entity(pd, Entity_Fish, &fish_tick, "img/moorish_idol.png", NULL, x, y);
}

void destroy_entity(PlaydateAPI *pd, Entity *ent)
{
	pd->sprite->removeSprite(ent->sprite);
	free(ent->acceleration);
	free(ent->velocity);

	if (ent->animation == NULL)
	{
		pd->graphics->freeBitmap(pd->sprite->getImage(ent->sprite));
		pd->sprite->freeSprite(ent->sprite);
	}
	else
	{
		animation->free(pd, ent->animation);
	}

	storage->entity->deregister(ent);
	free(ent);
}