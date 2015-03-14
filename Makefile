all: scanner

scanner: scanner.l scanner.y
	bison -y -d scanner.y
	flex scanner.l
	gcc -o scanner y.tab.c lex.yy.c

.PHONY: clean
clean:
	rm -f scanner
	rm -f *.o lex.yy.c y.tab.*
