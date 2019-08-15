#ifndef MAIN_H
#define MAIN_H

#define MAX_PATH_LEN 4096

#include <stddef.h>

struct state {
  char cwd[MAX_PATH_LEN], hostname[MAX_PATH_LEN], username[MAX_PATH_LEN],
      shortdir[MAX_PATH_LEN], homedir[MAX_PATH_LEN];
  char **tokens;
  size_t n_subcommands, n_tok;
  char **subcommands;
};

#endif
