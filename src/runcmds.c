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
        fflush(myStdout);
        continue;
      }
    }

    if(command->output != NULL) {
      close(1);
      if(open(command->output, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR) != 1) {
        fprintf(myStdout, "Write failed: %s\n", command->output);
        fflush(myStdout);
        continue;
      }
    }

    pid_t pid = fork();
    if(pid != 0) {
      int status;
      wait(&status);
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

  return 0;
}
