#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>

typedef struct {
  char **tokens;
  size_t noTokens;
} token_list;

typedef struct {
  char *program;
  token_list arguments;
  char *output;
  char *input;
} command_t;


token_list tokenize(char* line);
command_t *parseCommand(token_list tokens, size_t *tokIdx);

#endif
