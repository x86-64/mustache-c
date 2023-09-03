#ifndef MUSTACHEC_INTERNAL_H
#define MUSTACHEC_INTERNAL_H

typedef struct mustache_ctx {
	mustache_api_t        *api;
	mustache_template_t   *template;
	void                  *userdata;
} mustache_ctx;

void *internal_mustache_malloc(size_t size);

void *internal_mustache_realloc(void *dst, size_t size);

void *internal_mustache_calloc(size_t num, size_t size);

void *internal_mustache_free(void *dst);

char *mustache_strdup(const char *src);

#endif
