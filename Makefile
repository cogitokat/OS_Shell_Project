all: shell

debug: lexer.l parser.y shell.c shell.h builtins.h builtins.c
	bison -y -d parser.y
	flex -d lexer.l
	gcc -o shelldbg y.tab.c lex.yy.c shell.c builtins.c -D DEBUG

shell: lexer.l parser.y shell.c shell.h builtins.h builtins.c
	bison -y parser.y
	flex lexer.l
	gcc -o shell y.tab.c lex.yy.c shell.c builtins.c


.PHONY: clean
clean:
	rm -f shell shelldbg
	rm -f *.o lex.yy.c y.tab.*
