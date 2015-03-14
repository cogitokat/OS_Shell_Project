#include <stdio.h>
#include "scanner.h"

extern yylex();
extern yylineno;
extern char* yytext;

int main(void){
  printf("Your tokens...\n");
  int ntoken, vtoken;
  ntoken = yylex();
  while(ntoken){
    switch(ntoken){
      case 1:
        printf("NUMBER\n");
        break;
      case 2:
        printf("WORD\n");
        break;
      case 3:
        printf("PIPE\n");
        break;
    }
    ntoken = yylex();
  }
  return 0;
}

