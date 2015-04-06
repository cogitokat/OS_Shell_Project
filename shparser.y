%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "builtins.h"
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
%token ERRTOK;
%token '\n' '>' '<'
%left '|'
%type <np> command commands params param

%%

start : line        			{fprintf(stdout, "start\n");}

line : line command '\n'                {fprintf(stdout, "line command\n"); 
               				 RootNode = $2; YYACCEPT;}
     | line commands '\n'               {fprintf(stdout, "line commands\n"); 
           				 RootNode = $2; YYACCEPT;}
     | /*EMPTY*/
     | line '\n'
     ;

command : WORD                          {fprintf(stdout, "WORD (%s)\n", $1); $$ = new_command($1, NULL);}
	| ERRTOK			{fprintf(stdout, "cmd ERRTOK!\n"); YYABORT;}
        | WORD params                   {fprintf(stdout, "WORD (%s) params\n", $1);$$ = new_command($1, $2);}
	| ERRTOK params			{fprintf(stdout, "ERRTOK! params\n"); YYABORT;}
        ;

param : WORD                            {fprintf(stdout, "WORD (%s)\n", $1); $$ = new_param($1);}
      | ERRTOK				{fprintf(stdout, "ERRTOK!\n"); YYABORT;}
      ;

commands : command '|' commands         {fprintf(stdout, "command | commands\n"); $$ = new_pipe($1, $3);}
         | command                      {fprintf(stdout, "command\n"); $$ = new_pipe($1, NULL);}
         ;

params : param                          {fprintf(stdout, "param\n"); $$ = new_params($1, NULL);}
       | param params                   {fprintf(stdout, "param params\n"); $$ = new_params($1, $2);}
       ;

%%

void yyerror(const char *msg) {
  fprintf(stderr, "line %d: %s\n", yylineno, msg);
}
