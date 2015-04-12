%{
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include "shell.h"
#include "builtins.h"
#include "y.tab.h"

#define MAX_STR_SIZE 3000
#define MAX_INCLUDE_DEPTH 30
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
int include_stack_ptr = 0;
char word_buf[MAX_STR_SIZE];

void stripEscapes(const char *inputstr);

%}

word ([^\t \n\"\?\'\&><\|\*]|\\&|\\\ |\\\"|\\>|\\<|\\\'|\\\|\\*)+
pattern ([^\t \n\"\\\'\&><\|]|\\&|\\\ |\\\"|\\>|\\<|\\\'|\\\|)+

%x DBLQUOTES
%x SNGLQUOTES
%s PROCESS

%%
                                        char string_buf[MAX_STR_SIZE];
                                        char *string_buf_ptr;
                                        
                                        
                                        { if (firstWord==1) {
                                            BEGIN(PROCESS);
                                            #if defined DEBUG
                                            fprintf( stderr, "from initial to process\n" );
                                            #endif
                                          }
                                        }
\"                                      {
                                         #if defined DEBUG
                                         fprintf(stderr, "begin dblquote from initial\n");
                                         #endif
                                         string_buf_ptr = string_buf; BEGIN(DBLQUOTES);
                                         }
\'                                      {
                                         #if defined DEBUG
                                         fprintf(stderr, "begin snglquote from initial\n");
                                         #endif
                                         string_buf_ptr = string_buf; BEGIN(SNGLQUOTES);
                                         }
 
 \$\{{word}\}                           { 
                                          #if defined DEBUG
                                          fprintf(stderr, "Hi, variable!\n");
                                          #endif
                                          char *varvalue;
                                          char subseq[yyleng-2];
                                          memcpy( subseq, &yytext[2], yyleng-3 );
                                          subseq[yyleng-3] = '\0';
                                          if ((varvalue = getenv(subseq)) != NULL){
                                            yylval.s = strdup(varvalue);
                                            return WORD;
                                           } else {
                                            #if defined DEBUG
                                            fprintf(stderr, "No variable %s, ignoring.\n", subseq);
                                            #endif
                                          }
                                        }

~                                      {
                                          #if defined DEBUG
                                          fprintf(stderr, "Hi, tilde! yytext: %s\n", yytext);
                                          #endif
                                          char *homedir;
                                          homedir = getenv("HOME");
                                          yylval.s = strdup(homedir);
                                          return WORD;
                                       }

~{word}                                {
                                          #if defined DEBUG
                                          fprintf(stderr, "Hi, tildeword! yytext: %s\n", yytext);
                                          #endif
                                          char subseq[yyleng];
                                          char *token;
                                          char *userdir;
                                          struct passwd *pwd;
                                          char finalpath[1024];
                                          memcpy(subseq, &yytext[1], yyleng-1);
                                          subseq[yyleng-1] = '\0';
                                          stripEscapes(subseq);
                                          token = strtok(word_buf, "/");
                                          #if defined DEBUG
                                          fprintf(stderr, "first token: %s\n", token);
                                          #endif
                                          pwd = getpwnam(token);
                                          if (pwd == NULL){
                                            yylval.s = strdup(yytext);
                                          } else {
                                            userdir = pwd->pw_dir;
                                            strcpy(finalpath, userdir);
                                            #if defined DEBUG
                                            fprintf(stderr, "copied %s to finalpath\n", userdir);
                                            #endif
                                            token = strtok(NULL, "/");
                                            while (token != NULL) {
                                              strcat(finalpath, "/");
                                              strcat(finalpath, token);
                                              #if defined DEBUG
                                              fprintf(stderr, "token: %s\n", token);
                                              #endif
                                              token = strtok(NULL, "/");
                                              #if defined DEBUG
                                              fprintf(stderr, "updated path: %s\n", finalpath);
                                              #endif
                                            }
                                            #if defined DEBUG
                                            fprintf(stderr, "final path: %s\n", finalpath);
                                            #endif
                                            yylval.s = strdup(finalpath);
                                          }
                                          return WORD;
                                       }

<DBLQUOTES>{
[^\"\n]*        { char *yptr = yytext;
                   while ( *yptr )
                   *string_buf_ptr++ = *yptr++;
                 }
\n               { printf(">> "); *string_buf_ptr++ = '\n'; }
\\[\\"]          { *string_buf_ptr++ = yytext[1];}
\"               { *string_buf_ptr = '\0';
                   #if defined DEBUG
                   fprintf(stderr, "string is: %s", string_buf);
                   #endif
                   yylval.s = strdup(string_buf); BEGIN(INITIAL); 
                   return WORD;}
}

<SNGLQUOTES>{
[^\'\n]*        { char *yptr = yytext;
                   while ( *yptr )
                   *string_buf_ptr++ = *yptr++;
                 }
\n               { printf(">> "); *string_buf_ptr++ = '\n'; }
\\[\\']          { *string_buf_ptr++ = yytext[1];}
\'               { *string_buf_ptr = '\0';
                   yylval.s = strdup(string_buf); BEGIN(INITIAL); 
                   return WORD;}
}

<PROCESS>{
[ \t]+                		              {;}
{word}      { char *alvalue;
              stripEscapes(yytext);
              if ((alvalue = getAlias(word_buf)) != NULL && firstWord == 1){
                if ( include_stack_ptr >= MAX_INCLUDE_DEPTH ) {
                  #if defined DEBUG
                  fprintf(stderr, "Cannot expand alias: nested too deeply.\n");
                  #endif
                  yylval.s = strdup(word_buf);
                  return WORD;
                }
                #if defined DEBUG
                fprintf(stderr, "alvalue: %s\n", alvalue);
                #endif
                include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
                yy_scan_string(alvalue);
                #if defined DEBUG
                fprintf(stderr, "switched buffer to str.\n");
                fprintf(stderr, "first word = %d\n", firstWord);
                #endif
                BEGIN(INITIAL);
              } else {
                yylval.s = strdup(word_buf);
                firstWord=0;
                #if defined DEBUG
                fprintf(stderr, "not an alias-> %s\n", word_buf);
                fprintf(stderr, "first word = %d\n", firstWord);
                #endif
                return WORD;
              }
            }
}

{word}                                  { if(firstWord==1) {
                                            #if defined DEBUG
                                            fprintf(stderr, "first word = %d\n", firstWord);
                                            #endif
                                            yyless(0); BEGIN(PROCESS); 
                                            #if defined DEBUG
                                            fprintf( stderr, "from initial to process w/ word\n" );
                                            #endif
                                          } else {
                                            yylval.s = strdup(yytext);
                                            #if defined DEBUG
                                            fprintf(stderr, "first word = %d\n", firstWord);
                                            fprintf(stderr, "not first word %s\n", yytext);
                                            #endif
                                            return WORD;
                                          }
                                        }
{pattern}                               { 
                                         #if defined DEBUG
                                         fprintf(stderr, "Found a pattern %s\n", yytext); 
                                         #endif
                                         stripEscapes(yytext);
                                         yylval.s = strdup(word_buf); return PATTERN;}

                                         
\&                                      {return *yytext;}
2>					                            {return RE_STDERR;}
\&1                                     {return STDOUT;}
[|]                                     {firstWord=1;
                                         #if defined DEBUG
                                         fprintf(stderr, "pipe, first word = %d\n", firstWord);
                                         #endif
                                         return *yytext;}
[><]                   		              {return *yytext;}
[\n]                                    {++inputlineno; return *yytext;}
[ ]+                 		                {;}

<<EOF>>                                 { if ( --include_stack_ptr < 0 ) {
                                            #if defined DEBUG
                                            fprintf(stderr, "include_stack_ptr=%d\n eof\n", include_stack_ptr);
                                            #endif
                                            yyterminate();
                                          } else {
                                            #if defined DEBUG
                                            fprintf(stderr, "include_stack_ptr=%d\ndel buf\n", include_stack_ptr);
                                            #endif
                                            yy_delete_buffer( YY_CURRENT_BUFFER );
                                            yy_switch_to_buffer( include_stack[include_stack_ptr] );
                                          }
                                        }

%%

int yywrap(void)
{
  return 1;
}

void stripEscapes(const char *inputstr) {
  int inputlen = strlen(inputstr);
  int i;
  int n = 0;
  for (i=0; i<inputlen; i++) {
    if (inputstr[i] == '\\' && inputstr[i+1] == '\\') {
      word_buf[n++] = '\\';
    }else if (inputstr[i] == '\\' && inputstr[i+1] != '\\') {
      word_buf[n++] = inputstr[++i];
    } else {
      word_buf[n++] = inputstr[i];
    }
  }
  word_buf[n++] = '\0';
  #if defined DEBUG
  fprintf(stderr, "wordbuff: %s\n", word_buf);
  #endif
}