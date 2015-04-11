%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glob.h>
#include "builtins.h"
#include "shellparser.h"

extern void yyerror(const char *msg);
extern int yylineno;
extern char * yytext;
extern int yylex(void);

Node* expandPattern(char* pattern);

%}

%error-verbose

%union {
  int num;
  char* s;
  Node *np;
};

%token <num> NUMBER
%token <s> WORD PATTERN
%token ERRTOK STDOUT RE_STDERR
%token '\n' '>' '<' '&'
%left '|'
%type <np> command commands redir params param commandline

%%

start : line                            {fprintf(stdout, "start\n"); doneParsing = 1; }
      | error                           {fprintf(stdout, "General error\n"); YYABORT;}
      ;

line : line commands '\n'               {fprintf(stdout, "line commands\n"); 
                                         RootNode = $2; runBG = 0; return 0;}
     | line redir '\n'                  {fprintf(stdout, "line redir\n");
                                         RootNode = $2; runBG = 0; return 0;}
     | line commands '&' '\n'           {fprintf(stdout, "line command\n"); 
                                         RootNode = $2; runBG = 1; return 0;}
     | line redir '&' '\n'              {fprintf(stdout, "line redir\n"); 
                                         RootNode = $2; runBG = 1; return 0;}
     | /*EMPTY*/      
     | line '\n'
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

commands : command '|' commands         {fprintf(stdout, "command | commands\n"); $$ = new_pipe($1, $3);}
         | command                      {fprintf(stdout, "command\n"); $$ = new_pipe($1, NULL);}
         ;

command : WORD                          {fprintf(stdout, "WORD (%s)\n", $1); $$ = new_command($1, NULL);}
        | WORD params                   {fprintf(stdout, "WORD (%s) params\n", $1);$$ = new_command($1, $2);}
        | PATTERN '\n'                  {yyerror("syntax error, pattern received instead of a command\n"); YYABORT;}
        ;         

params : param                          {fprintf(stdout, "param\n"); $$ = new_params($1, NULL);}
       | PATTERN                        {fprintf(stdout, "PATTERN (%s)\n", $1); 
                                           $$ = expandPattern($1); }
       | param params                   {fprintf(stdout, "param params\n"); $$ = new_params($1, $2);}
       ;
       
param : WORD                            {fprintf(stdout, "WORD (%s)\n", $1); $$ = new_param($1);}
      ;
      
%%

void yyerror(const char *msg) {
  if (isatty(0)){
    fprintf(stderr, "%s\n", msg);
  } else {
    fprintf(stderr, "line %d: %s\n", inputlineno, msg);
  }
}

Node* expandPattern(char* pattern) {
   glob_t globbuf;
   Node* retParams = new_params(NULL, NULL);
   
   glob(pattern, GLOB_NOCHECK, NULL, &globbuf);
   
   if(globbuf.gl_pathc==0) { // No matches for our wildcards
    Node* newparam = new_param(pattern);
    retParams->type.ParamsNode.first = newparam;
    retParams->type.ParamsNode.second = NULL;
   } else if(globbuf.gl_pathc>0) { // Results!!
    int i;
    Node* nparam;
    Node* nparams = retParams;
    for(i=0;i<globbuf.gl_pathc;i++)
    {
     nparam = new_param(globbuf.gl_pathv[i]);
     nparams->type.ParamsNode.first = nparam;
     if(i+1==globbuf.gl_pathc) { // We are on our final iteration 
       nparams->type.ParamsNode.second = NULL;
     } else {
      nparams->type.ParamsNode.second = new_params(NULL,NULL);
      nparams = nparams->type.ParamsNode.second;
     }
    }
   }
  return retParams;
   
 }