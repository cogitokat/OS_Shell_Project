%{
#include <stdio.h>
#include <string.h>
#include "shellparser.h"
#include "y.tab.h"

#define MAX_STR_SIZE 200

char * unquote(char *string);
%}

%x DBLQUOTES SNGLQUOTES

%%
                                      char string_buf[MAX_STR_SIZE];
                                      char *string_buf_ptr;

\"                                      {string_buf_ptr = string_buf; BEGIN(DBLQUOTES);}
<DBLQUOTES>[^\\"\n]*      { char *yptr = yytext;
                                          while ( *yptr )
                                            *string_buf_ptr++ = *yptr++; }
<DBLQUOTES>\\n             { *string_buf_ptr++ = '\n'; }
<DBLQUOTES>\\t              { *string_buf_ptr++ = '\t'; }
<DBLQUOTES>\\[\\"]          { *string_buf_ptr++ = yytext[1];}
<DBLQUOTES>\"               { yylval.s = strdup(string_buf); BEGIN(INITIAL); 
                                           *string_buf_ptr = '\0'; return WORD;}

([^\t \n\"\?\\\'\&><\|]|\\&|\\\"|\\>|\\<|\\\'|\\\|)+ {yylval.s = strdup(yytext); return WORD;}
\&                                       {return *yytext;}
2>					                            {return RE_STDERR;}
\&1                                     {return STDOUT;}
[|><\n]                 		            {return *yytext;}
[ \t\r]+                		              {;}
<<EOF>>                           {return END_OF_FILE;}

.                       		             {yyerror("Invalid character!"); return ERRTOK;}

%%

int yywrap(void)
{
  return 1;
}
