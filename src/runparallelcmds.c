#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"


int main(int argc, char* argv[]) {


  size_t bufferSize = 1024;
  char input[bufferSize];

  int myStdinFd = dup(0);
  int myStdoutFd = dup(1);

  FILE* myStdin = fdopen(myStdinFd, "r");
  FILE* myStdout = fdopen(myStdoutFd, "w");

  size_t noProcesses = 1;
  size_t processesRunning = 0;


  if(argc == 3 && strcmp(argv[1], "-j") == 0) {
    noProcesses = sscanf(argv[2], "%zu", &noProcesses);
  }

  char* tempFileRoot = "temp";
  size_t tempFileCount = 0;

  while(true) {
    char* result = fgets(input, bufferSize, myStdin);
    if(result == NULL) break;

    token_list tokens = tokenize(input);
    size_t idx = 0;
    while(idx < tokens.noTokens) {
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
          dup2(myStdinFd, 0);
          fprintf(myStdout, "Write failed: %s\n", command->output);
          fflush(myStdout);
          continue;
        }
      }
      else {
        close(1);
        char name[100];
        tempFileCount+=1;
        snprintf(name, 100, "%s%zu",tempFileRoot, tempFileCount);
        int tempFd = open(name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if( tempFd != 1) {
          fprintf(myStdout, "Temp write failed: %s With code: %d", name, tempFd);
          fflush(myStdout);
          continue;
        }
      }

      processesRunning += 1;
      pid_t pid = fork();
      if(pid != 0) {

        if(processesRunning > noProcesses + 1) {
          // We deplenished our process quota; waiting for one of the other ones to finish
          int status;
          wait(&status);
          processesRunning -= 1;
        }
        // Restore standard I/O
        dup2(myStdinFd, 0);
        dup2(myStdoutFd, 1);
      }
      else {
        // In fork
        char* arguments[100];
        arguments[0] = command->program;
        memcpy(&arguments[1], command->arguments.tokens, (command->arguments.noTokens+1) * sizeof(char*));
        if(execvp(command->program, arguments) < 0) {
          fprintf(myStdout, "Execute failed: %s\n", command->program);
          fflush(myStdout);
          exit(1);
        }
      }

      if(idx < tokens.noTokens && strcmp(";",tokens.tokens[idx]) != 0) {
        break;
      }
      else {
        idx += 1;
      }
    }

  }

  int status;

  // Gather up the zombies.
  while(wait(&status) != -1);

  // Clean up files
  for(int i = 1; i <= tempFileCount; ++i) {

    char name[100];
    snprintf(name, 100, "%s%zu",tempFileRoot, i);

    int tFd = open(name, O_RDONLY);

    int readBytes;
    char buf[1000];
    while((readBytes = read(tFd, buf, 1000)) != 0) {
      write(myStdoutFd, buf, readBytes);
    }
    close(tFd);

    remove(name);
  }

  return 0;
}
