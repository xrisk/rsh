#ifndef MAIN_H
#define MAIN_H

#define MAX_PATH_LEN 4096

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <termios.h>

struct state {
  char cwd[MAX_PATH_LEN], hostname[MAX_PATH_LEN], username[MAX_PATH_LEN],
      shortdir[MAX_PATH_LEN], homedir[MAX_PATH_LEN];
  char **tokens;
  bool bg;
  size_t n_subcommands, n_tok;
  char **subcommands;
  int shell_terminal;
  int shell_is_interactive;
  pid_t shell_pgid;

  struct termios shell_tmodes;
};

#endif
