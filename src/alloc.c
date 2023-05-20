#include <stdlib.h>
#include <stdio.h>

void *alloc(size_t size)
{
	printf("Allocated %d bytes", size);
	void *new_mem;

	new_mem = malloc(size);

	if (new_mem == NULL)
	{
		printf("Out of memory");
		exit(1);
	}

	return new_mem;
}
