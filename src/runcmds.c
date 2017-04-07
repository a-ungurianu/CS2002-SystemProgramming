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
  char input[1025];

  while(true) {
    char* result = fgets(input, bufferSize, stdin);
    if(result == NULL) break;

    printf("Command: %s", input);
    token_list tokens = tokenize(input);
    size_t idx = 0;
    command_t *command = parseCommand(tokens, &idx);

    int stdinFDCopy = dup(0);
    int stdoutFDCopy = dup(1);

    if(command->input != NULL) {
      close(0);
      open(command->input, O_RDONLY);
    }

    if(command->output != NULL) {
      close(1);
      open(command->output, O_WRONLY);
    }

    pid_t pid = fork();
    if(pid != 0) {
      int status;
      wait(&status);
      dup2(stdinFDCopy, 0);
      dup2(stdoutFDCopy, 1);
    }
    else {
      // In fork
      char* arguments[100];
      arguments[0] = command->program;
      memcpy(&arguments[1], command->arguments.tokens, (command->arguments.noTokens+1) * sizeof(char*));
      if(execv(command->program, arguments) < 0) {
        printf("Execute failed: %s\n", command->program);
      }
    }

  }

  return 0;
}
