#include "parse.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

char *parseInputRedirect(token_list tokens, size_t *tokIdx) {
  if(strcmp(tokens.tokens[*tokIdx], "<") == 0) {
    *tokIdx += 1;
    return tokens.tokens[(*tokIdx)++];
  }
  return NULL;
}

char *parseOutputRedirect(token_list tokens, size_t *tokIdx) {
  if(strcmp(tokens.tokens[*tokIdx], ">") == 0) {
    *tokIdx += 1;
    return tokens.tokens[(*tokIdx)++];
  }
  return NULL;
}

command_t *parseCommand(token_list tokens, size_t *tokIdx) {
  command_t *command = malloc(sizeof(command_t));
  command->input = NULL;
  command->output = NULL;
  command->program = tokens.tokens[(*tokIdx)++];

  char* args[100];
  size_t argIdx = 0;

  while(*tokIdx < tokens.noTokens) {
    char * res = parseInputRedirect(tokens, tokIdx);
    if(res) {
      command->input = res;
      continue;
    }

    res = parseOutputRedirect(tokens, tokIdx);
    if(res) {
      command->output = res;
      continue;
    }

    args[argIdx++] = tokens.tokens[(*tokIdx)++];

  }

  command->arguments.tokens = malloc((argIdx + 1) * sizeof(char*));
  memcpy(command->arguments.tokens, args, argIdx * sizeof(char*));
  command->arguments.noTokens = argIdx;
  command->arguments.tokens[argIdx] = NULL;

  return command;
}

token_list tokenize(char* line) {
  bool inQuotes = false;
  size_t curIdx = 0;

  char *args[100];
  size_t argIdx = 0;

  char curSeg[1024];
  size_t segIdx = 0;

  while(line[curIdx] != '\0' && line[curIdx] != '\n') {
    while((inQuotes || line[curIdx] != ' ') && line[curIdx] != '\0' && line[curIdx] != '\n') {
      if(line[curIdx] == '"') {
        inQuotes = !inQuotes;
        curIdx += 1;
      }
      else {
        curSeg[segIdx++] = line[curIdx++];
      }
    }
    curSeg[segIdx] = '\0';
    args[argIdx] = malloc((strlen(curSeg) + 1) * sizeof(char));
    strcpy(args[argIdx], curSeg);
    segIdx = 0;
    argIdx++;
    curIdx++;
  }

  char** res = malloc((argIdx + 1) * sizeof(char*));
  memcpy(res, args, argIdx * sizeof(char*));

  res[argIdx] = NULL;

  return (token_list){res, argIdx};
}

