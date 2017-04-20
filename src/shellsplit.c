#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "parse.h"

int main() {

  size_t bufferSize = 1024;
  char input[bufferSize + 1];

  fgets(input, bufferSize, stdin);

  token_list tokens = tokenize(input);

  size_t idx = 0;


  while(idx < tokens.noTokens) {
    command_t *command = parseCommand(tokens, &idx);

    printf("Run program \"%s\"", command->program);

    if(command->arguments.noTokens > 0) {
      printf(" with argument");
      if(command->arguments.noTokens > 1) {
        putchar('s');
      }
      printf(" \"%s\"", command->arguments.tokens[0]);

      for(size_t i = 1; i < command->arguments.noTokens; ++i) {
        printf(" and \"%s\"", command->arguments.tokens[i]);
      }
    }
    putchar('.');

    if(command->input != NULL) {
      printf(" Read the input from file \"%s\".", command->input);
    }

    if(command->output != NULL) {
      printf(" Write the output to file \"%s\".", command->output);
    }
    putchar('\n');
    if(idx < tokens.noTokens && strcmp(";", tokens.tokens[idx]) != 0) {
      break;
    }
    else {
      idx += 1;
    }
  }

  return 0;
}
