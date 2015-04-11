all: shell

debug: shparser.l shparser.y shellparser.c shellparser.h builtins.h builtins.c
	bison -y -d shparser.y
	flex shparser.l
	gcc -o shelldbg y.tab.c lex.yy.c shellparser.c builtins.c -D DEBUG

shell: shparser.l shparser.y shellparser.c shellparser.h builtins.h builtins.c
	bison -y -d shparser.y
	flex shparser.l
	gcc -o shell y.tab.c lex.yy.c shellparser.c builtins.c


.PHONY: clean
clean:
	rm -f shell shelldbg
	rm -f *.o lex.yy.c y.tab.*
