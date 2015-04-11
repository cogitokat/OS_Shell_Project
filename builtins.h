#define MAX_ALIAS 100

typedef struct {
  char* cmdname;
  int (*cmdfunc)(int, char*[]);
} bient;

extern const bient  bitab[];
extern int ncmds;


typedef struct AliasEntry {
  char name[MAX_ALIAS];
  char value[MAX_ALIAS];
  struct AliasEntry *next;
} AliasEntry;

AliasEntry *rootAlias;

extern int x_chdir(int, char *[]);
extern int x_setenv(int, char *[]);
extern int x_unsetenv(int, char *[]);
extern int x_printenv(int, char *[]);
extern int x_alias(int, char *[]);
extern int x_unalias(int, char *[]);
extern int x_printalias(int, char *[]);
extern int x_bye(int, char *[]);

extern char* getAlias(char *inputstr);
char lastExpandedAlias[MAX_ALIAS];

