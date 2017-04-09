#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"

int main() {
  size_t bufferSize = 1024;
  char input[bufferSize];

  int myStdinFd = dup(0);
  int myStdoutFd = dup(1);

  FILE* myStdin = fdopen(myStdinFd, "r");
  FILE* myStdout = fdopen(myStdoutFd, "w");

  size_t noProcesses = 3;
  size_t processesRunning = 0;
  while(true) {
    char* result = fgets(input, bufferSize, myStdin);
    if(result == NULL) break;

    token_list tokens = tokenize(input);
    size_t idx = 0;
    command_t *command = parseCommand(tokens, &idx);
    if(command->input != NULL) {
      close(0);
      if(open(command->input, O_RDONLY) != 0){
        fprintf(myStdout, "Read failed: %s\n", command->input);
        continue;
      }
    }

    if(command->output != NULL) {
      close(1);
      if(open(command->output, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR) != 1) {
        fprintf(myStdout, "Write failed: %s\n", command->output);
        continue;
      }
    }

    processesRunning += 1;
    fprintf(myStdout, "Number of processes running: %d\n", processesRunning);
    fflush(myStdout);
    pid_t pid = fork();
    if(pid != 0) {
      if(processesRunning > noProcesses) {
        fprintf(myStdout, "Waiting for one of the processes!\n");
        fflush(myStdout);
        int status;
        wait(&status);
        processesRunning -= 1;
      }
      dup2(myStdinFd, 0);
      dup2(myStdoutFd, 1);
    }
    else {
      // In fork
      char* arguments[100];
      arguments[0] = command->program;
      memcpy(&arguments[1], command->arguments.tokens, (command->arguments.noTokens+1) * sizeof(char*));
      if(execv(command->program, arguments) < 0) {
        fprintf(myStdout, "Execute failed: %s\n", command->program);
        exit(1);
      }
    }

  }

  int status;
  while(wait(&status) != -1) {
    fprintf(myStdout, "Collecting zombie ");
  }

  return 0;
}
