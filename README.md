# OS_Shell_Project
---
### Compile
```
make clean && make
```
### Run
Interactive:
```
./shell
```
File input:
```
./shell < inputfile
```
### Features
* Takes commands in the form of: 
```
cmd [arg] * [|cmd [arg] * ] * [< fn1]? [ >[>]? fn2 ]? [ 2>fn3 || 2>&1 ] ?[&]?
```
* Piping, I/O redirection, Background processing
* Environment variables:
	* setenv, unsetenv, printenv
    * replacement with ${VARIABLE}
* Aliasing:
	* alias, unalias
    * alias with no arguments prints all aliases
    * checks for infinite aliases
    * catches excessively nested aliases (30x)
* Other builtin commands:
	* cd changes directory and updates PWD variable
    * bye allows you to exit the shell
    	* SIGINT, SIGQUIT and SIGSTP are disabled in the shell process
    * All builtins can be forked, but running in a separate process negates some functionality. This is similar to implementation of other shells.
* Quotes:
	* Strings in quotes are taken literally
* Wildcard matching with * and ?
	* If no match is found, the pattern is used literally
* Errors:
	* Line numbers in non-interactive mode
    * Appropriate error messages
* Extra credit: tilde expansion
	* Tilde expansion occurs at the beginning of a word
    * Not implemented: tilde expansion within PATH variable
    
### Not implemented
* File completion
* Tilde expansion in PATH variable