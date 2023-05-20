#ifndef LINKEDLISTC
#define LINKEDLISTC

#include <stdlib.h>
#include <stdbool.h>

typedef struct LinkedListNode
{
	void *data;
	struct LinkedListNode *next;
} LinkedListNode;

typedef struct
{
	LinkedListNode *head;
	size_t size;
} LinkedList;

typedef struct _linkedlist_api
{
	LinkedList *(*new)(void);
	void (*add)(LinkedList *, void *);
	int (*index_of)(LinkedList *, void *);
	void *(*get)(LinkedList *, size_t);
	bool (*remove)(LinkedList *, void *);
	void (*free)(LinkedList *);
	void (*cleanup)(void);
	void (*for_each)(const LinkedList *, void (*)(void *))
} _linkedlist_api;

_linkedlist_api *linked_list;

LinkedList *linkedlist_new(void);
LinkedListNode *linkedlist_new_node(void *data);
LinkedListNode *traverse(const LinkedList *list);
void linkedlist_add(LinkedList *list, void *data);
int linkedlist_index_of(LinkedList *list, void *data);
void *linkedlist_get(LinkedList *list, size_t index);
bool linkedlist_remove(LinkedList *list, void *data);
void linkedlist_free_rec(LinkedListNode *current);
void linkedlist_free(LinkedList *list);
void cleanup_linkedlist_api();
void init_linkedlist_api();
void for_each(LinkedList *list, void (*consumer)(void *item));

#endif