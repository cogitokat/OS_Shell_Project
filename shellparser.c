#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shellparser.h"
#include "builtins.h"
#include "y.tab.h"

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
        fprintf(stderr, "Error in freeNode(): Invalid Node Type.\n");
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
    switch (np->label) {
      case command_node:
        createProcCommand(np);
        break;
      case pipe_node:
        evalPipe(np, STDIN_FILENO, 1);
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

int checkBuiltin (char* command) {
  int i = 0;
  for (i = 0; i < ncmds; i++) {
    char* testcmd = bitab[i].cmdname;
    if(strcmp(testcmd, command) == 0){
      return i;
    }
  }
  return -1;
}

int checkAlias(char* command) {
  return -1;
}

// Evaluate a single command.
int evalCommand(Node *np) {
  fprintf(stderr, "Evaluating a command..\n");
  int ret;
  char *command = np->type.CommandNode.command;
  Node *childparams = np->type.CommandNode.childparams; // Make a pointer to the params.
  int numparams = countArgs(childparams); // Count the number of params.
  int binum = checkBuiltin(command); // Check if it is a builtin command.
  int alias = checkAlias(command); // Check if it is an alias.
  if(binum > -1) { // Handle builtins.
    fprintf(stderr, "This is a builtin: %d.\n", binum); // Debugging
    char *paramslist[numparams]; // Array of the params, no need for null entry.
    int i;
    Node *curr = childparams; // Pointer will indicate current node.
    for (i = 0; i < numparams; i++) { // Fill in paramslist.
      paramslist[i] = (curr->type.ParamsNode.first)->type.ParamNode.param;
      curr = curr->type.ParamsNode.second;
    }
    if(bitab[binum].cmdfunc(numparams, paramslist) == -1){ // Call the builtin function.
      fprintf(stderr, "Error with builtin.\n"); // If the function returns -1, error.
      ret = -1;
    } else {
      ret = 0;;
    }
  } else { // Handle regular commands.
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
  }
  return ret;
}

// Create a new process for a single command
// and pass to evalCommand() when in the child process.
void createProcCommand(Node *np) 
{
  printf("Evaluating command\n"); // Debug statement.
  int process;
  process = fork(); // Create a new process for the command.
  if (process > 0) { // If the process is > 0, we are still the parent,
    wait((int *) 0); // so wait. (Null pointer - return value not saved.)
  }
  else if (process == 0) { // If process == 0, we are in the child...
    evalCommand(np);
  }
  else if (process == -1) { // If process == -1, then something went wrong.
    fprintf(stderr, "Can't fork!"); // Error message.
    exit(2); // Exit with a non-zero status.
  }
}

int countCmds(Node *np) {
  int count = 0;
  if (np != NULL) {
    count = 1 + countCmds(np->type.PipeNode.pipe);
  }
  return count;
}

// Here we will evaluate multiple commands that are 
// piped together.
/*
void evalPipe(Node *np) {
  pid_t pid1;
  if ((pid1 = fork()) < 0) {
    fprintf(stderr, "fork error\n");
    exit(-1);
  } else if (pid1 == 0) { // child
    pid_t pid2;
    int fd[2];
    if (pipe(fd) < 0) {
      fprintf(stderr, "pipe failed\n");
      exit(-1);
    }
    if ((pid2 = fork()) < 0) {
      fprintf(stderr, "fork error\n");
      exit(-1);
    } else if (pid2 == 0) { // child
      close(STDOUT_FILENO);
      dup(fd[1]); // fd[1] is the write end,  call dup fd[1] to stdout
      close(fd[0]);
      close(fd[1]);
      // write to stdout
      evalCommand(np->type.PipeNode.pipe->type.PipeNode.command); // output of will directed to pipe write end
    } else { // parent
      close(STDIN_FILENO);
      dup(fd[0]); // fd[0] is the read end.
      close(fd[0]);
      close(fd[1]);
      // read from(stdin) an empty pipe blocks reader.
      evalCommand(np->type.PipeNode.command);
    }
  } else { // parent
    if (waitpid(pid1, NULL, 0) < 0) {
      fprintf(stderr, "wait error\n");
      exit(-1);
    }
  }
}*/
static int child = 0;

void error_exit(const char *kom){
  perror(kom);
  (child ? _exit : exit)(EXIT_FAILURE);
}

// If pidToWait <= 0 then do not wait
void evalPipe(Node *np, int in_fd, pid_t pidToWait) {
  if (pidToWait>0) {
    fprintf(stderr, "Waiting to eval %s   PID waiting: %d\n", np->type.PipeNode.command->type.CommandNode.command, pidToWait);
    waitpid(pidToWait, (int*)0, 0);
    fprintf(stderr, "Done waiting for PID %d\n", pidToWait);
  }

  if (np->type.PipeNode.pipe!=NULL) {
    // ** Not the last command
    int fd[2];
    pid_t childpid;

    if ((pipe(fd) == -1) || ((childpid = fork()) == -1)) {
      error_exit("Failed to setup pipeline");
    }

    if (childpid == 0){
         child = 1;
         fprintf(stderr, "Redirecting stdin to %d\n", in_fd);
         if (dup2(in_fd, STDIN_FILENO) == -1)
           error_exit("Failed to redirect stdin");
         if (dup2(fd[1], STDOUT_FILENO) == -1)
           error_exit("Failed to redirect stdout");
         //fprintf(stderr, "closing file ids: %d %d\n", fd[1], in_fd);
         //if (close(fd[1]) == -1)
         //  error_exit("Failed to close extra pipe descriptors");
         //if(close(in_fd) == -1)
         //  error_exit("Failed to close in_fd");
         //fprintf(stderr, "cmd [notlast]: %s\n", np->type.PipeNode.command->type.CommandNode.command);
         fprintf(stderr, "buffer read-point: %d   buffer write-point: %d\n", fd[0], fd[1]);
         fprintf(stderr, "recurse: %s   output fd: %d  input fd: %d\n", np->type.PipeNode.command->type.CommandNode.command, fd[1], in_fd);
         evalCommand(np->type.PipeNode.command);
    }
    
    close(fd[1]);

    // Recursive
    evalPipe(np->type.PipeNode.pipe, fd[0], childpid);

  } else {
    // ** Last Command
    pid_t childpid;
    childpid = fork();
      
    if (childpid == 0) {
      fprintf(stderr, "cmd last: %s   output fd: %d  input fd: %d\n", np->type.PipeNode.command->type.CommandNode.command, STDOUT_FILENO, in_fd);
      if(in_fd != STDIN_FILENO) {
        if (dup2(in_fd, STDIN_FILENO) != -1)
          close(in_fd);
        else error_exit("Failed to redirect stdin");
        }
      evalCommand(np->type.PipeNode.command);
    }

    waitpid(childpid, (int*)0, 0);

    return;
  }

  /*if(np->type.PipeNode.pipe == NULL) {
    printf("last cmd 1: %s\n", np->type.PipeNode.command->type.CommandNode.command);

    if(in_fd != STDIN_FILENO) {
      if(dup2(in_fd, STDIN_FILENO) != -1)
        close(in_fd);
      else error_exit("dup2");
    }

    evalCommand(np->type.PipeNode.command);
    printf("last cmd: %s\n", np->type.PipeNode.command->type.CommandNode.command);
    //error_exit("evalCommand last");
  } else {
    printf("last cmd next: %s\n", np->type.PipeNode.pipe->type.PipeNode.command->type.CommandNode.command);

    printf("childpid = %d\n", childpid);
  }A*/
}
