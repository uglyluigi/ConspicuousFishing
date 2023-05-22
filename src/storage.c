#include "linkedlist.h"
#include "fish.h"
#include "bubble.h"
#include "fish_hook.h"
#include "util.h"
#include "alloc.h"
#include "storage.h"

void register_entity(enum EntityKind kind, Entity entity)
{
	EntityPointer *entity_ptr_info = MALLOC(1, EntityPointer);
	entity_ptr_info->kind = kind;
	entity_ptr_info->data = entity;
	linked_list->add(entity_storage, entity_ptr_info);
}

void *get_entity_by_sprite(enum EntityKind kind, LCDSprite *sprite)
{
	int i = 0;
	// this is probably VERY slow
	EntityPointer *entity;

	while ((entity = linked_list->get_at(entity_storage, i)) != NULL)
	{
		if (entity->kind == kind)
		{
			switch (kind)
			{
			case kFishEntity:
			{
				FishEntity *fish = entity->data.fish;

				if (fish->sprite == sprite)
				{
					return fish;
				}
			}
			break;
			case kBubbleEntity:
			{
				BubbleEntity *bubble_entity = entity->data.bubble;

				if (bubble_entity->sprite == sprite)
				{
					return bubble_entity;
				}
			}
			break;
			case kHookEntity:
			{
				FishHookEntity *hook = entity->data.hook;

				if (hook->sprite == sprite)
				{
					return hook;
				}
			}
			break;

			default:
				break;
			}
		}

		i++;
	}

	return NULL;
}

void deregister_entity(void *entity)
{
	linked_list->remove(entity_storage, entity);
}

void init_storage()
{
	entity_storage = linked_list->new ();
	fish_entities = linked_list->new ();
	bubbles = linked_list->new ();
}

void clean_storage()
{
	linked_list->free(entity_storage);
	linked_list->free(fish_entities);
	linked_list->free(bubbles);
}

void store_sprite(LCDSprite *sprite)
{
}

void store_fish(FishEntity *fish)
{
	linked_list->add(fish_entities, fish);
}