#include "malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>

typedef struct Chunk {
	size_t size;
	bool free;
} Chunk;

size_t avail = 4096;
void *first = NULL;

void* next_chunk(void* current) {
	return current + sizeof(Chunk) + ((Chunk*)current)->size;
}

void* first_allocable_address(void* chunk_header) {
	return chunk_header + sizeof(Chunk);
}

size_t chunk_size(void* chunk) {
	return ((Chunk*)chunk)->size;
}

bool is_chunk_free(void* chunk) {
	return ((Chunk*)chunk)->free;
}

void* malloc(size_t size) {
	if (size > avail) {
		return NULL;
	}

	Chunk *chunk = first;

	while (size > chunk_size(chunk)) {
		if(chunk == NULL) {
			chunk = mmap(NULL, size + sizeof(Chunk), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
			((Chunk*)chunk)->size = size;
			((Chunk*)chunk)->free = false;
			avail -= size;
			return first_allocable_address(chunk);
		}
		if(((Chunk*)chunk)->free) {
			((Chunk*)chunk)->free = false;	
			return first_allocable_address(chunk);
		}
		chunk = next_chunk(chunk);
	}

	return NULL;
}

void free(void *ptr) {
	if(ptr == NULL) {
		return;
	}

	if(((Chunk*)(ptr - sizeof(Chunk)))->free == false) {
		munmap(ptr, ((Chunk*)(ptr - sizeof(Chunk)))->size);
		((Chunk*)(ptr - sizeof(Chunk)))->size = true;
	} 
}

void test(void) {
	int *some = malloc(sizeof(int));
	*some = 8;
	printf("%p\n", some);
	printf("%d", *some);
	free(some);
}
