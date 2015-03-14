#include <stdio.h>
#include <stdlib.h>

Node *_new_node(NodeEnum type) {
  Node *np = (Node *)malloc(sizeof(Node));
  np->type = type;
  return np
}

Node *new_command(char* command, Node *childparams) {
  Node *np = _new_node(CommandType);
  CommandNode node;
  node.command = command;
  node.childparams = childparams;
  np->command = node;
  return np;
}

Node *new_pipe(Node *command, Node *pipe) {
  Node *np = _new_node(PipeType);
  PipeNode node;
  node.command = command;
  node.pipe = pipe;
  np->command = node;
  return np;
}

Node *new_param(char* param) {
  Node *np = _new_node(ParamType);
  ParamNode node;
  node.param = param;
  np->command = node;
  return np;
}

Node *new_params(Node *first, Node *second) {
void freeNode(Node *np){
  if (np == NULL) {
    return;
  } else {
    switch (np->type) {
      case CommandType:
        free((np->command).command);
        freeNode((np->command).allparams);
        break;
      case PipeType:
        freeNode((np->pipe).command);
        freeNode((np->pipe).pipe);
        break;
      case ParamsType:
        freeNode((np->params).first);
        freeNode((np->params).second);
        break;
      case ParamType:
        free((np->param).param);
        break;
      default:
        fprintf(stderr, "Error in freeNode(): Invalid Node Type");
        exit(-1);
    }   
    free(np);
  }
}

  Node *np = _new_node(CommandType);
  CommandNode node;
  node.command = command;
  node.childparams = childparams;
  np->command = node;
  return np;
}


