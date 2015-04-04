%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shellparser.h"

void yyerror(const char *msg);
int yylineno;
int yylex(void);
void displayPrompt(void);

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

//start : line				{fprintf(stdout, "start\n");}

line : line command '\n'                {fprintf(stdout, "line command\nWalking the tree...\n"); 
     					 evalNode($2); freeNode($2); displayPrompt();}
     | line commands '\n'               {fprintf(stdout, "line commands\nWalking the tree...\n"); 
					 evalNode($2); freeNode($2); displayPrompt();}
     | /*EMPTY*/			
     | line '\n'		
     ;

command : WORD                          {fprintf(stdout, "WORD (%s)\n", $1); $$ = new_command($1, NULL);}
        | WORD params                   {fprintf(stdout, "WORD (%s) params\n", $1);$$ = new_command($1, $2);}
        ;

param : WORD                            {fprintf(stdout, "WORD (%s)\n", $1); $$ = new_param($1);}
      ;

commands : command '|' commands        {fprintf(stdout, "command | commands\n"); $$ = new_pipe($1, $3);}
         | command                      {fprintf(stdout, "command\n"); $$ = new_pipe($1, NULL);}
         ;

params : param                          {fprintf(stdout, "param\n"); $$ = new_params($1, NULL);}
       | param params                   {fprintf(stdout, "param params\n"); $$ = new_params($1, $2);}
       ;

%%

void yyerror(const char *msg) {
	fprintf(stderr, "line %d: %s\n", yylineno, msg);
	exit(1);
}

void displayPrompt(void) {
  if(isatty(0)) {
    fprintf(stdout, "$: ");
  }
}

int main(void) {
  displayPrompt();
  yyparse();
  return 0;
}
