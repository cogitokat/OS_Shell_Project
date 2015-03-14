%{
#include <stdio.h>
#include <stdlib.h>

void yyerror(const char *msg);
int yylineno;
int yylex(void);

%}

%error-verbose

%union {
  int num;
  char* s;
};

%token <num> NUMBER;
%token <s> WORD;
%token '\n'
%left '|'

%%

line : /*EMPTY*/			
     | line commands '\n'		{fprintf(stdout, "line commands\n");}
     | line command '\n'                {fprintf(stdout, "line command\n");}
     ;

command : WORD				{fprintf(stdout, "WORD (%s)\n", $1);}
	| WORD params			{fprintf(stdout, "WORD (%s) params\n", $1);}
	;

param : WORD				{fprintf(stdout, "WORD (%s)\n", $1);}
      ;

commands : commands '|' command		{fprintf(stdout, "commands | command\n");}
	 | command			{fprintf(stdout, "command\n");}
	 ;

params : param				{fprintf(stdout, "param\n");}
       | param params			{fprintf(stdout, "param params\n");}
       ;

%%

void yyerror(const char *msg) {
	fprintf(stderr, "line %d: %s\n", yylineno, msg);
	exit(1);
}

int main(void){
  yyparse();
  return 0;
}
