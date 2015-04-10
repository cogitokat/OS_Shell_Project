#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "builtins.h"

extern char **environ;

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

int x_chdir(int nargs, char *args[]) {
  fprintf(stderr,"Executing cd...\n");
  char * dest;
  if(nargs == 0) {
    fprintf(stderr, "No arguments supplied, ET phone home ~\n");
    dest = getenv("HOME");
  } else if (nargs > 1) {
    fprintf(stderr, "Too many arguments.\n");
    return -1;
  } else dest = args[0];
  struct stat sb;
  if (stat(dest, &sb) != 0 ){
    fprintf(stderr, "%s: No such file or directory.\n", dest);
    return -1;
  }
  if (!S_ISDIR(sb.st_mode)){
    fprintf(stderr, "%s: Not a directory.\n", dest);
    return -1;
  }
  if(chdir(dest) == -1)
  {
    fprintf(stderr, "Could not change directories: %s\n", dest);
    return -1;
  }

  char * cwd = getcwd(0,0);
  fprintf(stdout, "Changed directory, now in %s\n",cwd);
  return 0;
}

int x_setenv(int nargs, char *args[]) {
  fprintf(stderr,"\nExecuting setenv...\n");
  /*int i = 0;
  //char * hey = args[0]; // Variable we are trying to store
  fprintf(stderr,"Got args[0]: %s\n", args[0]);
  //char hello[MAX_VAR_LENGTH]; // Variable trying to check, set to null

  while (variables[i][0]!='\0' && i < MAX_NUM_VARS ) // Find empty slot in variables array
  {
    fprintf(stderr,"I is currently %i and current varible in slot is %s\n",i,variables[i]);
    //strncpy(hello, variables[i], MAX_VAR_LENGTH);  // Variable at current slot
    if(strcmp(variables[i], args[0]) == 0) // If we have an entry, need to overwrite it
    {
      fprintf(stderr,"Equal: %s and %s\n",variables[i], args[0]);
      strncpy(values[i], args[1], MAX_VAL_LENGTH); // Overwrite value
      disabled[i] = 0; // In case it was previously disabled
      fprintf(stderr,"OVERWROTE %s = %s at spot %i\n",args[0],variables[i], i); // Not at end if here
      return 0;
    }
    i++;
  }
  if(i == MAX_NUM_VARS)  // Now should be at empty slot if not at end
  {
    fprintf(stderr,"Oh noes at the end :(\n");
    return -1;
  }

  strncpy(variables[i], args[0], MAX_VAR_LENGTH);
  strncpy(values[i], args[1], MAX_VAL_LENGTH);

  fprintf(stderr,"SET %s to %s at spot %i\n",variables[i], values[i], i); // Not at end if here

  return 0;*/
  
  if (nargs < 2) {
    fprintf(stderr, "setenv usage: setenv variable word\n Not enough arguments were provided.\n");
    return -1;
  } else if (nargs > 2) {
    fprintf(stderr, "setenv usage: setenv variable word\n Too many arguments.\n");
    return -1;
  } else if (nargs == 2) {
    VariableEntry *currEntry;
    currEntry = rootVariable;
    if (currEntry == NULL) {
      fprintf(stderr, "Why is rootVariable NULL??\n");
      return -1;
    }
    // If the list is empty, everything is easy...
    if(rootVariable->next == NULL){
       VariableEntry *newNode = (VariableEntry*)malloc(sizeof(VariableEntry));
      if(newNode == NULL){
        fprintf(stderr, "Unable to allocate memory for new variable\n");
        return -1;
      }
      strncpy(newNode->name, args[0], MAX_VAR);
      strncpy(newNode->value, args[1], MAX_VAR);
      rootVariable->next = newNode;
      fprintf(stderr, "Added new variable %s=%s as first item\n", newNode->name, newNode->value);
      return 0;
    } else {
      // Iterate through the variable list...
      while (currEntry != NULL) {
        // Check if the variable is in the list, and replace if so.
        if (strcmp(currEntry->name, args[0]) == 0) {
          strncpy(currEntry->value, args[1], MAX_VAR);
          fprintf(stderr, "Updated variable %s=%s\n", currEntry->name, currEntry->value);
          return 0;
        } else if (currEntry->next == NULL) { // If the next entry is null, it's the end, so add new entry...
           VariableEntry *newNode = (VariableEntry*)malloc(sizeof(VariableEntry));
          if(newNode == NULL){
            fprintf(stderr, "Unable to allocate memory for new variable\n");
            return -1;
          }
          strncpy(newNode->name, args[0], MAX_VAR);
          strncpy(newNode->value, args[1], MAX_VAR);
          currEntry->next = newNode;
          fprintf(stderr, "Added new variable %s=%s\n", newNode->name, newNode->value);
          return 0;
        }
        currEntry = currEntry->next;
      }
    }
  }
  fprintf(stderr, "Could not set variable %s to %s.", args[0], args[1]);
  return -1;
}


int x_unsetenv(int nargs, char *args[]) {
/*  int i = 0;
  
  char * hey = args[0];
  int equal = 1;
  while(equal != 0 && i<100)
  {
    char * hello = variables[i];
    equal = (strcmp(hello, hey));
    if(equal == 0)
    {
      fprintf(stderr,"Found at slot %i, setting disabled bit\n",i);
      disabled[i] = (int) 1;
      break;
    }
    i++;
  }

  if(i==100)
  {
    // fprintf(stderr,"Cound not find %s\n", hey);
    return 0;
  }

  // fprintf(stderr,"Executing unsetenv...\n");
  return 0;*/
  if (nargs == 0) {
    fprintf(stderr, "unsetenv usage: unsetenv variable\nNot enough arguments were provided.\n");
    return -1;
  } else if (nargs > 1) {
    fprintf(stderr, "unsetenv usage: unsetenv variable\nToo many arguments.\n");
    return -1;    
  }
  
  
  
   VariableEntry *prevEntry;
   VariableEntry *currEntry;
  prevEntry = NULL;
  currEntry = rootVariable;
  if (currEntry == NULL) {
    fprintf(stderr, "Why is rootVariable NULL??\n");
    return -1;
  }
  while(currEntry->next != NULL) {
    if (strcmp(currEntry->name, args[0]) == 0) {
      prevEntry->next = currEntry->next;
      free(currEntry);
      return 0;
    }
    prevEntry = currEntry;
    currEntry = currEntry->next;
  }
  
  fprintf(stderr, "Couldn't delete %s, variable not found.", args[0]);
  return -1;
}

int x_printenv(int nargs, char *args[]) {
  fprintf(stderr,"\nExecuting printenv...\n");
/*  int i = 0;
  for(i = 0; i< 100; i++)
  {
    if(variables[i][0]!='\0' && !disabled[i])
    {
      char * var = variables[i];
      char * val = values[i];
      fprintf(stderr,"%s = %s\n", var, val );
    }
  }

  char * hello[] = {"PATH"}; // this list argument for getal function
                              //  TODO: replace to take in from other function
  const  char *strGetal; 
  strGetal = getvar(1,hello);
  //char * hello ="hehehe";
    fprintf(stderr,"Got env variable for home --> %s",strGetal);
//    fprintf(stderr, "Could not find variable\n");
  return 0;*/
    VariableEntry *currEntry;
    currEntry = rootVariable;
    if (currEntry == NULL) {
      fprintf(stderr, "Variable list was not initialized\n");
      return -1;
    }
    
    // Print all the variables that are not part of the shell
    int i;
    for(i = 0; environ[i] != NULL; i++) {
      fprintf(stdout, "%s\n",environ[i]);
    }
    
    currEntry = rootVariable->next;
    while (currEntry!=NULL) {
      // Print all the variables that are only part of the shell
      if (getenv(currEntry->name)==NULL){
        fprintf(stdout, "%s=%s\n", currEntry->name, currEntry->value);
      }
      currEntry = currEntry->next;
    }
    return 0;  
}




/*---------*/ 

int x_alias(int nargs, char *args[]) {
  fprintf(stderr,"\nExecuting alias...\n");
  /*int i = 0;
  //char * hey = args[0]; // Variable we are trying to store
  fprintf(stderr,"Got args[0]: %s\n", args[0]);
  //char hello[MAX_VAR_LENGTH]; // Variable trying to check, set to null
  fprintf(stderr,"Current alias at spot i: %s\n", alias_names[0]);
  while (alias_names[i][0]!='\0' && i < MAX_ALIAS_LENGTH ) // Find empty slot in variables array
  {
    fprintf(stderr,"I is currently %i and current varible in slot is %s\n",i,alias_names[i]);
    //strncpy(hello, variables[i], MAX_VAR_LENGTH);  // Variable at current slot
    if(strcmp(alias_names[i], args[0]) == 0) // If we have an entry, need to overwrite it
    {
      fprintf(stderr,"Equal: %s and %s\n",alias_names[i], args[0]);
      strncpy(alias_vals[i], args[1], MAX_VAL_LENGTH); // Overwrite value
      alias_disabled[i] = 0; // In case it was previously disabled
      fprintf(stderr,"OVERWROTE %s = %s at spot %i\n",args[0],alias_names[i], i); // Not at end if here
      return 0;
    }
    i++;
  }
  if(i == MAX_NUM_VARS)  // Now should be at empty slot if not at end
  {
    fprintf(stderr,"Oh noes at the end :(\n");
    return 1;
  }

  strncpy(alias_names[i], args[0], MAX_VAR_LENGTH);
  strncpy(alias_vals[i], args[1], MAX_VAL_LENGTH);

  fprintf(stderr,"SET %s to %s at spot %i\n",alias_names[i], alias_vals[i], i); // Not at end if here

  return 0;*/
  if (nargs == 1) { // Given alias name
    fprintf(stderr, "alias usage: alias name word\nNot enough arguments were provided.\n");
    return -1;
  } else if (nargs > 2) { // Given alias name word word...
    fprintf(stderr, "alias usage: alias name word\nToo many arguments.\n");
    return -1;
  } else if (nargs == 2) { // Given alias name word
    AliasEntry *currEntry;
    currEntry = rootAlias;
    if (currEntry == NULL) {
      fprintf(stderr, "Alias list was not initialized.\n");
      return -1;
    }
    // If the list is empty, everything is easy...
    if(rootAlias->next == NULL){
       AliasEntry *newNode = (AliasEntry*)malloc(sizeof(AliasEntry));
      if(newNode == NULL){
        fprintf(stderr, "Unable to allocate memory for new alias\n");
        return -1;
      }
      strncpy(newNode->name, args[0], MAX_ALIAS);
      strncpy(newNode->value, args[1], MAX_ALIAS);
      rootAlias->next = newNode;
      fprintf(stderr, "Added new alias %s=%s as first item\n", newNode->name, newNode->value);
    } else { 
      // Iterate through the variable list...
      while (currEntry != NULL) {
        // Check if the variable is in the list, and replace if so.
        if (strcmp(currEntry->name, args[0]) == 0) {
          strncpy(currEntry->value, args[1], MAX_VAR);
          fprintf(stderr, "Updated alias %s=%s\n", currEntry->name, currEntry->value);
          return 0;
        } else if (currEntry->next == NULL) { // If the next entry is null, it's the end, so add new entry...
           AliasEntry *newNode = (AliasEntry*)malloc(sizeof(AliasEntry));
          if(newNode == NULL){
            fprintf(stderr, "Unable to allocate memory for new alias\n");
            return -1;
          }
          strncpy(newNode->name, args[0], MAX_VAR);
          strncpy(newNode->value, args[1], MAX_VAR);
          currEntry->next = newNode;
          fprintf(stderr, "Added new alias %s=%s\n", newNode->name, newNode->value);
        }
        currEntry = currEntry->next;
      }
    }
  } else if (nargs == 0) { // Given alias
     AliasEntry *currEntry;
    currEntry = rootAlias;
    if (currEntry == NULL) {
      fprintf(stderr, "Alias list was not initialized.\n");
      return -1;
    }
    currEntry = rootAlias->next;
    while (currEntry != NULL) {
      // Print all the aliases!
      fprintf(stdout, "%s=%s\n", currEntry->name, currEntry->value);
      currEntry = currEntry->next;
    }
    return 0;
  } 
}

int x_unalias(int nargs, char *args[]) {
  fprintf(stderr,"Executing unalias...\n");
 /* int i = 0;
  
  char * newVar = args[0];
  int equal = 1;
  while(equal != 0 && i<100)
  {
    char * oldVar = alias_names[i]; // Get current in table
    equal = (strcmp(oldVar, newVar)); // Compare to see if same
    if(equal == 0)
    {
      fprintf(stderr,"Found at slot %i, setting disabled bit\n",i);
      alias_disabled[i] = (int) 1;
      break;
      return 0;
    }
    i++;
  }

  if(i==100)
  {
    fprintf(stderr,"Cound not find %s\n", newVar);
    return 0;
  }

  return 0; */
  if (nargs == 0) {
    fprintf(stderr, "unalias usage: unalias name\nNot enough arguments were provided.\n");
    return -1;
  } else if (nargs > 1) {
    fprintf(stderr, "unalias usage: unalias name\nToo many arguments.\n");
    return -1;    
  }
  AliasEntry *prevEntry;
  AliasEntry *currEntry;
  prevEntry = NULL;
  currEntry = rootAlias;
  if (currEntry == NULL) {
    fprintf(stderr, "Why is rootAlias NULL??\n");
    return -1;
  }
  while(currEntry->next != NULL) {
    if (strcmp(currEntry->name, args[0]) == 0) {
      prevEntry->next = currEntry->next;
      free(currEntry);
      return 0;
    }
    prevEntry = currEntry;
    currEntry = currEntry->next;
  }
  
  fprintf(stderr, "Couldn't delete %s, alias not found.", args[0]);
  return -1;
}

int x_bye(int nargs, char *args[]) {
  int i;
  fprintf(stdout,"Goodbye");
  for (i=0; i<nargs; i++)
    fprintf(stdout, " %s", args[i]);
  fprintf(stdout, "\n");
  exit(0);
}
