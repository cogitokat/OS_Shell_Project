%{
#include <stdio.h>
#include <string.h>
#include "shellparser.h"
#include "builtins.h"
#include "y.tab.h"

#define MAX_STR_SIZE 1000
#define MAX_INCLUDE_DEPTH 10
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
int include_stack_ptr = 0;


%}

word ([^\t \n\"\?\\\'\&><\|\*]|\\&|\\\"|\\>|\\<|\\\'|\\\|)+
pattern ([^\t \n\"\\\'\&><\|]|\\&|\\\"|\\>|\\<|\\\'|\\\|)+
%x DBLQUOTES
%x SNGLQUOTES
%x VARIABLE
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

<DBLQUOTES>{
[^\\"\n]*      { char *yptr = yytext;
                   while ( *yptr )
                   *string_buf_ptr++ = *yptr++;
               }
\n               { printf(">> "); *string_buf_ptr++ = '\n'; }
\\n              { *string_buf_ptr++ = '\n'; }
\\t              { *string_buf_ptr++ = '\t'; }
\\[\\"]          { *string_buf_ptr++ = yytext[1];}
\"               { *string_buf_ptr = '\0';
                   yylval.s = strdup(string_buf); BEGIN(INITIAL); 
                   return WORD;}
}

<SNGLQUOTES>{
[^\\'\n]*      { char *yptr = yytext;
                   while ( *yptr )
                   *string_buf_ptr++ = *yptr++;
               }
\n               { printf(">> "); *string_buf_ptr++ = '\n'; }
\\n              { *string_buf_ptr++ = '\n'; }
\\t              { *string_buf_ptr++ = '\t'; }
\\[\\']          { *string_buf_ptr++ = yytext[1];}
\'               { *string_buf_ptr = '\0';
                   yylval.s = strdup(string_buf); BEGIN(INITIAL); 
                   return WORD;}
}

<PROCESS>{
[ \t]+                		              {;}
{word}    { char *alvalue;
              if ((alvalue = getAlias(yytext)) != NULL && firstWord == 1){
                if ( include_stack_ptr >= MAX_INCLUDE_DEPTH ) {
                  #if defined DEBUG
                  fprintf(stderr, "Cannot expand alias: nested too deeply.\n");
                  #endif
                  yylval.s = strdup(yytext);
                  return WORD;
                }
                fprintf(stderr, "alvalue: %s\n", alvalue);
                include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
                yy_scan_string(alvalue);
                #if defined DEBUG
                fprintf(stderr, "switched buffer to str.\n");
                fprintf(stderr, "first word = %d\n", firstWord);
                #endif
                BEGIN(INITIAL);
              } else {
                yylval.s = strdup(yytext);
                firstWord=0;
                #if defined DEBUG
                fprintf(stderr, "not an alias-> %s\n", yytext);
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
{pattern}                              { 
                                         #if defined DEBUG
                                         fprintf(stderr, "YAY A PATTERN! %s\n", yytext); 
                                         #endif
                                         yylval.s = strdup(yytext); return PATTERN;}

                                         
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
[ \t]+                		              {;}
.                       		            { yyerror("Invalid character!\n");}
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
