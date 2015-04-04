#include <stdio.h>
#include "builtins.h"

const bient bitab[] = {
  {"cd", x_chdir},
  {"setenv", x_setenv},
  {"unsetenv", x_unsetenv},
  {"printenv", x_printenv},
  {"alias", x_alias},
  {"unalias", x_unalias},
  {"bye", x_bye}
};

int ncmds = sizeof(bitab) / sizeof(bient);

char * variables [100]; // Holds the variables
char * values [100];  // Holds the values
int * disabled [100]; // Holds whether or not variable has been unset (1 if has been unset)

int x_chdir(int nargs, char *args[]) {
  printf("Executing cd...\n");
  return 0;
}

int x_setenv(int nargs, char *args[]) {
  printf("\nExecuting setenv...\n");
  int i = 0;
  char * hey = args[0]; // Variable we are trying to store
  printf("Got args[0]: %s\n", hey);
  char * hello = '\0'; // Variable trying to check, set to null

  while (variables[i] && i < 100 ) // Find empty slot in variables array
  {
    printf("I is currently %i and current varible in slot is %s\n",i,variables[i]);
    hello = variables[i];  // Variable at current slot
    if(strcmp(hello, hey) == 0) // If we have an entry, need to overwrite it
    {
      printf("Equal: %s and %s",hello, hey);
      values[i] = args[1]; // Overwrite value
      disabled[i] = 0; // In case it was previously disabled
      printf("OVERWROTE %s = %s at spot %i\n",hey, hello, i); // Not at end if here
      return 0;
    }
    i++;
  }
  if(i == 100)  // Now should be at empty slot if not at end
  {
    printf("Oh noes at the end :(\n");
    return 1;
  }

  variables[i] = args[0]; // Set variable
  values[i] = args[1];  // Set value

  printf("SET %s to %s at spot %i\n",variables[i], values[i], i); // Not at end if here

  return 0;
}

int x_unsetenv(int nargs, char *args[]) {
  /*
  int i = 0;
  
  char * hey = args[0];
  int equal = 1;
  while(equal != 0 && i<100)
  {
    char * hello = variables[i];
    equal = (strcmp(hello, hey));
  }
  if(equal)
    printf("Found at slot %i\n",i);

  printf("Executing unsetenv...\n");
  */
  return 0;
}

int x_printenv(int nargs, char *args[]) {
  printf("Executing printenv...\n");
  return 0;
}

int x_alias(int nargs, char *args[]) {
  printf("Executing alias...\n");
  return 0;
}

int x_unalias(int nargs, char *args[]) {
  printf("Executing unalias...\n");
  return 0;
}

int x_bye(int nargs, char *args[]) {
  printf("Executing bye...\n");
  return 0;
}



