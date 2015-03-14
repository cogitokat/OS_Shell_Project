%{
#include "y.tab.h"
%}

%%

0                       {yylval.num = atoi(yytext); return NUMBER;}
[1-9][0-9]*             {yylval.num = atoi(yytext); return NUMBER;}
([A-Za-z]|[0-9]|-)([A-Za-z]|[0-9]|_|\.)* {yylval.s = strdup(yytext); return WORD;}
[|\n]                   {return *yytext;}
[ \t\r]+                {;}

.                       {yyerror("invalid character");}

%%

int yywrap(void)
{
  return 1;
}

