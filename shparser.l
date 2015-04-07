%{
#include <stdio.h>
#include <string.h>
#include "shellparser.h"
#include "y.tab.h"

char * unquote(char *string);
%}

%%

([^\t \n\"\?\\\'\&><\|]|\\&|\\\"|\\>|\\<|\\\'|\\\|)+ {yylval.s = strdup(yytext); return WORD;}
(\"(\\.|[^\\"])*\")|(\'(\\.|[^\\'])*\')  {char unquoted[yyleng+1];
                                          strncpy(unquoted, yytext, yyleng+1); 
                                          unquote(unquoted);
                                          yylval.s = strdup(unquoted);
                                          return WORD;}
(&)                                      {return *yytext;}
(2>)					                           {return RE_STDERR;}
(&1)                                     {return STDOUT;}
[|><\n]                 		             {return *yytext;}
[ \t\r]+                		             {;}

.                       		             {yyerror("Invalid character!");}

%%

int yywrap(void)
{
  return 1;
}

char * unquote(char *string) {
  /* We can strip quotes in the lexer,
   * making parsing easier because then
   * it can be parsed as a normal word.
   */
  char *strp = string;
  int stringlen = strlen(strp);
  int r = 0; // for reading the string
  int w = 0; // for writing back to the string
  if (strp[0] == '"') { // if the string is double quoted
    for (r=0; r<stringlen; r++) { // loop over the string
      if (strp[r] != '"' && strp[r] != '\\') { // regular char
        strp[w++] = strp[r]; // keep the char
      } else if (strp[r+1] == '"' && strp[r] == '\\') { // escaped '"'
	strp[w++] = '"';
      } else if (strp[r+1] != '"' && strp[r] == '\\') { // regular '\'
	strp[w++] = '\\';
      }
    }
  } else if (strp[0] == '\'') { // if the string is single quoted
  for (r=0; r<stringlen; r++) { // same as for double quote
      if (strp[r] != '\'' && strp[r] != '\\') {
        strp[w++] = strp[r];
      } else if (strp[r+1] == '\'' && strp[r] == '\\') {
        strp[w++] = '\'';
      } else if (strp[r+1] != '\'' && strp[r] == '\\') {
        strp[w++] = '\\';
      }   
    }
  }   
  strp[w] = '\0';
  return (string);
}
