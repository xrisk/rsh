#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "history.h"
#include "ls.h"
#include "main.h"
#include "nightswatch.h"
#include "pinfo.h"
#include "prompt.h"

extern struct state shell_state;

int search_builtin(process *proc) {

  char **tokens = proc->argv;
  size_t ntok = proc->n_tokens;
  if (ntok == 0)
    return true;

  if (strcmp(tokens[0], "cd") == 0) {
    char *cdpath;
    if (proc->n_tokens > 1) {
      cdpath = tokens[1];
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
      printf("%s ", tokens[it]);
    printf("\n");
    return true;
  } else if (strcmp(tokens[0], "ls") == 0) {
    ls(proc);
    return true;
  } else if (strcmp(tokens[0], "pinfo") == 0) {
    pinfo(proc);
    return true;
  } else if (strcmp(tokens[0], "nightswatch") == 0) {
    nightswatch();
    return true;
  } else if (strcmp(tokens[0], "dirty") == 0) {
    dirty();
    return true;
  } else if (strcmp(tokens[0], "interrupt") == 0) {
    interrupt();
    return true;
  } else if (strcmp(tokens[0], "history") == 0) {
    show_history();
    return true;
  } else if (strcmp(tokens[0], "exit") == 0) {
    exit(0);
  }

  return false;
}
