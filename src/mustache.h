#ifndef MUSTASHE_H
#define MUSTASHE_H

typedef enum   token_type_t  token_type_t;
typedef struct template_t    template_t;

enum token_type_t {
	TOKEN_TEXT,
	TOKEN_VARIABLE,
	TOKEN_SECTION_START,

};

struct template_t {
	token_type_t           type;
	
	char                  *text;
	
	template_t            *next;
};
template_t *   mustache_string_parse(char *string);
template_t *   mustache_file_parse(char *filename);
void           mustache_dump(template_t *template);

#endif
