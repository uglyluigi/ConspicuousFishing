#ifndef LINKEDLISTC
#define LINKEDLISTC

#include <stdlib.h>
#include <stdbool.h>

#define INDEX_MIN_SIZE 10
#define INDEX_GROW_FACTOR 2

typedef struct LinkedListNode
{
	void *data;
	struct LinkedListNode *next;
} LinkedListNode;

typedef struct
{
	void **index;
	size_t index_size;
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
	void (*for_each)(const LinkedList *, void (*)(void *));
	void *(*get_at)(const LinkedList *, size_t);
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
void linkedlist_for_each(LinkedList *list, void (*consumer)(void *item));
void *linkedlist_get_at(const LinkedList *list, size_t index);
void linkedlist_index_grow(LinkedList *, size_t);
void linkedlist_build_index(const LinkedList *list);

#endif