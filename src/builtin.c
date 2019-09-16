#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "env.h"
#include "external.h"
#include "history.h"
#include "kill.h"
#include "ls.h"
#include "main.h"
#include "nightswatch.h"
#include "pinfo.h"
#include "prompt.h"
#include "util.h"

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
  } else if (strcmp(tokens[0], "pwd") == 0) {
    printf("%s\n", shell_state.cwd);
  } else if (strcmp(tokens[0], "echo") == 0) {
    for (size_t it = 1; it < ntok; ++it)
      printf("%s ", tokens[it]);
    printf("\n");
  } else if (strcmp(tokens[0], "ls") == 0) {
    ls(proc);
  } else if (strcmp(tokens[0], "pinfo") == 0) {
    pinfo(proc);
  } else if (strcmp(tokens[0], "nightswatch") == 0) {
    nightswatch(proc);
  } else if (strcmp(tokens[0], "dirty") == 0) {
    dirty();
  } else if (strcmp(tokens[0], "interrupts") == 0) {
    interrupt(0);
  } else if (strcmp(tokens[0], "history") == 0) {
    show_history(proc);
  } else if (strcmp(tokens[0], "quit") == 0 || strcmp(tokens[0], "exit") == 0) {
    exit(0);
  } else if (strcmp(tokens[0], "jobs") == 0) {
    print_job_table();
  } else if (strcmp(tokens[0], "setenv") == 0) {
    set_environ(proc);
  } else if (strcmp(tokens[0], "unsetenv") == 0) {
    unset_environ(proc);
  } else if (strcmp(tokens[0], "fg") == 0) {
    foreground(proc);
  } else if (strcmp(tokens[0], "bg") == 0) {
    background(proc);
  } else if (strcmp(tokens[0], "kjob") == 0) {
    kjob(proc);
  } else if (strcmp(tokens[0], "overkill") == 0) {
    overkill(proc);
  } else
    return false;

  return true;
}
