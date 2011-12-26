%{
#include <config.h>
#include <mustache.h>
#include <parser.tab.h>	
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

void yyerror (template_t **, const char *);

typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE mustache__scan_string (const char *string);  
extern int mustache_lex_destroy(void);
extern int mustache_lex(YYSTYPE *);
extern int mustache_get_lineno(void);
extern char *mustache_get_text(void);
char *mustache_ext_file;

#define emit_error(fmt, ...){                                           \
	do {                                                            \
		char _buffer[DEF_BUFFER_SIZE];                          \
		                                                        \
		snprintf(_buffer, sizeof(_buffer), fmt, ##__VA_ARGS__); \
		yyerror(template, _buffer);                                 \
		YYERROR;                                                \
	}while(0); }

%}

%start  start

%define api.pure
%parse-param {template_t **template}

%union {
	char                  *text;
	template_t            *template;
}
%token TEXT MUSTAG_START MUSTAG_END
%type  <text>                  TEXT MUSTAG_START MUSTAG_END text
%type  <template>              tpl_tokens
%type  <template>              tpl_token

%%

start : tpl_tokens { *template = $1; }

tpl_tokens :
	/* empty */ {
		$$ = NULL;
	}
	| tpl_token {
		$$ = $1;
	}
	| tpl_tokens tpl_token {
		template_t *p = $1;
		
		while(p->next != NULL)
			p = p->next;
		
		p->next = $2;
		$$ = $1;
	}
	;

tpl_token :
	text {                                   // simple text
		$$ = malloc(sizeof(template_t));
		$$->type = TOKEN_TEXT;
		$$->text = $1;
		$$->next = NULL;
	}
	| MUSTAG_START text MUSTAG_END {         // mustache tag
		$$ = malloc(sizeof(template_t));
		$$->type = TOKEN_VARIABLE;
		$$->text = $2;
		$$->next = NULL;
	}
	| MUSTAG_START '#' text MUSTAG_END {     // mustache section start
		$$ = malloc(sizeof(template_t));
		$$->type = TOKEN_SECTION_START;
		$$->text = $3;
		$$->next = NULL;
	}
	;

text :
	TEXT {
		$$ = $1;
	}
	| text TEXT {    // eat up text duplicates
		uintmax_t len1, len2;
		
		len1 = strlen($1);
		len2 = strlen($2);
		$1   = realloc($1, len1 + len2 + 1);
		memcpy($1 + len1, $2, len2 + 1);
		
		$$  = $1;
		free($2);
	}

%%

void yyerror(template_t **template, const char *msg){ // {{{
	char                  *file              = mustache_ext_file; 
	
	if(!file)
		file = "-";
	
	fprintf(stderr, "%s: error: %d: %s near '%s'\n", file, mustache_get_lineno(), msg, mustache_get_text());
	(void)template;
} // }}}

template_t *   mustache_string_parse(char *string){ // {{{
	template_t *new_template = NULL;
	
	mustache__scan_string(string);
	
	yyparse(&new_template);
	
	mustache_lex_destroy();
	return new_template;
} // }}}

#ifdef FILE_PARSING
template_t *   mustache_file_parse(char *filename){ // {{{
	template_t            *new_template      = NULL;
	FILE                  *fd;
	char                  *content           = NULL;
	uintmax_t              content_off       = 0;
	uintmax_t              content_size      = 0;

	if( (fd = fopen(filename, "rb")) == NULL)
		return NULL;
		
	while(!feof(fd)){
		content_size += 1024;
		content       = realloc(content, content_size + 1); // 1 for terminating \0
		if(!content)
			break;
		
		content_off  += fread(content + content_off, 1, content_size - content_off, fd);
	}
	fclose(fd);
	
	if(content){
		mustache_ext_file = strdup(filename);
		
		content[content_off] = '\0';
		
		new_template = mustache_string_parse(content);
		free(content);
		
		if(mustache_ext_file){
			free(mustache_ext_file);
			mustache_ext_file = NULL;
		}
	}
	return new_template;
} // }}}
#endif

#ifdef DEBUG

char * token_descr[] = {
	[TOKEN_TEXT]     = "text",
	[TOKEN_VARIABLE] = "variable",
	[TOKEN_SECTION_START] = "section",
};

void mustache_dump(template_t *template){ // {{{
	template_t            *p;
	
	p = template;
	do{
		fprintf(stderr, "token: ->type '%s'; ->text: '%s'; ->next = %p\n",
			token_descr[p->type],
			p->text,
			p->next
		);
	}while( (p = p->next) != NULL);
} // }}}
#endif
