#ifndef STORAGEC
#define STORAGEC

#include "linkedlist.h"
#include "bubble.h"
#include "fish.h"
#include "fish_hook.h"

LinkedList *entity_storage;
LinkedList *bubble_storage;

enum EntityKind
{
	kFishEntity,
	kBubbleEntity,
	kHookEntity
};

typedef union
{
	FishEntity *fish;
	BubbleEntity *bubble;
	FishHookEntity *hook;
} Entity;

typedef struct
{
	Entity data;
	enum EntityKind kind;
} EntityPointer;

typedef struct _bubble_storage_api
{
	void (*add)(BubbleEntity *);
	void (*remove)(BubbleEntity *);
} _bubble_storage_api;

_bubble_storage_api *bubble;

typedef struct _entity_storage_api
{
	void (*add)(Entity, enum EntityKind);
	void (*deregister)(Entity, enum EntityKind);
	void *(*get_by_sprite)(enum EntityKind, LCDSprite *);
} _entity_storage_api;

_entity_storage_api *entity;

typedef struct _storage_api
{
	_entity_storage_api *entity;
	_bubble_storage_api *bubble;
} _storage_api;

_storage_api *storage;

#undef entity;
#undef bubble;

void init_storage();
void clean_storage();
void init_storage_api();

// new api
// bubble
void bubble_sto_add(BubbleEntity *bubble);
void bubble_sto_remove(BubbleEntity *bubble);
void init_bubble_storage_api();

// entity
void entity_sto_register(Entity entity, enum EntityKind kind);
void entity_sto_deregister(Entity entity, enum EntityKind kind);
void *entity_sto_get_entity_by_sprite(enum EntityKind kind, LCDSprite *sprite);
void init_entity_storage_api();

#endif
