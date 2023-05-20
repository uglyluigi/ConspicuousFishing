#ifndef STORAGEC
#define STORAGEC

#include "linkedlist.h"
#include "bubble.h"
#include "fish.h"
#include "fish_hook.h"

LinkedList *sprite_storage;
LinkedList *entity_storage;

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

void register_entity(enum EntityKind kind, Entity entity);
EntityPointer *get_entity_by_sprite(enum EntityKind kind, LCDSprite *sprite);
void deregister_entity(void *entity);
void init_storage();
void clean_storage();
void store_sprite(LCDSprite *sprite);
LCDSprite *retrieve_sprite(LCDSprite *sprite);

#endif
