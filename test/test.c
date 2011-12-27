#include <stdint.h>
#include <config.h>
#include <mustache.h>
#include <stdio.h>

int error_flag;

void tests_error(mustache_api_t *api, void *userdata, uintmax_t lineno, char *error){
	fprintf(stderr, "error: %d: %s\n", (int)lineno, error);
	error_flag = 1;
}

int tests_test1(void){
	mustache_template_t   *template;
	char                   source_str[] = "Hello, world! I'm { { mustache } } library! Here is {{myvar}} test. Here is {{#section}} test. {{/section}}";
	
	mustache_api_t         api = {
		.read         = &mustache_std_strread,
		.error        = &tests_error,
	};
	mustache_strread_ctx   mystr = { source_str, 0 };
	
	template = mustache_compile(&api, &mystr);
	
	mustache_dump(template);
	mustache_free(template);

	return error_flag;
}

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

