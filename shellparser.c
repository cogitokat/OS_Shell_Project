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
  Node *np = _new_node(CommandType);
  CommandNode node;
  node.command = command;
  node.childparams = childparams;
  np->command = node;
  return np;
}

