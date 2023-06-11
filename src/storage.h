#ifndef STORAGEC
#define STORAGEC

#include "linkedlist.h"
#include "entity/entity.h"

LinkedList *entity_storage;

enum EntityKind
{
	kFishEntity,
	kBubbleEntity,
	kHookEntity
};

typedef struct _entity_storage_api
{
	void (*add)(Entity*);
	void (*deregister)(Entity*);
	void *(*get_by_sprite)(LCDSprite *);
} _entity_storage_api;

_entity_storage_api *entity;

typedef struct _storage_api
{
	_entity_storage_api *entity;
} _storage_api;

_storage_api *storage;

void init_storage();
void clean_storage();
void init_storage_api();

// entity
void entity_sto_register(const Entity*);
void entity_sto_deregister(const Entity*);
void *entity_sto_get_entity_by_sprite(const LCDSprite *sprite);
void init_entity_storage_api();

#endif
