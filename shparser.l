%{
#include <stdio.h>
#include <string.h>
#include "shellparser.h"
#include "y.tab.h"

char * unquote(char *string);
%}

%x DBLQUOTES SNGLQUOTES

%%

\"  { BEGIN(DBLQUOTES);}

<DBLQUOTES>\"           {BEGIN(INITIAL);}
<DBLQUOTES>[^\"]+      {yylval.s = strdup(yytext); return WORD;
                                      BEGIN(INITIAL);}

\'  { BEGIN(SNGLQUOTES);}

<SNGLQUOTES>\'           {BEGIN(INITIAL);}
<SNGLQUOTES>[^\']+      {yylval.s = strdup(yytext); return WORD;
                                      BEGIN(INITIAL);}


([^\t \n\"\?\\\'\&><\|]|\\&|\\\"|\\>|\\<|\\\'|\\\|)+ {yylval.s = strdup(yytext); return WORD;}
(&)                                      {return *yytext;}
(2>)					                           {return RE_STDERR;}
(&1)                                     {return STDOUT;}
[|><\n]                 		             {return *yytext;}
[ \t\r]+                		             {;}
<<EOF>>                           {return END_OF_FILE;}

.                       		             {yyerror("Invalid character!"); return ERRTOK;}

%%

int yywrap(void)
{
  return 1;
}
