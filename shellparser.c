#include <stdio.h>
#include <stdlib.h>
#include "shellparser.h"
#include "y.tab.h"

// Create a new node. This allocates the memory for a node
// and gives it a type.
Node *_new_node(NodeEnum type) {
  Node *np = (Node *)malloc(sizeof(Node));
  np->type = type;
  return np;
}

// Functions for creating different types of nodes and
// adding values and node pointers to them.
// A CommandNode, PipeNode, ParamNode or ParamsNode is
// inserted into a Node.
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

// Recursive function to free nodes...
void freeNode(Node *np){
  if (np == NULL) { // Return if there are no nodes left.
    return;
  } else {
    switch (np->type) {
      // For each case, based on the node type,
      // free the value, or free the nodes it 
      // points to recursively.
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
    free(np); // Free the np.
  }
}

// Recursive function to print node values. We only need this
// for debugging. Same general idea as freeNode() above,
// except returning error values.
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

// This is what will be called for each line of input. Determine the
// type of node and pass it to a function for further evaluation.
int evalNode(Node *np) {  
  if (np == NULL) {
    return 0;
  } else {
    int ret = 0;
    switch (np->type) {
      case CommandType:
        evalCommand(np);
        break;
      case PipeType:
        evalPipe(np);
        break;
      default:
        fprintf(stderr, "Error: cannot print node of invalid type");
        ret = -1;
        break;
    }
    return ret;
  }
}

// Recursively count the number of params. The params node is a
// right-skewed binary tree, so we only need to follow the
// second node pointers.
int countArgs(Node *paramsptr) {
  int count = 0;
  if (paramsptr != NULL) {
    count = 1 + countArgs(((paramsptr)->params).second);
  }
  return count;
}

// Evaluate a single command.
void evalCommand(Node *np) 
{
  printf("Evaluating command\n"); // Debug statement.
  int process;
  process = fork(); // Create a new process for the command.
  if (process > 0) { // If the process is > 0, we are still the parent,
    wait((int *) 0); // so wait. (Null pointer - return value not saved.)
  }
  else if (process == 0) { // If process == 0, we are in the child...
    char *command = (np->command).command; // Get the command name string.
    Node *childparams = (np->command).childparams; // Get the childparams node.
    int numparams = countArgs(childparams); // Count the number of params.
    char **paramslist = (char**)malloc((numparams+2) * sizeof(char*)); // Pointer to pointer to an array
    int i = 0;
    paramslist[0] = command; // The first param in the array is always the command name.
    paramslist[numparams+1] = NULL; // The last thing in the array must be null.
    Node *curr = childparams; // A node pointer that will indicate the current node.
    for (i = 0; i < numparams; i++) {
      // Basically, add the name of the param that is in the first node.
      // (Remember, the params are a right-skewed binary tree.)
      paramslist[i+1] = (((((np)->params).first)->param).param);
      ((np)->params).second; // Make curr point to the second node.
    }
    if (execvp(command, paramslist) == -1) { // Execute the command with execvp().
      fprintf(stderr, "Can't execute %s\n.", command); // Tell us if there's an error.
      free(paramslist); // Always clean up.
      exit(1); // Exit with a non-zero status.
    } else {
      free(paramslist); // Always clean up.
      exit(0); // Exit with a zero status (no problems).
    }
  }
  else if (process == -1) { // If process == -1, then something went wrong.
    fprintf(stderr, "Can't fork!"); // Error message.
    exit(2); // Exit with a non-zero status.
  }
}

// Here we will evaluate multiple commands that are 
// piped together.
void evalPipe(Node *np) 
{
  printf("Evaluating pipe\n");

}
