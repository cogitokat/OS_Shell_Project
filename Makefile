all: scanner shell

scanner: scanner.l scanner.y
	bison -y -d scanner.y
	flex scanner.l
	gcc -o scanner y.tab.c lex.yy.c

shell: shparser.l shparser.y shellparser.c shellparser.h
	bison -y -d shparser.y
	flex shparser.l
	gcc -o shell y.tab.c lex.yy.c shellparser.c


.PHONY: clean
clean:
	rm -f scanner shell
	rm -f *.o lex.yy.c y.tab.*
