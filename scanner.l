%{
#include "scanner.h"
%}

%%

[1-9][0-9]*                       	 return NUMBER;

([A-Za-z]|[0-9]|-)([A-Za-z]|[0-9]|_|\.)* return WORD;
[|] 					 return PIPE;
[ \t\n]+                		 ;
.                       		 printf("unexpected character\n");

%%

int yywrap(void)
{
  return 1;
}

