# OS_Shell_Project
---

### scanner
For now, scanner takes a file as input and parses it.

```
make clean && make scanner
./scanner < test.in
```
The output should be:
```
WORD (-l)
param
WORD (ls) params
command
WORD (wc)
commands | command
line commands
WORD (myShell.c)
param
WORD (cat) params
line command
```
Each time scanner matches a production and is able to reduce, it prints the
right hand side of the production, as well as any (token).

### shell
A start to the shell. It will take lines of input from stdin and print a
line representing the AST.

```
make clean && make shell
./shell
```
... then type some example commands. It is very buggy and does not execute
anything, yet.
