#include <stdint.h>
#include <config.h>
#include <mustache.h>
#include <stdio.h>

int error_flag;

uintmax_t  tests_varget(mustache_api_t *api, void *userdata, mustache_token_variable_t *token){
	// hardcoded assignments: myvar = "VARIABLE"; mysecvar = "VARIABLE2"
	if(strncmp("myvar", token->text, token->text_length) == 0){
		return api->write(api, userdata, "VARIABLE", 8);
	}
	if(strncmp("mysecvar", token->text, token->text_length) == 0){
		return api->write(api, userdata, "VARIABLE2", 9);
	}
	return 0; // error
}
uintmax_t  tests_sectget(mustache_api_t *api, void *userdata, mustache_token_section_t *token){
	if(strcmp("section", token->name) == 0){
		// call render on inner section as many times as needed
		// 'api' and 'userdata' can be redefined
		return mustache_render(api, userdata, token->section);
	}
	return 0; // error
}
void       tests_error(mustache_api_t *api, void *userdata, uintmax_t lineno, char *error){
	fprintf(stderr, "error: %d: %s\n", (int)lineno, error);
	error_flag = 1;
}

int tests_test1(void){
	mustache_template_t   *template;
	char                   mytemplate[]      = "Hello, world! I'm mustache-c library! Here is {{myvar}} test. Here is {{#section}}SECTION {{mysecvar}}{{/section}} test.";
	char                   target_output[]   = "Hello, world! I'm mustache-c library! Here is VARIABLE test. Here is SECTION VARIABLE2 test.";
	
	mustache_api_t         api = {
		.read         = &mustache_std_strread,
		.write        = &mustache_std_strwrite,
		.varget       = &tests_varget,
		.sectget      = &tests_sectget,
		.error        = &tests_error,
	};
	mustache_str_ctx  srcstr = { mytemplate, 0 };
	mustache_str_ctx  dststr = { NULL, 0 };
	
	template = mustache_compile(&api, &srcstr);
	mustache_render(&api, &dststr, template);
	mustache_free(template);
	
	if(strcmp(dststr.string, target_output) != 0)
		return 1; // error
	
	return error_flag;
}

// Test related functions {{{

typedef int (*test_func)(void);
typedef struct test_desc {
	char                  *name;
	test_func              func;
} test_desc;

test_desc     test_list[] = {
	{ "test1", &tests_test1 },
	{ NULL,    NULL         }
};

int main(int argc, char **argv){
	int                    ret;
	test_desc             *p;
	
	p = test_list;
	while( p->func != NULL ){
		error_flag = 0;
		
		if( (ret = p->func()) != 0){
			fprintf(stderr, "Test '%s' failed: (%d)\n", p->name, ret);
			return 255;
		}
		p++;
	}
	return 0;
}
// }}}
