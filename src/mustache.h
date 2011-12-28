#ifndef MUSTACHE_H
#define MUSTACHE_H

typedef struct mustache_api_t             mustache_api_t;
typedef struct mustache_token_t           mustache_token_t;
typedef struct mustache_token_t           mustache_template_t;
typedef enum   mustache_token_type_t      mustache_token_type_t;
typedef struct mustache_token_variable_t  mustache_token_variable_t;
typedef struct mustache_token_section_t   mustache_token_section_t;

typedef uintmax_t (*mustache_api_read)   (mustache_api_t *api, void *userdata, char *buffer, uintmax_t buffer_size);
typedef uintmax_t (*mustache_api_write)  (mustache_api_t *api, void *userdata, char *buffer, uintmax_t buffer_size);
typedef uintmax_t (*mustache_api_varget) (mustache_api_t *api, void *userdata, mustache_token_variable_t *token); 
typedef uintmax_t (*mustache_api_sectget)(mustache_api_t *api, void *userdata, mustache_token_section_t  *token); 
typedef void      (*mustache_api_error)  (mustache_api_t *api, void *userdata, uintmax_t lineno, char *error); 

enum mustache_token_type_t {
	TOKEN_TEXT,
	TOKEN_VARIABLE,
	TOKEN_SECTION
};

struct mustache_token_variable_t {
	char                  *text;
	uintmax_t              text_length;
};

struct mustache_token_section_t {
	char                  *name;
	mustache_token_t      *section;
	uintmax_t              inverted;
};

struct mustache_token_t {
	mustache_token_type_t  type;
	
	union {
		mustache_token_variable_t     token_simple;
		mustache_token_section_t      token_section;
	};
	
	mustache_token_t      *next;
};

struct mustache_api_t {
	mustache_api_read     read;
	mustache_api_write    write;
	mustache_api_varget   varget;
	mustache_api_sectget  sectget;
	mustache_api_error    error;
};

// api
mustache_template_t * mustache_compile(mustache_api_t *api, void *userdata);
uintmax_t             mustache_render (mustache_api_t *api, void *userdata, mustache_template_t *template);
void                  mustache_free   (mustache_template_t *template);

// helpers (build with --enable-helpers, default)
typedef struct mustache_str_ctx {
	char                  *string;
	uintmax_t              offset;
} mustache_str_ctx;

uintmax_t             mustache_std_strread  (mustache_api_t *api, void *userdata, char *buffer, uintmax_t buffer_size);
uintmax_t             mustache_std_strwrite (mustache_api_t *api, void *userdata, char *buffer, uintmax_t buffer_size);

// debug api (build with --enable-debug, not default)
void                  mustache_dump   (mustache_template_t *template);

#endif
