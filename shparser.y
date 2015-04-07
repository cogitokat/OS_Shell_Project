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
%token ERRTOK STDOUT RE_STDERR;
%token '\n' '>' '<' '&'
%left '|'
%type <np> command commands redir params param commandline

%%

start : line                            {fprintf(stdout, "start\n");}

line : line commands '\n'               {fprintf(stdout, "line commands\n"); 
                                         RootNode = $2; YYACCEPT;}
     | line redir '\n'                  {fprintf(stdout, "line redir\n");
                                         RootNode = $2; YYACCEPT;}
     | line commands '&' '\n'            {fprintf(stdout, "line command\n"); 
                                         RootNode = $2; runBG = 1; YYACCEPT;}
     | line redir '&' '\n'              {fprintf(stdout, "line redir\n"); 
                                         RootNode = $2; runBG = 1; YYACCEPT;}
     | /*EMPTY*/      
     | line '\n'
     ;

command : WORD                          {fprintf(stdout, "WORD (%s)\n", $1); $$ = new_command($1, NULL);}
        | ERRTOK                        {fprintf(stdout, "cmd ERRTOK!\n"); YYABORT;}
        | WORD params                   {fprintf(stdout, "WORD (%s) params\n", $1);$$ = new_command($1, $2);}
        | ERRTOK params                 {fprintf(stdout, "ERRTOK! params\n"); YYABORT;}
        ;

redir : commandline '<' WORD                                     {fprintf(stdout, "< WORD\n"); 
                                                                $$ = new_redir($1, $3, NULL, 0, NULL, 0);}
      | commandline '<' WORD '>' WORD                            {fprintf(stdout, "< WORD > WORD\n");
                                                                $$ = new_redir($1, $3, $5, 0, NULL, 0);}
      | commandline '<' WORD '>' '>' WORD                        {fprintf(stdout, "< WORD >> WORD\n");
                                                                $$ = new_redir($1, $3, $6, 1, NULL, 0);}
      | commandline '<' WORD '>' WORD RE_STDERR WORD             {fprintf(stdout, "< WORD > WORD RE_STDERR WORD\n");
                                                                $$ = new_redir($1, $3, $5, 0, $7, 0);}
      | commandline '<' WORD '>' WORD RE_STDERR STDOUT           {fprintf(stdout, "< WORD > WORD RE_STDERR STOUT\n");
                                                                $$ = new_redir($1, $3, $5, 0, NULL, 1);}
      | commandline '<' WORD '>' '>' WORD RE_STDERR WORD         {fprintf(stdout, "< %s >> %s 2> %s\n", $3, $6, $8);
                                                                $$ = new_redir($1, $3, $6, 1, $8, 0);}
      | commandline '<' WORD '>' '>' WORD RE_STDERR STDOUT       {fprintf(stdout, "< WORD >> WORD RE_STDERR STOUT\n");
                                                                $$ = new_redir($1, $3, $6, 1, NULL, 1);}
      | commandline '>' WORD                                     {fprintf(stdout, "> WORD\n");
                                                                $$ = new_redir($1, NULL, $3, 0, NULL, 0);}
      | commandline '>' '>' WORD                                 {fprintf(stdout, ">> WORD\n");
                                                                $$ = new_redir($1, NULL, $4, 1, NULL, 0);}
      | commandline '>' WORD RE_STDERR WORD                      {fprintf(stdout, "> WORD RE_STDERR WORD\n");
                                                                $$ = new_redir($1, NULL, $3, 0, $5, 0);}
      | commandline '>' '>' WORD RE_STDERR WORD                  {fprintf(stdout, ">> WORD RE_STDERR WORD\n");
                                                                $$ = new_redir($1, NULL, $4, 1, $6, 0);}
      | commandline '>' WORD RE_STDERR STDOUT                    {fprintf(stdout, "> WORD RE_STDERR STDOUT\n");
                                                                $$ = new_redir($1, NULL, $3, 0, NULL, 1);}
      | commandline '>' '>' WORD RE_STDERR STDOUT                {fprintf(stdout, ">> WORD RE_STDERR STDOUT\n");
                                                                $$ = new_redir($1, NULL, $4, 1, NULL, 1);}
      | commandline '<' WORD RE_STDERR WORD                      {fprintf(stdout, "< WORD RE_STDERR WORD\n");
                                                                $$ = new_redir($1, $3, NULL, 0, $5, 0);}
      | commandline '<' WORD RE_STDERR STDOUT                    {fprintf(stdout, "< WORD RE_STDERR STDOUT\n");
                                                                $$ = new_redir($1, $3, NULL, 0, NULL, 1);}
      | commandline RE_STDERR WORD                               {fprintf(stdout, "RE_STDERR WORD\n");
                                                                $$ = new_redir($1, NULL, NULL, 0, $3, 0);}
      | commandline RE_STDERR STDOUT                             {fprintf(stdout, "RE_STDERR\n");
                                                                $$ = new_redir($1, NULL, NULL, 0, NULL, 0);}
      ;

commandline : commands                                          {fprintf(stdout, "commandline\n"); 
                                                                $$ = $1;}
            ;

param : WORD                            {fprintf(stdout, "WORD (%s)\n", $1); $$ = new_param($1);}
      | ERRTOK                          {fprintf(stdout, "ERRTOK!\n"); YYABORT;}
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
