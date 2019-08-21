#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "parse.h"

extern struct state shell_state;

void split_into_subcommands(char *line) {

  char *copy = strdup(line);
  char *ptr = strtok(copy, ";");

  size_t ntokens = 0;
  while (ptr != NULL) {
    ++ntokens;
    ptr = strtok(NULL, ";");
  }

  strcpy(copy, line);
  ptr = strtok(copy, ";");

  shell_state.n_subcommands = ntokens;
  shell_state.subcommands = malloc(sizeof(char *) * ntokens);

  size_t index = 0;
  while (ptr != NULL) {
    shell_state.subcommands[index] = calloc(strlen(ptr) + 1, sizeof(char));
    strcpy(shell_state.subcommands[index], ptr);
    ptr = strtok(NULL, ";");
    index++;
  }

  if (copy != NULL) {
    free(copy);
    copy = NULL;
  }
}

void parse_subcommand(char *subcommand) {

  shell_state.bg = false;

  long len = strlen(subcommand);
  for (long i = len - 1; i >= 0; --i) {
    char c = subcommand[i];
    if (c == '&') {
      shell_state.bg = true;
      break;
    } else if (!(c == '\t' || c == ' ')) {
      break;
    }
  }

  char *copy = strdup(subcommand);
  char *ptr = strtok(copy, " \t");

  size_t ntokens = 0;
  while (ptr != NULL) {
    if (strcmp(ptr, "&") != 0) {
      ++ntokens;
    }
    ptr = strtok(NULL, " \t");
  }

  strcpy(copy, subcommand);
  ptr = strtok(copy, " \t");

  shell_state.n_tok = ntokens;
  shell_state.tokens = calloc(ntokens + 1, sizeof(char *)); // 1 null ptr

  size_t index = 0;
  while (ptr != NULL) {

    if (strcmp(ptr, "&") == 0) {
      ptr = strtok(NULL, " \t");
      continue;
    }
    shell_state.tokens[index] = calloc(strlen(ptr) + 1, sizeof(char));
    strcpy(shell_state.tokens[index], ptr);
    size_t len = strlen(shell_state.tokens[index]);
    if (shell_state.tokens[index][len - 1] == '&') {
      shell_state.tokens[index][len - 1] = '\0';
    }
    ptr = strtok(NULL, " \t");
    index++;
  }

  if (copy != NULL) {
    free(copy);
    copy = NULL;
  }
}
