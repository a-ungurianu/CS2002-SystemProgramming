#include "parse.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const char* COMMAND_TERMINATORS[5] = {";", "&&", "&" , "||", "|"};
static const char* INPUT_REDIRECT = "<";
static const char* OUTPUT_REDIRECT = ">";
static const char* ARG_SEPARATORS = " \t";
static const char* INPUT_TERMINATORS = "\n\0";

char *parseInputRedirect(token_list tokens, size_t *tokIdx) {
  if(strcmp(tokens.tokens[*tokIdx], INPUT_REDIRECT) == 0) {
    *tokIdx += 1;
    return tokens.tokens[(*tokIdx)++];
  }
  return NULL;
}

char *parseOutputRedirect(token_list tokens, size_t *tokIdx) {
  if(strcmp(tokens.tokens[*tokIdx], OUTPUT_REDIRECT) == 0) {
    *tokIdx += 1;
    return tokens.tokens[(*tokIdx)++];
  }
  return NULL;
}

/*
  Is this a token that terminates a command
 */
bool isTerminatorToken(char* token) {
  for(size_t i = 0; i < 5; ++i) {
    if(strcmp(COMMAND_TERMINATORS[i], token) == 0) return true;
  }
  return false;
}


/*
  Parse the first command from the given list of tokens
 */
command_t *parseCommand(token_list tokens, size_t *tokIdx) {
  command_t *command = malloc(sizeof(command_t));
  command->input = NULL;
  command->output = NULL;
  command->program = tokens.tokens[(*tokIdx)++];

  char* args[100];
  size_t argIdx = 0;

  while(*tokIdx < tokens.noTokens && !isTerminatorToken(tokens.tokens[*tokIdx])) {
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

/*
  Skip allowed whitespace between command arguments.
 */
void skipWhitespace(char* line, size_t *curIdx) {
  while(!strchr(INPUT_TERMINATORS, line[*curIdx]) && strchr(ARG_SEPARATORS, line[*curIdx]))
    *curIdx += 1;
}


/*
  Tokenize the input into a list of words that can be parsed by the parsing functions.
 */
token_list tokenize(char* line) {
  bool inQuotes = false;
  size_t curIdx = 0;

  char *args[100];
  size_t argIdx = 0;

  char curSeg[1024];
  size_t segIdx = 0;

  while(!strchr(INPUT_TERMINATORS, line[curIdx])) {
    while((inQuotes || !strchr(ARG_SEPARATORS, line[curIdx])) && !strchr(INPUT_TERMINATORS, line[curIdx])) {
      if(!inQuotes) {
        for(size_t i = 0; i < 5; ++i) {
          if(strcmp(strndup(&line[curIdx], strlen(COMMAND_TERMINATORS[i])),COMMAND_TERMINATORS[i]) == 0) {
            if(segIdx != 0) {
              curSeg[segIdx] = '\0';
              args[argIdx] = strdup(curSeg);
            }
            args[argIdx++] = strdup(COMMAND_TERMINATORS[i]);
            curIdx += strlen(COMMAND_TERMINATORS[i]);
            segIdx = 0;
            skipWhitespace(line, &curIdx);
          }
        }
      }

      if(line[curIdx] == '"') {
        inQuotes = !inQuotes;
        curIdx += 1;
      }
      else {
        curSeg[segIdx++] = line[curIdx++];
      }
    }
    if(segIdx != 0) {
      curSeg[segIdx] = '\0';
      args[argIdx] = malloc((strlen(curSeg) + 1) * sizeof(char));
      strcpy(args[argIdx], curSeg);
      segIdx = 0;
      argIdx++;
    }
    curIdx++;
  }

  // Move the parsed tokens from a static array to a heap allocated array
  char** res = malloc((argIdx + 1) * sizeof(char*));
  memcpy(res, args, argIdx * sizeof(char*));

  res[argIdx] = NULL;

  return (token_list){res, argIdx};
}
