#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  char *program;
  char* *arguments;
  char *output;
  char *input;
} command_t;

char** split_line(char* line) {
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

  return res;
}


int main() {

  size_t bufferSize = 1024;
  char input[bufferSize + 1];

  fgets(input, bufferSize, stdin);

  char **segments = split_line(input);

  size_t segIdx = 0;

  while(segments[segIdx] != NULL) {
    printf("%s\n", segments[segIdx]);
    segIdx++;
  }

  return 0;
}
