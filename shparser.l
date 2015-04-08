%{
#include <stdio.h>
#include <string.h>
#include "shellparser.h"
#include "y.tab.h"

#define MAX_STR_SIZE 1000
#define MAX_INCLUDE_DEPTH 10
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
int include_stack_ptr = 0;

%}

word ([^\t \n\"\?\\\'\&><\|]|\\&|\\\"|\\>|\\<|\\\'|\\\|)+
%x DBLQUOTES
%x VARIABLE
%s PROCESS

%%
                                        char string_buf[MAX_STR_SIZE];
                                        char *string_buf_ptr;
                                      
                                        { if (firstWord==1) {
                                            BEGIN(PROCESS);
                                            fprintf( stderr, "from initial to process\n" );
                                          }
                                        }
\"                                      {string_buf_ptr = string_buf; BEGIN(DBLQUOTES);}
\$\{                                    {fprintf(stderr, "Hi, variable!\n"); BEGIN(VARIABLE);}

<DBLQUOTES>{
[^\\"\n]*      { char *yptr = yytext;
                   while ( *yptr )
                   *string_buf_ptr++ = *yptr++;
               }
\n               { printf(">> "); *string_buf_ptr++ = '\n'; }
\\n              { *string_buf_ptr++ = '\n'; }
\\t              { *string_buf_ptr++ = '\t'; }
\\[\\"]          { *string_buf_ptr++ = yytext[1];}
\"               { yylval.s = strdup(string_buf); BEGIN(INITIAL); 
                   *string_buf_ptr = '\0'; return WORD;}
}

<VARIABLE>{     
{word}\}      { firstWord=0;
                if (strcmp(yytext, "variable") == 0){
                  fprintf(stderr, "switched buffer to str.\n");
                  fprintf(stderr, "first word = %d\n", firstWord);
                  yylval.s = strdup("cd");
                  BEGIN(INITIAL);
                  return WORD;
                } else {
                  fprintf(stderr, "No variable %s\n", yytext);
                  BEGIN(INITIAL);
                }
              }
[^\n\t ><\$\&\"]   {fprintf(stderr, "Invalid variable!\n"); return ERRTOK;}
}

<PROCESS>{
\"                                      {fprintf(stderr, "begin quote from process\n"); string_buf_ptr = string_buf; BEGIN(DBLQUOTES);}
\$\{                                    {fprintf(stderr, "Hi, variable!\n"); BEGIN(VARIABLE);}
[ \t]+                		              {;}
{word}   { if ((strcmp(yytext, "test") == 0) && firstWord == 1){
                if ( include_stack_ptr >= MAX_INCLUDE_DEPTH ) {
                  fprintf( stderr, "aliases nested too deeply" );
                  exit(1);
                }
                include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
                yy_scan_string("cd");
                fprintf(stderr, "switched buffer to str.\n");
                fprintf(stderr, "first word = %d\n", firstWord);
                BEGIN(INITIAL);
              } else {
                yylval.s = strdup(yytext);
                fprintf(stderr, "not test-> %s\n", yytext);
                firstWord=0;
                fprintf(stderr, "first word = %d\n", firstWord);
                return WORD;
            }
          }
}



{word}                                  { if(firstWord==1) {
                                            fprintf(stderr, "first word = %d\n", firstWord);
                                            yyless(0); BEGIN(PROCESS); 
                                            fprintf( stderr, "from initial to process w/ word\n" );
                                          } else {
                                            yylval.s = strdup(yytext);
                                            fprintf(stderr, "first word = %d\n", firstWord);
                                            fprintf(stderr, "not first word %s\n", yytext);
                                            return WORD;
                                          }
                                        }
\&                                      {return *yytext;}
2>					                            {return RE_STDERR;}
\&1                                     {return STDOUT;}
[|]                                     {firstWord=1; fprintf(stderr, "pipe, first word = %d\n", firstWord); return *yytext;}
[><\n]                 		              {return *yytext;}
[ \t]+                		              {;}
.                       		            {fprintf(stderr, "Invalid character! %s\n", yytext); return ERRTOK;}
<<EOF>>                                 { if ( --include_stack_ptr < 0 ) {
                                              fprintf(stderr, "include_stack_ptr=%d\n eof\n", include_stack_ptr);
                                              return END_OF_FILE;
                                            } else {
                                              fprintf(stderr, "include_stack_ptr=%d\ndel buf\n", include_stack_ptr);
                                              yy_delete_buffer( YY_CURRENT_BUFFER );
                                              yy_switch_to_buffer( include_stack[include_stack_ptr] );
                                            }
                                          }

%%

int yywrap(void)
{
  return 1;
}
