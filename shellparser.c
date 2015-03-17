#include <stdio.h>
#include <stdlib.h>
#include "shellparser.h"
#include "y.tab.h"

#define MAX_LENGTH 1024

extern int yyparse();
extern void yy_scan_string(const char *str);

Node *_new_node(NodeEnum type) {
  Node *np = (Node *)malloc(sizeof(Node));
  np->type = type;
  return np;
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
  np->pipe = node;
  return np;
}

Node *new_param(char* param) {
  Node *np = _new_node(ParamType);
  ParamNode node;
  node.param = param;
  np->param = node;
  return np;
}

Node *new_params(Node *first, Node *second) {
  Node *np = _new_node(ParamsType);
  ParamsNode node;
  node.first = first;
  node.second = second;
  np->params = node;
  return np;
}

void freeNode(Node *np){
  if (np == NULL) {
    return;
  } else {
    switch (np->type) {
      case CommandType:
        free((np->command).command);
        freeNode((np->command).childparams);
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

int printNode(Node *np) {
  
  if (np == NULL) {
    return 0;
  } else {
    int ret = 0;
    switch (np->type) {
      case CommandType:
        printf("%s ", (np->command).command);
        if (printNode((np->command).childparams) < 0) {
          ret = -1;
          break;
        }
        ret = 0;
        break;
      case PipeType:
        if (printNode((np->pipe).command) < 0) {
          ret = -1;
          break;
        }
        if ((np->pipe).pipe != NULL) {
          printf(" | ");
          if (printNode((np->pipe).pipe) < 0) {
            ret = -1;
            break;
          }
        }
        ret = 0;
        break;
      case ParamType:
        printf("%s ", (np->param).param);
        ret = 0;
        break;
      case ParamsType:
        if (printNode((np->params).first) < 0) {
          ret = -1;
          break;
        }
        if (printNode((np->params).second) < 0) {
          ret = -1;
          break;
        }
        ret = 0;
        break;
      default:
        fprintf(stderr, "Error: cannot print node of invalid type");
        ret = -1;
        break;
    }
    return ret;
  }
}
/*
int main() {
  char line[MAX_LENGTH+1];
  int err = 0;
  while(err==0) {
    printf("HEY: ");
    if (fgets(line, MAX_LENGTH, stdin) != NULL) {
      yy_scan_string(line);
      err  = yyparse();
    } else {
      continue;
    }   
  }
  return err;
}
*/
