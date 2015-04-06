#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
 
void pipeline( char * ar[], int pos, int in_fd);
void error_exit(const char*);
static int child = 0; /* whether it is a child process relative to main() */
 
int main(int argc, char * argv[]) {
  if(argc < 2){
    printf("argCount = %d", argc);
    printf("Usage: %s option (option) ...\n", argv[0]);
    exit(1);
  }
  pipeline(argv, 1, STDIN_FILENO);
  return 0;
}
 
void error_exit(const char *kom){
   perror(kom);
   (child ? _exit : exit)(EXIT_FAILURE);
}

void pipeline(char *ar[], int pos, int in_fd){
   if(ar[pos+1] == NULL){ /*last command */
     if(in_fd != STDIN_FILENO){
        if(dup2(in_fd, STDIN_FILENO) != -1)
          close(in_fd); /*successfully redirected*/
        else error_exit("dup2");
     }
     execlp(ar[pos], ar[pos], NULL);
     error_exit("execlp last");
   }
   else{
     int fd[2];
     pid_t childpid;
 
     if ((pipe(fd) == -1) || ((childpid = fork()) == -1)) {
        error_exit("Failed to setup pipeline");
     }
     if (childpid == 0){					/* child executes current command */
        child = 1;
        close(fd[0]);                                  
        if (dup2(in_fd, STDIN_FILENO) == -1)           /*read from in_fd */
           perror("Failed to redirect stdin");         
        if (dup2(fd[1], STDOUT_FILENO) == -1)			/*write to fd[1]*/
           perror("Failed to redirect stdout");
        else if ((close(fd[1]) == -1) || (close(in_fd) == - 1))
           perror("Failed to close extra pipe descriptors");
        else {
           execlp(ar[pos], ar[pos], NULL);
           error_exit("Failed to execlp");
        }
     } 
     close(fd[1]);		/* parent executes the rest of commands */
     close(in_fd);
     pipeline(ar, pos+1, fd[0]);
   }
}