#define MAX_NUM_VARS 100
#define MAX_VAR_LENGTH 100
#define MAX_VAL_LENGTH 100
#define MAX_ALIAS_LENGTH 100

#define EXIT_SHELL 5

typedef struct {
  char* cmdname;
  int (*cmdfunc)(int, char*[]);
} bient;

extern const bient  bitab[];
extern int ncmds;

char variables[MAX_NUM_VARS][MAX_VAR_LENGTH]; // Holds the variables
char values [MAX_NUM_VARS][MAX_VAL_LENGTH];  // Holds the values
int disabled [MAX_NUM_VARS]; // Holds whether or not variable has been unset (1 if has been unset)
char alias_names[MAX_ALIAS_LENGTH][MAX_ALIAS_LENGTH]; // Holds the alias names
char alias_vals[MAX_ALIAS_LENGTH][MAX_ALIAS_LENGTH]; // Holds the alias values
int alias_disabled [MAX_ALIAS_LENGTH]; // Holds whether or not variable has been unset (1 if has been unset)


extern int x_chdir(int, char *[]);
extern int x_setenv(int, char *[]);
extern int x_unsetenv(int, char *[]);
extern int x_printenv(int, char *[]);
extern int x_alias(int, char *[]);
extern int x_unalias(int, char *[]);
extern int x_printalias(int, char *[]);
extern int x_bye(int, char *[]);
