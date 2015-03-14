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

typedef struct {
  char *command;
  struct NodeLabel *childparams;
} CommandNode;

typedef struct {
  struct NodeLabel *command;
  struct NodeLabel *pipe;
} PipeNode;

typedef struct {
  char *param;
} ParamNode;

typedef struct {
  struct NodeLabel *first;
  struct NodeLabel *second;
} ParamsNode;

typedef enum { 
  CommandType, PipeType, RedirType, ParamType, ParamsType 
} NodeEnum;

typedef struct NodeLabel{
  NodeEnum type;
  union {
    CommandNode command;
    PipeNode pipe;
    ParamsNode params;
    ParamNode param;
  };
} Node;

Node *new_command(char* command, Node *childparams);
Node *new_pipe(Node *command, Node *pipe);
Node *new_param(char *param);
Node *new_params(Node *first, Node *second);

void freeNode(Node *np);

#endif
