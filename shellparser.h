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

typedef struct Node{
  enum { command_node, pipe_node, param_node, params_node } label;
  union { struct { char*     command;
                   struct Node*     childparams; }  CommandNode;
          struct { struct Node*     command;
                   struct Node*     pipe; }         PipeNode;
          struct { char*     param; }        ParamNode;
          struct { struct Node*     first;
                   struct Node*     second; }       ParamsNode;
      } type;
} Node;

Node *new_command(char* command, Node *childparams);
Node *new_pipe(Node *command, Node *pipe);
Node *new_param(char *param);
Node *new_params(Node *first, Node *second);

void freeNode(Node *np);

int printNode(Node *np);
int evalNode(Node *np);
void evalPipe(Node *np, int in_fd, int first);
int evalCommand(Node *np);
void createProcCommand(Node *np);

#endif
