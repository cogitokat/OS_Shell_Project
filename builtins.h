typedef struct {
  char* cmdname;
  int (*cmdfunc)(int, char*[]);
} bient;

extern const bient  bitab[];
extern int ncmds;

extern int x_chdir(int, char *[]);
extern int x_setenv(int, char *[]);
extern int x_unsetenv(int, char *[]);
extern int x_printenv(int, char *[]);
extern int x_alias(int, char *[]);
extern int x_unalias(int, char *[]);
extern int x_bye(int, char *[]);
