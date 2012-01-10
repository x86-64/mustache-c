GENSOURCE = parser.tab.h parser.tab.c parser.lex.c

lib_LTLIBRARIES=libmustache_c.la
libmustache_c_la_SOURCES= $(GENSOURCE)
libmustache_c_la_LDFLAGS= -module -avoid-version -shared
EXTRA_DIST = parser.l parser.y                 
CLEANFILES = $(GENSOURCE)

include_HEADERS=mustache.h

parser.tab.c: parser.lex.c
	${BISON} -p mustache_p_ -d -b parser parser.y
parser.lex.c:
	${FLEX}  -Pmustache_p_ --bison-bridge -o parser.lex.c parser.l

if HAVE_FROZEN
OPT_DIRS=frozen
endif
SUBDIRS = . $(OPT_DIRS)
