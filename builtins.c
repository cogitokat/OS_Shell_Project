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
  #if defined DEBUG
  fprintf(stderr,"Executing cd...\n");
  #endif
  char * dest;
  if(nargs == 0) {
    shell_error("No arguments supplied, ET phone home ~");
    dest = getenv("HOME");
  } else if (nargs > 1) {
    shell_error("Too many arguments");
    return -1;
  } else dest = args[0];
  struct stat sb;
  if (stat(dest, &sb) != 0 ){
    shell_error("%s: No such file or directory", dest);
    return -1;
  }
  if (!S_ISDIR(sb.st_mode)){
    shell_error("%s: Not a directory.", dest);
    return -1;
  }
  if(chdir(dest) == -1)
  {
    shell_error("Could not change directories: %s", dest);
    return -1;
  }

  char * cwd = getcwd(0,0);
  setenv("PWD", cwd, 1);
  fprintf(stdout, "Changed directory, now in %s\n",cwd);
  return 0;
}

int x_setenv(int nargs, char *args[]) {
  #if defined DEBUG
  fprintf(stderr,"\nExecuting setenv...\n");
  #endif
  
  if (nargs < 2) {
    shell_error("setenv usage: setenv variable word\n Not enough arguments were provided");
    return -1;
  }
  if (nargs > 2) {
    shell_error("setenv usage: setenv variable word\n Too many arguments");
    return -1;
  }
  if (nargs == 2) {
    if(setenv(args[0], args[1], 1) == -1) {
      shell_error("The environment variable %s could not be set to %s", args[0], args[1]);
      return -1;
    }
    fprintf(stdout, "Setting environment: '%s=%s'\n", args[0], args[1]);
    return 0;
  }
}


int x_unsetenv(int nargs, char *args[]) {

  if (nargs < 1) {
    shell_error("unsetenv usage: unsetenv variable\nNot enough arguments were provided");
    return -1;
  }
  if (nargs > 1) {
    shell_error("unsetenv usage: unsetenv variable\nToo many arguments.");
    return -1;
  }
  if (nargs == 1) {
    if (getenv(args[0]) == NULL) {
      shell_error("No environment variable %s found", args[0]);
      return -1;
    }
    if (unsetenv(args[0]) == -1) {
      shell_error("Could not remove environment variable %s", args[0]);
      return -1;
    }
    fprintf(stdout, "Environment variable %s successfully removed.\n", args[0]);
    return 0;
  }
}

int x_printenv(int nargs, char *args[]) {
  #if defined DEBUG
  fprintf(stderr,"\nExecuting printenv...\n");
  #endif
  
  // Print all the variables
  int i;
  for(i = 0; environ[i] != NULL; i++) {
    fprintf(stdout, "%s\n",environ[i]);
  }

    return 0;  
}


/*---------*/ 

int x_alias(int nargs, char *args[]) {
  #if defined DEBUG
  fprintf(stderr,"\nExecuting alias...\n");
  #endif

  if (nargs == 1) { // Given alias name
    shell_error("alias usage: alias name word\nNot enough arguments were provided");
    return -1;
  }
  if (nargs > 2) { // Given alias name word word...
    shell_error("alias usage: alias name word\nToo many arguments");
    return -1;
  }
  if (nargs == 2) { // Given alias name word
  
    AliasEntry *currEntry;
    currEntry = rootAlias;
    
    if (currEntry == NULL) {
      shell_error("Alias list was not initialized");
      return -1;
    }
    // If the list is empty, everything is easy...
    if(rootAlias->next == NULL){
       AliasEntry *newNode = (AliasEntry*)malloc(sizeof(AliasEntry));
      if(newNode == NULL){
        shell_error("Unable to allocate memory for new alias");
        return -1;
      }
      strncpy(newNode->name, args[0], MAX_LENGTH);
      strncpy(newNode->value, args[1], MAX_LENGTH);
      rootAlias->next = newNode;
      fprintf(stdout, "Added new alias %s=%s as first item\n", newNode->name, newNode->value);
    } else { 
      // Iterate through the variable list...
      while (currEntry != NULL) {
        // Check if the variable is in the list, and replace if so.
        if (strcmp(currEntry->name, args[0]) == 0) {
          strncpy(currEntry->value, args[1], MAX_LENGTH);
          fprintf(stdout, "Updated alias %s=%s\n", currEntry->name, currEntry->value);
          return 0;
        } else if (currEntry->next == NULL) { // If the next entry is null, it's the end, so add new entry...
           AliasEntry *newNode = (AliasEntry*)malloc(sizeof(AliasEntry));
          if(newNode == NULL){
            shell_error("Unable to allocate memory for new alias");
            return -1;
          }
          strncpy(newNode->name, args[0], MAX_LENGTH);
          strncpy(newNode->value, args[1], MAX_LENGTH);
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
      #if defined DEBUG
      fprintf(stderr, "Alias list was not initialized.\n");
      #endif
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
  #if defined DEBUG
  fprintf(stderr,"Executing unalias...\n");
  #endif

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
    shell_error("Alias list was not initialized");
    return -1;
  }
  while(currEntry != NULL) {
    #if defined DEBUG
    fprintf(stderr, "len name: %zd, len args[0]: %zd\n", strlen(currEntry->name), strlen(args[0]));
    #endif
    if (strcmp(currEntry->name, args[0]) == 0) {
      prevEntry->next = currEntry->next;
      free(currEntry);
      return 0;
    }
    prevEntry = currEntry;
    currEntry = currEntry->next;
  }
  
  shell_error("Couldn't delete %s, alias not found", args[0]);
  return -1;
}

char * getAlias(char *inputstr) {
  #if defined DEBUG
  fprintf(stderr,"Retrieving alias...\n");
  #endif
    AliasEntry *currEntry;
    currEntry = rootAlias;
    while (currEntry != NULL) {
      if (strcmp(currEntry->name, inputstr) == 0 && strcmp(lastExpandedAlias, inputstr) != 0) {
        #if defined DEBUG
        fprintf(stderr, "Found alias: %s\n", currEntry->name);
        #endif
        strncpy(lastExpandedAlias, currEntry->name, MAX_LENGTH); // Store the most recently expanded alias here
        return currEntry->value;
      }
      #if defined DEBUG
      fprintf(stderr, "alias while loop\n");
      #endif
      currEntry = currEntry->next;
    }
  return NULL;
}

int x_bye(int nargs, char *args[]) {
  int i;
  fprintf(stdout,"Goodbye");
  for (i=0; i<nargs; i++)
    fprintf(stdout, " %s", args[i]);
  fprintf(stdout, "\n");
  exit(0);
}
