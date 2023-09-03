#include <stdlib.h>
#include <string.h>
#include "mustache.h"
#include "mustache-internal.h"

static mustache_memory_malloc_f mustache_memory_malloc = malloc;
static mustache_memory_realloc_f mustache_memory_realloc = realloc;
static mustache_memory_calloc_f mustache_memory_calloc = calloc;
static mustache_memory_free_f mustache_memory_free = free;

// explicit strdup that doesn't hide malloc
char *mustache_strdup(const char *src) {
	size_t len = strlen(src) + 1;
	char *s = internal_mustache_malloc(len);
	if (s == NULL)
		return NULL;
	return (char *)memcpy(s, src, len);
}

void *
internal_mustache_malloc(size_t size)
{
    return mustache_memory_malloc(size);
}

void *
internal_mustache_realloc(void *dst, size_t size)
{
    return mustache_memory_realloc(dst, size);
}

void *
internal_mustache_calloc(size_t num, size_t size)
{
    return mustache_memory_calloc(num, size);
}

void *
internal_mustache_free(void *dst)
{
    mustache_memory_free(dst);
    return NULL;
}

void
mustache_memory_setup(mustache_memory_malloc_f new_malloc, mustache_memory_realloc_f new_realloc,
                    mustache_memory_calloc_f new_calloc, mustache_memory_free_f new_free)
{
    mustache_memory_malloc = new_malloc;
    mustache_memory_realloc = new_realloc;
    mustache_memory_calloc = new_calloc;
    mustache_memory_free = new_free;
}
