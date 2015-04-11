#ifndef _SHELLPARSER_H_
#define _SHELLPARSER_H_

/* The shell parser should fill a tree as it parses, then walk through and
 * call the necessary functions. There should also be functions for
 * freeing memory. The tree will have different types of nodes:
 * CommandNode: a complete command that should be executed
 * PipeNode: two commands should be piped before being executed
 * ParamNode: a node with a single param, the last child of the params
 * ParamsNode: a node with two params
 */

#define OK 0
#define ERRORS 1

typedef struct Node{
  enum { command_node, pipe_node, redir_node, param_node, params_node } label;
  union { struct { char*     command;
                   struct Node*     childparams; }  CommandNode;
          struct { struct Node*     command;
                   struct Node*     pipe; }         PipeNode;
		    struct { char * infile;
              char* outfile;
              int append;
              char* stderrfile;
              int stderr_to_out;
			        struct Node* commandline; }   RedirNode;
          struct { char*     param; }        ParamNode;
          struct { struct Node*     first;
                   struct Node*     second; }       ParamsNode;
      } type;
} Node;

Node *RootNode;
int runBG;
int doneParsing;
int firstWord;
int inputlineno;

Node *new_command(char* command, Node *childparams);
Node *new_pipe(Node *command, Node *pipe);
Node *new_param(char *param);
Node *new_params(Node *first, Node *second);
Node *new_redir(Node *commandline, char* infile, char* outfile, int append, char* stderrfile, int stderr_to_out);

void freeNode(Node *np);

int printNode(Node *np);
int evalNode(Node *np);
int evalPipe(Node *np, int in_fd, pid_t pidToWait);
int evalCommand(Node *np);
int createProcCommand(Node *np);
int evalBuiltin(Node *np, int binum, int forked);

void displayPrompt(void);
void initialize(void);
int main(void);

#endif
