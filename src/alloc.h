#ifndef ALLOC_C
#define ALLOC_C

#include <stdlib.h>

#define MALLOC(num, type) (type *)alloc((num) * sizeof(type))
#define malloc CALL MACRO INSTEAD

extern void *alloc(size_t size);

#endif