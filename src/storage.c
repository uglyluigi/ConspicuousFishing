#include "linkedlist.h"
#include "fish.h"
#include "bubble.h"
#include "fish_hook.h"
#include "util.h"
#include "alloc.h"
#include "storage.h"

void init_storage()
{
	init_bubble_storage_api();
	init_entity_storage_api();
	init_storage_api();
	entity_storage = linked_list->new ();
	bubble_storage = linked_list->new ();
}

void clean_storage()
{
	// TODO probably need to deallocate the EntityPointers inside of here
	linked_list->free(entity_storage);
	linked_list->free(bubble_storage);
	free(entity);
	free(bubble);
	free(storage);
}

// Bubble storage methods
void bubble_sto_add(BubbleEntity *bubble)
{
	linked_list->add(bubble_storage, bubble);
}

void bubble_sto_remove(BubbleEntity *bubble)
{
	linked_list->remove(bubble_storage, bubble);
}

void init_bubble_storage_api()
{
	_bubble_storage_api *api = MALLOC(1, _bubble_storage_api);
	api->add = &bubble_sto_add;
	api->remove = &bubble_sto_remove;
	bubble = api;
}

void entity_sto_register(Entity entity, enum EntityKind kind)
{
	EntityPointer *entity_ptr_info = MALLOC(1, EntityPointer);
	entity_ptr_info->kind = kind;
	entity_ptr_info->data = entity;
	linked_list->add(entity_storage, entity_ptr_info);
}

void entity_sto_deregister(Entity entity, enum EntityKind kind)
{
	for (int i = 0; i < entity_storage->size; i++)
	{
		EntityPointer *entity_ptr_info = linked_list->get(entity_storage, i);

		if (entity_ptr_info->kind == kind)
		{
			switch (entity_ptr_info->kind)
			{
			case kHookEntity:
			{
				FishHookEntity *hook = entity_ptr_info->data.hook;

				if (hook == entity.hook)
				{
					linked_list->remove(entity_storage, entity_ptr_info);
					free(entity_ptr_info);
				}

				break;
			}
			case kFishEntity:
			{
				FishEntity *fish = entity_ptr_info->data.fish;

				if (fish == entity.fish)
				{
					linked_list->remove(entity_storage, entity_ptr_info);
					free(entity_ptr_info);
				}
				break;
			}
			case kBubbleEntity:
			{
				BubbleEntity *bubble = entity_ptr_info->data.bubble;

				if (bubble == entity.bubble)
				{
					linked_list->remove(entity_storage, entity_ptr_info);
					free(entity_ptr_info);
				}
				break;
			}
			}
		}
	}
}

void *entity_sto_get_entity_by_sprite(enum EntityKind kind, LCDSprite *sprite)
{
	int i = 0;
	EntityPointer *entity;

	while ((entity = linked_list->get(entity_storage, i)) != NULL)
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

void init_entity_storage_api()
{
	_entity_storage_api *api = MALLOC(1, _entity_storage_api);
	api->add = &entity_sto_register;
	api->deregister = &entity_sto_deregister;
	api->get_by_sprite = &entity_sto_get_entity_by_sprite;
	entity = api;
}

void init_storage_api()
{
	_storage_api *api = MALLOC(1, _storage_api);
	api->bubble = bubble;
	api->entity = entity;
	storage = api;
}