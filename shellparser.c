#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shellparser.h"
#include "builtins.h"
#include "y.tab.h"

static void error_exit(const char *msg);

// Functions for creating different types of nodes and
// adding values and node pointers to them.
// A CommandNode, PipeNode, ParamNode or ParamsNode is
// inserted into a Node.

Node *new_command(char* command, Node *childparams) {
  Node* newnode = (Node*) malloc(sizeof(Node));
  newnode->label = command_node;
  newnode->type.CommandNode.command = command;
  newnode->type.CommandNode.childparams = childparams;
  return newnode;
}

Node *new_pipe(Node *command, Node *pipe) {
  Node* newnode = (Node*) malloc(sizeof(Node));
  newnode->label = pipe_node;
  newnode->type.PipeNode.command = command;
  newnode->type.PipeNode.pipe = pipe;
  return newnode;
}

Node *new_param(char* param) {
  Node* newnode = (Node*) malloc(sizeof(Node));
  newnode->label = param_node;
  newnode->type.ParamNode.param = param;
  return newnode;
}

Node *new_params(Node *first, Node *second) {
  Node* newnode = (Node*) malloc(sizeof(Node));
  newnode->label = params_node;
  newnode->type.ParamsNode.first = first;
  newnode->type.ParamsNode.second = second;
  return newnode; 
}

// Recursive function to free nodes...
void freeNode(Node *np){
  if (np == NULL) { // Return if there are no nodes left.
    return;
  } else {
    switch (np->label) {
      // For each case, based on the node tag,
      // free the value, or free the nodes it 
      // points to recursively.
      case command_node:
        free(np->type.CommandNode.command);
        freeNode(np->type.CommandNode.childparams);
        break;
      case pipe_node:
        freeNode(np->type.PipeNode.command);
        freeNode(np->type.PipeNode.pipe);
        break;
      case params_node:
        freeNode(np->type.ParamsNode.first);
        freeNode(np->type.ParamsNode.second);
        break;
      case param_node:
        free(np->type.ParamNode.param);
        break;
      default:
        error_exit("Error in freeNode(): Invalid Node Type.");
        
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
    switch (np->label) {
      case command_node:
        printf("%s \n", (np->type.CommandNode.command));
        if (printNode(np->type.CommandNode.childparams) < 0) {
          ret = -1;
          break;
        }
        break;
      case pipe_node:
        if (printNode(np->type.PipeNode.command) < 0) {
          ret = -1;
          break;
        }
        if (np->type.PipeNode.pipe != NULL) {
          printf(" | \n");
          if (printNode(np->type.PipeNode.pipe) < 0) {
            ret = -1;
            break;
          }
        }
        break;
      case param_node:
        printf("%s \n", (np->type.ParamNode.param));
        break;
      case params_node:
        if (printNode(np->type.ParamsNode.first) < 0) {
          ret = -1;
          break;
        }
        if (printNode(np->type.ParamsNode.second) < 0) {
          ret = -1;
          break;
        }
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
    int binum;
    switch (np->label) {
      case command_node:
        if ((binum = checkBuiltin(np)) >= 0) {
          ret = evalBuiltin(np, binum);
        } else {
          ret = createProcCommand(np);
        }
        break;
      case pipe_node:
        ret = evalPipe(np, STDIN_FILENO, 0);
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
    count = 1 + countArgs(paramsptr->type.ParamsNode.second);
  }
  return count;
}

int checkBuiltin (Node *np) {
  char *command = np->type.CommandNode.command;
  int i = 0;
  for (i = 0; i < ncmds; i++) {
    char* testcmd = bitab[i].cmdname;
    if(strcmp(testcmd, command) == 0){
      fprintf(stderr, "Found builtin!\n");
      return i;
    }
  }
  return -1;
}

// Maybe we will check for aliases here.
int checkAlias(char* command) {
  return -1;
}



// Evaluate a builtin command, no forking!
int evalBuiltin(Node *np, int binum){
  fprintf(stderr, "This is a builtin: %d in table.\n", binum); // Debugging
  int ret;
  Node *childparams = np->type.CommandNode.childparams;
  int numparams = countArgs(childparams);
  char *paramslist[numparams]; // Array of the params, no need for null entry.
  int i;
  Node *curr = childparams; // Pointer will indicate current node.
  for (i = 0; i < numparams; i++) { // Fill in paramslist.
    paramslist[i] = (curr->type.ParamsNode.first)->type.ParamNode.param;
    curr = curr->type.ParamsNode.second;
  }   
  ret = bitab[binum].cmdfunc(numparams, paramslist);
  if (ret == -1) { // Call the builtin function.
    fprintf(stderr, "Error with builtin.\n"); // If the function returns -1, error.
  }
  return ret;
}  

// Evaluate a single command.
int evalCommand(Node *np) {
  fprintf(stderr, "Evaluating a command..\n");
  int ret;
  char *command = np->type.CommandNode.command;
  Node *childparams = np->type.CommandNode.childparams; // Make a pointer to the params.
  int numparams = countArgs(childparams); // Count the number of params.
  char *paramslist[numparams+2]; // Array of the params.
  int i;
  paramslist[0] = command; // The first param in the array is always the command name.
  paramslist[numparams+1] = NULL; // The last thing in the array must be null.
  Node *curr = childparams; // A node pointer that will indicate the current node.
  for (i = 0; i < numparams; i++) {
    // Add the name of the param that is in the first node.
    // (Remember, the params are a right-skewed binary tree.)
    paramslist[i+1] = (curr->type.ParamsNode.first)->type.ParamNode.param;
    fprintf(stderr, "Param str: %s\n", (curr->type.ParamsNode.first)->type.ParamNode.param); // Debugging
    curr = curr->type.ParamsNode.second; // Make curr point to the second node.
  }
  fprintf(stderr, "Executing %s.\n", paramslist[0]);
  if (execvp(paramslist[0], paramslist) == -1) { // Execute the command with execvp().
    fprintf(stderr, "Can't execute %s.\n", paramslist[0]); // Tell us if there's an error.
    ret = -1; // Exit with a non-zero status.
  } else {
    ret = 0;; // Exit with a zero status (no problems).
  }
  return ret;
}

// Create a new process for a single command
// and pass to evalCommand() when in the child process.
int createProcCommand(Node *np) 
{
  int ret = 0;
  int process;
  process = fork(); // Create a new process for the command.
  if (process > 0) { // If the process is > 0, we are still the parent,
    wait((int *) 0); // so wait. (Null pointer - return value not saved.)
  }
  else if (process == 0) { // If process == 0, we are in the child...
    ret = evalCommand(np);
  }
  else if (process == -1) { // If process == -1, then something went wrong.
    error_exit("Can't fork!");
  }
  return ret;
}

static void error_exit(const char *msg){
  perror(msg);
  exit(EXIT_FAILURE);
}

// Here we will evaluate multiple commands that are 
// piped together.
int evalPipe(Node *np, int in_fd, pid_t pidToWait) {
  fprintf(stderr, "recursive pipe function...\n");
  // Only wait if pidToWait is > 0, meaning it is a child
  if (pidToWait>0) {
    waitpid(pidToWait, (int*)0, 0);
  }

  if (np->type.PipeNode.pipe!=NULL) {
    // ** Not the last command
    int fd[2];
    pid_t childpid;

    if ((pipe(fd) == -1) || ((childpid = fork()) == -1)) {
      error_exit("Failed to setup pipeline");
    }

    if (childpid == 0){ // child
      int binum;
      if (dup2(in_fd, STDIN_FILENO) == -1)
        error_exit("Failed to redirect stdin");
      if (dup2(fd[1], STDOUT_FILENO) == -1)
        error_exit("Failed to redirect stdout");
      if ((binum = checkBuiltin(np->type.PipeNode.command)) >= 0) {
        evalBuiltin(np->type.PipeNode.command, binum);
      } else {
        evalCommand(np->type.PipeNode.command);
      }
    }
    // In parent

    // If this is not closed, then the second command hangs
    // closing the pipe is how the process know there is no 
    // more input.
    close(fd[1]);

    // Recursion powers!
    evalPipe(np->type.PipeNode.pipe, fd[0], childpid);

  } else {
    // ** Last Command
    pid_t childpid;
    childpid = fork();

    if (childpid == 0) { // child
      int binum;
      if(in_fd != STDIN_FILENO) {
        if (dup2(in_fd, STDIN_FILENO) != -1)
          close(in_fd);
        else error_exit("Failed to redirect stdin");
        }
      if ((binum = checkBuiltin(np->type.PipeNode.command)) >= 0) {
        evalBuiltin(np->type.PipeNode.command, binum);
      } else {
        evalCommand(np->type.PipeNode.command);
      }
    }
    // in parent
    // wait for the child to die before return
    waitpid(childpid, (int*)0, 0);

    return 0;
  }
}

void displayPrompt(void) {
  if(isatty(0)) {
    fprintf(stdout, "$: ");
  }
}

void initialize(void) {
  // Initialize variable arrays
  int i;
  for (i = 0; i < MAX_NUM_VARS; i++) {
    variables[i][0] = '\0';
    values[i][0] = '\0';
    disabled[i] = 0;
  }
}

int getCommand(){
  int status = yyparse();
  fprintf(stderr, "yyparse()=%d\n", status);
  if (status == 0){
    return OK;
  } else if (status == 1){
    return ERRORS;
  }
}

int main(void) {
  initialize();
  while(1) {
    displayPrompt();
    switch (getCommand()) {
      case ERRORS:
        fprintf(stderr, "Recover from errors...\n");
        while(yylex()) 
          fprintf(stderr, "clear lex: %s\n", yylval.s);
        break;
      case OK:
        evalNode(RootNode); 
        freeNode(RootNode);
        fprintf(stderr, "Done evaling..\n");
    }
  }
}


