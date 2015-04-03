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

int x_chdir(int nargs, char *args[]) {
  printf("Executing cd...\n");
  return 0;
}

int x_setenv(int nargs, char *args[]) {
  printf("Executing setenv...\n");
  return 0;
}

int x_unsetenv(int nargs, char *args[]) {
  printf("Executing unsetenv...\n");
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
