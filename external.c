#include <stdbool.h>
#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "external.h"
#include "main.h"

extern struct state shell_state;

int do_background_command(char **tokens) {

  pid_t forkPID;

#if DEBUG
  fprintf(stderr, "executing background\n");
#endif

  if ((forkPID = fork()) < 0) {
    perror("failed to fork");
    return false;
  } else if (forkPID == 0) {

    pid_t myPID = getpid();
    setpgid(myPID, myPID);

    /* https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html#Initializing-the-Shell
     */

    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
    execvp(tokens[0], tokens);
    perror("execvp");

    return QUIT_NOW;
  } else {
    setpgid(forkPID, forkPID);
    return true;
  }
}

int search_external_cmd(char **tokens, bool bg) {

  if (bg)
    return do_background_command(tokens);

  pid_t forkPID;

  forkPID = fork();

  if (forkPID == 0) {

    pid_t myPID = getpid();
    setpgid(myPID, myPID);
    tcsetpgrp(shell_state.shell_terminal, myPID);

    /*https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html#Initializing-the-Shell*/

    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

#if DEBUG
    printf("executing %s\n", tokens[0]);
#endif

    execvp(tokens[0], tokens);
    perror("execvp");
    return QUIT_NOW;

  } else {
    setpgid(forkPID, forkPID);
    shell_state.fg_pid = forkPID;

    if (!bg) {
      shell_state.shell_pgid = tcsetpgrp(shell_state.shell_terminal, forkPID);
    }

    while (shell_state.fg_pid != -1) {
    }

    tcsetpgrp(shell_state.shell_terminal, getpid());
  }

  return true;
}
