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

typedef struct
{
	LinkedList *(*new)(void);
	void (*add)(LinkedList *, void *);
	int (*index_of)(LinkedList *, void *);
	void *(*get)(LinkedList *, size_t);
	bool (*remove)(LinkedList *, void *);
	void (*free)(LinkedList *);
	void (*cleanup)(void);
} _linkedlist_api;

_linkedlist_api *linked_list;

LinkedList *linkedlist_new(void)
{
	LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
	list->head = NULL;
	list->size = 0;
	return list;
}

LinkedListNode *linkedlist_new_node(void *data)
{
	LinkedListNode *node = (LinkedListNode *)malloc(sizeof(LinkedListNode));
	node->data = data;
	node->next = NULL;
	return node;
}

LinkedListNode *traverse(const LinkedList *list)
{
	LinkedListNode *current_node = list->head;

	if (current_node == NULL)
	{
		return NULL;
	}

	while (current_node->next != NULL)
	{
		current_node = current_node->next;
	}

	return current_node;
}

void linkedlist_add(LinkedList *list, void *data)
{
	LinkedListNode *node = linkedlist_new_node(data);

	if (list->head == NULL)
	{
		list->head = node;
	}
	else
	{
		LinkedListNode *tail = traverse(list);
		tail->next = node;
	}

	list->size += 1;
}

int linkedlist_index_of(LinkedList *list, void *data)
{
	int current_index = 0;
	LinkedListNode *current_node = list->head;

	while (current_node != NULL)
	{
		if (current_node->data == data)
		{
			return current_index;
		}

		current_index++;
		current_node = current_node->next;
	}

	return -1;
}

void *linkedlist_get(LinkedList *list, size_t index)
{

	if (index > list->size - 1)
	{
		// what do?
	}

	int current_index = 0;
	LinkedListNode *current_node = list->head;

	while (current_node != NULL)
	{
		if (current_index == index)
		{
			return current_node->data;
		}

		current_index++;
		current_node = current_node->next;
	}

	return NULL;
}

bool linkedlist_remove(LinkedList *list, void *data)
{
	// Check if it's an empty list
	if (list->head == NULL)
	{
		// Empty lists cant have stuff removed from them
		return false;
	}

	if (data == NULL)
	{
		printf("You probably didn't mean to remove NULL from a linked list.");
		return false;
	}

	// Traverse the list
	LinkedListNode *prev_node;
	LinkedListNode *curr_node = list->head;

	while (curr_node != NULL)
	{
		if (curr_node->data == data)
		{
			LinkedListNode *old_ptr = curr_node;

			if (curr_node == list->head)
			{
				list->head = list->head->next;
			}
			else
			{
				prev_node->next = curr_node->next; // Sometimes this will be NULL, but that's OK
			}

			free(old_ptr);
			list->size -= 1;
			return true;
		}

		if (curr_node->next != NULL)
		{
			prev_node = curr_node;
			curr_node = curr_node->next;
		}
		else
		{
			return false;
		}
	}

	return false;
}

void linkedlist_free_rec(LinkedListNode *current)
{
	if (current->next != NULL)
	{
		linkedlist_free_rec(current->next);
	}

	free(current);
}

void linkedlist_free(LinkedList *list)
{
	linkedlist_free_rec(list->head);
}

void cleanup_linkedlist_api()
{
	free(linked_list);
}

void init_linkedlist_api()
{
	_linkedlist_api *api = (_linkedlist_api *)malloc(sizeof(_linkedlist_api));
	api->add = &linkedlist_add;
	api->get = &linkedlist_get;
	api->index_of = &linkedlist_index_of;
	api->new = &linkedlist_new;
	api->remove = &linkedlist_remove;
	api->free = &linkedlist_free;
	api->cleanup = &cleanup_linkedlist_api;
	linked_list = api;
}

#endif