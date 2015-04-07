#include <stdio.h>
#include <string.h>
#include "builtins.h"
#include <stdlib.h>
#include <unistd.h>

const bient bitab[] = {
  {"cd", x_chdir},
  {"setenv", x_setenv},
  {"unsetenv", x_unsetenv},
  {"printenv", x_printenv},
  {"alias", x_alias},
  {"unalias", x_unalias},
  {"pral", x_printalias},
  {"bye", x_bye}
};

int ncmds = sizeof(bitab) / sizeof(bient);

int x_chdir(int nargs, char *args[]) {
  // fprintf(stderr,"Executing cd...\n");
  int n = nargs;
  fprintf(stderr, "Size is: %i\n",nargs);
  char * dest;
  dest = args[0];
  if(nargs < 1)
  {
    fprintf(stderr, "No arguments supplied, ET phone home ~\n");
    char *b[2];
    b[0]="HOME";
    b[1]= getenv("HOME"); 
    dest = b[1];
  }

  if(chdir(dest) == -1)
  {
    fprintf(stderr, "Could not change!\n");
    return 1;
  }

  char * cwd = getcwd(0,0);
  fprintf(stderr, "Changed directory, now in %s\n",cwd);
  return 0;
}

int x_setenv(int nargs, char *args[]) {
  fprintf(stderr,"\nExecuting setenv...\n");
  int i = 0;
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

  return 0;
}

int x_unsetenv(int nargs, char *args[]) {
  int i = 0;
  
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
  return 0;
}

int x_printenv(int nargs, char *args[]) {
  fprintf(stderr,"\nExecuting printenv...\n");
  int i = 0;
  for(i = 0; i< 100; i++)
  {
    if(variables[i][0]!='\0' && !disabled[i])
    {
      char * var = variables[i];
      char * val = values[i];
      fprintf(stderr,"%s = %s\n", var, val );
    }
  }
  return 0;
}




/*---------*/ 

int x_alias(int nargs, char *args[]) {
  fprintf(stderr,"\nExecuting alias...\n");
  int i = 0;
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
      disabled[i] = 0; // In case it was previously disabled
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

  return 0;
}

int x_unalias(int nargs, char *args[]) {
  fprintf(stderr,"Executing unalias...\n");
  int i = 0;
  
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

  return 0;
}

/* FOR DEBUGGING ONLY, TODO REMOVE */
int x_printalias(int nargs, char *args[]) {
  fprintf(stderr,"\nExecuting print alias FOR DEBUGGING ONLY...\n");
  int i = 0;
  for(i = 0; i< 100; i++)
  {
    if(alias_names[i][0]!='\0' && !disabled[i])
    {
      char * var = alias_names[i];
      char * val = alias_vals[i];
      fprintf(stderr,"%s = %s\n", var, val );
    }
  }
  return 0;
}

int x_bye(int nargs, char *args[]) {
  int i;
  fprintf(stdout,"Goodbye");
  for (i=0; i<nargs; i++)
    fprintf(stdout, " %s", args[i]);
  fprintf(stdout, "\n");
  exit(0);
}
