#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ls.h"
#include "main.h"

extern struct state shell_state;

void ls(void) {

  bool all = false, longform = false;

  // TODO: actually count dirs instead of allocating extra
  char **dirs = calloc(shell_state.n_tok, sizeof(char *));
  size_t n_dirs = 0;

  for (size_t i = 1; i < shell_state.n_tok; ++i) {
    if (shell_state.tokens[i] == NULL)
      continue;
    if (shell_state.tokens[i][0] == '-') {
      if (strcmp(shell_state.tokens[i], "-la") == 0) {
        all = true, longform = true;
      } else if (strcmp(shell_state.tokens[i], "-al") == 0) {
        all = true, longform = true;
      } else if (strcmp(shell_state.tokens[i], "-a") == 0) {
        all = true;
      } else if (strcmp(shell_state.tokens[i], "-l") == 0) {
        longform = true;
      } else {
        printf("error: unknown option: %s\n", shell_state.tokens[i]);
      }
    } else {
      dirs[n_dirs] = strdup(shell_state.tokens[i]);
      ++n_dirs;
    }
  }

  for (size_t i = 0; i < n_dirs; ++i) {
    printf("%s:\n", dirs[i]);
    DIR *d = opendir(dirs[i]);
    if (d == NULL) {
      perror("ls");
      return;
    }

    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
      if (dir->d_name[0] == '.' && !all)
        continue;
      printf("%s\n", dir->d_name);
    }
    closedir(d);

    if (i != n_dirs)
      printf("\n");
  }
}
