%{
#include <stdio.h>
#include <stdlib.h>
#include "shellparser.h"

void yyerror(const char *msg);
int yylineno;
int yylex(void);

%}

%error-verbose

%union {
  int num;
  char* s;
  Node *np;
};

%token <num> NUMBER;
%token <s> WORD;
%token '\n'
%left '|'
%type <np> command commands params param

%%

line : line command '\n'                {fprintf(stdout, "line command\nWalking the tree...\n"); 
     					 printNode($2); freeNode($2);}
     | line commands '\n'               {fprintf(stdout, "line commands\nWalking the tree...\n"); 
					 printNode($2); freeNode($2);}
     | /*EMPTY*/
     ;

command : WORD                          {fprintf(stdout, "WORD (%s)\n", $1); $$ = new_command($1, NULL);}
        | WORD params                   {fprintf(stdout, "WORD (%s) params\n", $1);$$ = new_command($1, $2);}
        ;

param : WORD                            {fprintf(stdout, "WORD (%s)\n", $1); $$ = new_param($1);}
      ;

commands : commands '|' command         {fprintf(stdout, "commands | command\n"); $$ = new_pipe($3, $1);}
         | command                      {fprintf(stdout, "command\n"); $$ = new_pipe($1, NULL);}
         ;

params : param                          {fprintf(stdout, "param\n"); $$ = new_params($1, NULL);}
       | param params                   {fprintf(stdout, "param params\n"); $$ = new_params($2, $1);}
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
