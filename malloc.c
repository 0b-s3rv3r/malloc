#include "malloc.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

typedef struct Block {
	size_t size;
	bool free;
} Block;

#define BLOCK_SIZE sizeof(Block)
#define SIZE(b) ((Block*)b)->size
#define FREE(b) ((Block*)b)->free
#define NEXT(b) (char*)b + SIZE(b) + BLOCK_SIZE 
#define HEADER(b) ((Block*)b - 1)
#define ALLOC_ADDR(b) ((char*)b + BLOCK_SIZE)

void* first = NULL;

// TODO: Fix memory fragmentation
void* malloc(size_t size) {
	if(first == NULL) {
		first = sbrk(BLOCK_SIZE);
		if (first == (void*)-1) return NULL;
		SIZE(first) = 0;
		FREE(first) = true;
	}
	if(size == 0) return NULL;
	void* block = first;
	while (SIZE(block) != 0) {
		if(SIZE(block) >= size && FREE(block)) {
			FREE(block) = false;
			return ALLOC_ADDR(block);
		}
		block = NEXT(block);
	}
	SIZE(block) = size;
	FREE(block) = false;
	void* allocable_addr = ALLOC_ADDR(block);
	allocable_addr = sbrk(size + BLOCK_SIZE);
	if (block == (void*)-1) return NULL;
	void* next = NEXT(block);
	SIZE(next) = 0;
	FREE(next) = true;
	return ALLOC_ADDR(block);
}

void free(void* ptr) {
	FREE(HEADER(ptr)) = true;
	ptr = NULL;
}


void test(void) {
	int *n = malloc(sizeof(long long));
	*n = 8;
	free(n);

	int *n2 = malloc(sizeof(int));
	*n2 = 10;

	int *n3 = malloc(sizeof(int));

	char* str = malloc(sizeof(char) * 10);
	strcpy(str, "something");
	free(str);

	char* str2 = malloc(sizeof(char) * 5);
	strcpy(str2, "some");

	// std::printf after all allocations to not damage already existing memory
	printf("n2: %d\n", *n2);
	printf("n2 addr: %p\n", n2);
	printf("n2 block size: %d\n", (int)SIZE(HEADER(n2)));
	printf("n3 addr: %p\n", n3);

	printf("str2: %s\n", str2);
	printf("str2 addr: %p\n", str2);
	printf("str2 block size: %d\n", (int)SIZE(HEADER(str2)));

	assert((int)SIZE(HEADER(n2)) == 8);
	assert(n2 != n3);
	assert((int)SIZE(HEADER(str2)) == 10);

	free(n2);
	free(n3);
	free(str2);
}
