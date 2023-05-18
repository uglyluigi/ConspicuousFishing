#ifndef STORAGEC
#define STORAGEC
#include "linkedlist.c"

LinkedList *sprite_storage;

void init_storage()
{
	sprite_storage = linked_list->new ();
}

void clean_storage()
{
	linked_list->free(sprite_storage);
}
#endif