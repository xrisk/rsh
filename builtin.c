#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "ls.h"
#include "main.h"
#include "prompt.h"

extern struct state shell_state;

bool search_builtin(char **tokens, size_t ntok) {
  if (ntok == 0)
    return true;

  if (strcmp(tokens[0], "cd") == 0) {
    char *cdpath;
    if (shell_state.n_tok > 1) {
      cdpath = shell_state.tokens[1];
    } else {
      cdpath = shell_state.homedir;
    }
    if (chdir(cdpath) < 0) {
      perror("chdir");
    } else {
      get_cwd();
      // update shell vars
    }
    return true;
  } else if (strcmp(tokens[0], "pwd") == 0) {
    printf("%s\n", shell_state.cwd);
    return true;
  } else if (strcmp(tokens[0], "echo") == 0) {
    for (size_t it = 1; it < ntok; ++it)
      printf("%s ", shell_state.tokens[it]);
    printf("\n");
    return true;
  } else if (strcmp(tokens[0], "ls") == 0) {
    ls();
    return true;
  }

  return false;
}
