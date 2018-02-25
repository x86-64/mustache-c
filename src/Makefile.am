GENSOURCE = parser.tab.h parser.tab.c parser.lex.c

lib_LTLIBRARIES=libmustache_c.la
libmustache_c_la_SOURCES= $(GENSOURCE) mustache-internal.h
libmustache_c_la_LDFLAGS= -module -avoid-version -shared
EXTRA_DIST = parser.l parser.y                 
CLEANFILES = $(GENSOURCE)

include_HEADERS=mustache.h

parser.tab.c: parser.lex.c parser.y
	${BISON} -p mustache_p_ -d -b parser parser.y
parser.lex.c: parser.l
	${FLEX}  -Pmustache_p_ --bison-bridge -o parser.lex.c parser.l

