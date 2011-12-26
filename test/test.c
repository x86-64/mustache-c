#include <config.h>
#include <mustache.h>
#include <stdio.h>

int tests_test1(void){
	template_t            *template;
	char                   source_str[] = "Hello, world! I'm { { mustache } } library! Here is {{myvar}} test. Here is {{#section}} test.";
	
	template = mustache_string_parse(source_str);
	mustache_dump(template);
	return 0;
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
	
	p = &test_list;
	while( p->func != NULL ){
		if( (ret = p->func()) != 0){
			fprintf(stderr, "Test '%s' failed: (%d)\n", p->name, ret);
			return 255;
		}
		p++;
	}
	return 0;
}

