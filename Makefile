all: scanner

scanner: scanner.l scanner.h scanner.c
	flex scanner.l
	gcc -o scanner lex.yy.c scanner.c

.PHONY: clean
clean:
	rm -f scanner
	rm -f lex.yy.c
