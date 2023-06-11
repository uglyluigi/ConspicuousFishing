#include "linkedlist.h"
#include "util.h"
#include "alloc.h"
#include "storage.h"

void init_storage()
{
	init_entity_storage_api();
	init_storage_api();
	entity_storage = linked_list->new ();
}

void clean_storage()
{
	// TODO probably need to deallocate the EntityPointers inside of here
	linked_list->free(entity_storage);
	free(entity);
	free(storage);
}

void entity_sto_register(const Entity *entity)
{
	linked_list->add(entity_storage, entity);
}

void entity_sto_deregister(const Entity *entity)
{
	for (int i = 0; i < entity_storage->size; i++)
	{
		Entity *ent = linked_list->get(entity_storage, i);

		if (entity == ent)
		{
			linked_list->remove(entity_storage, ent);
			break;
		}
	}
}

void *entity_sto_get_entity_by_sprite(const LCDSprite *sprite)
{
	for (int i = 0; entity_storage->size; i++)
	{
		Entity *entity = linked_list->get(entity_storage, i);

		if (entity->sprite == sprite)
		{
			return entity;
		}
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
	api->entity = entity;
	storage = api;
}