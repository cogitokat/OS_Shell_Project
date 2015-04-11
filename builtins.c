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
  setenv("PWD", cwd, 1);
  fprintf(stdout, "Changed directory, now in %s\n",cwd);
  return 0;
}

int x_setenv(int nargs, char *args[]) {
  fprintf(stderr,"\nExecuting setenv...\n");
  
  if (nargs < 2) {
    fprintf(stderr, "setenv usage: setenv variable word\n Not enough arguments were provided.\n");
    return -1;
  }
  if (nargs > 2) {
    fprintf(stderr, "setenv usage: setenv variable word\n Too many arguments.\n");
    return -1;
  }
  if (nargs == 2) {
    if(setenv(args[0], args[1], 1) == -1) {
      fprintf(stderr, "The environment variable %s could not be set to %s.", args[0], args[1]);
      return -1;
    }
    fprintf(stderr, "Setting environment: '%s=%s'\n", args[0], args[1]);
    return 0;
  }
}


int x_unsetenv(int nargs, char *args[]) {

  if (nargs < 1) {
    fprintf(stderr, "unsetenv usage: unsetenv variable\nNot enough arguments were provided.\n");
    return -1;
  }
  if (nargs > 1) {
    fprintf(stderr, "unsetenv usage: unsetenv variable\nToo many arguments.\n");
    return -1;
  }
  if (nargs == 1) {
    if (getenv(args[0]) == NULL) {
      fprintf(stderr, "No environment variable %s found.", args[0]);
      return -1;
    }
    if (unsetenv(args[0]) == -1) {
      fprintf(stderr, "Could not remove environment variable %s.", args[0]);
      return -1;
    }
    fprintf(stderr, "Environment variable %s successfully removed.", args[0]);
    return 0;
  }
}

int x_printenv(int nargs, char *args[]) {
  fprintf(stderr,"\nExecuting printenv...\n");
  
  // Print all the variables
  int i;
  for(i = 0; environ[i] != NULL; i++) {
    fprintf(stdout, "%s\n",environ[i]);
  }

    return 0;  
}


/*---------*/ 

int x_alias(int nargs, char *args[]) {
  fprintf(stderr,"\nExecuting alias...\n");

  if (nargs == 1) { // Given alias name
    fprintf(stderr, "alias usage: alias name word\nNot enough arguments were provided.\n");
    return -1;
  }
  if (nargs > 2) { // Given alias name word word...
    fprintf(stderr, "alias usage: alias name word\nToo many arguments.\n");
    return -1;
  }
  if (nargs == 2) { // Given alias name word
  
    AliasEntry *currEntry;
    currEntry = rootAlias;
    
    /* Check if infinite alias
    if (checkInfiniteAlias(args[0], args[1]) == 1) {
      fprintf(stderr, "Cannot make infinite alias.");
      return -1;
    }
    
    */
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
          strncpy(currEntry->value, args[1], MAX_ALIAS);
          fprintf(stderr, "Updated alias %s=%s\n", currEntry->name, currEntry->value);
          return 0;
        } else if (currEntry->next == NULL) { // If the next entry is null, it's the end, so add new entry...
           AliasEntry *newNode = (AliasEntry*)malloc(sizeof(AliasEntry));
          if(newNode == NULL){
            fprintf(stderr, "Unable to allocate memory for new alias\n");
            return -1;
          }
          strncpy(newNode->name, args[0], MAX_ALIAS);
          strncpy(newNode->value, args[1], MAX_ALIAS);
          currEntry->next = newNode;
          fprintf(stderr, "Added new alias %s=%s\n", newNode->name, newNode->value);
          return 0;
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
    fprintf(stderr, "Alias list was not initialized.\n");
    return -1;
  }
  while(currEntry != NULL) {
    fprintf(stderr, "len name: %zd, len args[0]: %zd\n", strlen(currEntry->name), strlen(args[0]));
    if (strcmp(currEntry->name, args[0]) == 0) {
      prevEntry->next = currEntry->next;
      free(currEntry);
      return 0;
    }
    prevEntry = currEntry;
    currEntry = currEntry->next;
  }
  
  fprintf(stderr, "Couldn't delete %s, alias not found.\n", args[0]);
  return -1;
}

char * getAlias(char *inputstr) {
  fprintf(stderr,"Retrieving alias...\n");
    AliasEntry *currEntry;
    currEntry = rootAlias;
    while (currEntry != NULL) {
      if (strcmp(currEntry->name, inputstr) == 0 && strcmp(lastExpandedAlias, inputstr) != 0) {
        fprintf(stderr, "Yay, found alias: %s\n", currEntry->name);
        strncpy(lastExpandedAlias, currEntry->name, MAX_ALIAS); // Store the most recently expanded alias here
        return currEntry->value;
      }
      fprintf(stderr, "alias while loop\n");
      currEntry = currEntry->next;
    }
  return NULL;
}

/*
int checkInfiniteAlias(char * name, char * value) == 1) {
  // An alias is infinite if it contains its own name as the first "command".
  char *valtoken;
  valtoken = strtok(value," ");
  if (strcmp(name, valtoken) == 0) {
    return 1;
  }
  return 0;
  
}
*/


int x_bye(int nargs, char *args[]) {
  int i;
  fprintf(stdout,"Goodbye");
  for (i=0; i<nargs; i++)
    fprintf(stdout, " %s", args[i]);
  fprintf(stdout, "\n");
  exit(0);
}
