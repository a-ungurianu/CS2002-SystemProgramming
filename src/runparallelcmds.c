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


/*
  Redirect standard output to the file with the given name
 */
bool redirectStdoutToFile(char *fileName) {
  close(1);
  if(open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR) != 1) {
    return false;
  }
  return true;
}

/*
  Redirect standard input to the file with the given name
 */
bool redirectStdinFromFile(char *fileName) {
  close(0);
  if(open(fileName, O_RDONLY) != 0) {
    return false;
  }
  return true;
}

/*
  Execute the given command, printing error messages on failure.
 */
void execCommand(command_t *command) {
  char* arguments[100];
  arguments[0] = command->program;
  memcpy(&arguments[1], command->arguments.tokens, (command->arguments.noTokens+1) * sizeof(char*));
  if(execvp(command->program, arguments) < 0) {
    fprintf(stderr, "Execute failed: %s\n", command->program);
    fflush(stderr);
    exit(1);
  }
}

int main(int argc, char* argv[]) {
  size_t bufferSize = 1024;
  char input[bufferSize];

  // Saved standard input and standard output
  int myStdinFd = dup(0);
  int myStdoutFd = dup(1);

  // Create file handlers from the saved file descriptors
  // This allows us to use the I/O functions defined in stdio
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
        if(!redirectStdinFromFile(command->input)){
          dup2(myStdinFd, 0);
          fprintf(stderr, "Read failed: %s\n", command->input);
          fflush(stderr);
          continue;
        }
      }

      if(command->output != NULL) {
        if(!redirectStdoutToFile(command->output)) {
          dup2(myStdoutFd, 1);
          fprintf(stderr, "Write failed: %s\n", command->output);
          fflush(stderr);
          continue;
        }
      }
      else {
        char name[100];
        tempFileCount+=1;
        snprintf(name, 100, "%s%zu",tempFileRoot, tempFileCount);
        if(!redirectStdoutToFile(name)) {
          dup2(myStdoutFd, 1);
          fprintf(stderr, "Temp write failed: %s\n", name);
          fflush(stderr);
          continue;
        }
      }

      processesRunning += 1;
      pid_t pid = fork();
      if(pid != 0) {
        // In parent
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
        execCommand(command);
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
  for(size_t i = 1; i <= tempFileCount; ++i) {

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
