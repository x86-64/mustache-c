%{
#include <stdint.h>
#include "config.h"
#include "mustache.h"
#include "mustache-internal.h"
#include "parser.tab.h"
%}

%option noyywrap
%option case-insensitive
%option noinput
%option nounput
%option yylineno

%x comment
%x mustag

name        [a-z0-9.][a-z0-9_:.-]*
special     [#^/]

%%

<comment>[^*]*
<comment>"*"+[^*/]*
<comment>"*"+"/"        BEGIN(INITIAL);

<mustag>{special}     { return *yytext; }
<mustag>{name}        { yylval->text = mustache_strdup(yytext); return TEXT; }
<mustag>"}}"          { BEGIN(INITIAL); return MUSTAG_END;  }
<mustag>"}}}"         { BEGIN(INITIAL); return MUSTAG_NOESC_END;  }

"//"+[^\n]*           {                 }
"/*"                  { BEGIN(comment); }
"{{"                  { BEGIN(mustag); return MUSTAG_START; }
"{{{"                 { BEGIN(mustag); return MUSTAG_NOESC_START; }

"{" |
[^{]*                 { yylval->text = mustache_strdup(yytext); return TEXT;  }

%%
