#include <types.h>
#include "heap.h"
#include "memory.h"

void* malloc(u32 len)
{
	return kmalloc(len);
};
void free(void* p)
{
	kfree(p);
	return;
};


