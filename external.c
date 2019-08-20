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

bool do_background_command(char **tokens) {

  pid_t forkPID;
  fprintf(stderr, "executing background\n");

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
    signal(SIGCHLD, SIG_DFL);

    int ret = execvp(tokens[0], tokens);
    if (ret < 0)
      _exit(errno);
    else
      _exit(0);
  } else {

    setpgid(forkPID, forkPID);
    return true;
  }
}

bool search_external_cmd(char **tokens, bool bg) {

  if (bg)
    return do_background_command(tokens);

  pid_t forkPID, w;
  int status;

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
    signal(SIGCHLD, SIG_DFL);

    printf("executing %s\n", tokens[0]);

    int ret = execvp(tokens[0], tokens);
    if (ret < 0) {
      perror("execvp");
      _exit(0);
    } else
      _exit(0);
  } else {

    setpgid(forkPID, forkPID);
    tcsetpgrp(shell_state.shell_terminal, forkPID);

    w = waitpid(forkPID, &status, 0);

    if (w == -1) {
      fprintf(stderr, "Line %d: ", __LINE__);
      perror("waitpid");
      exit(EXIT_FAILURE);
    }
    if (WIFEXITED(status)) {
      int exitstatus = WEXITSTATUS(status);
      fprintf(stderr, "pid %d exited with status %d\n", forkPID, exitstatus);
      tcsetpgrp(shell_state.shell_terminal, getpid());
      return true;
    } else if (WIFSIGNALED(status)) {
      fprintf(stderr, "pid %d killed by signal %d\n", forkPID,
              WTERMSIG(status));
      tcsetpgrp(shell_state.shell_terminal, getpid());
      return true;
    }

    return true;
  }
}
