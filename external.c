#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <unistd.h>

#include "external.h"

bool search_external_cmd(char **tokens, size_t n_tok) {

  pid_t forkPID, w;
  int status;

  forkPID = fork();

  if (forkPID == 0) {
    int ret = execvp(tokens[0], tokens);
    if (ret < 0)
      _exit(errno);
    else
      _exit(0);
  } else {
    do {
      w = waitpid(forkPID, &status, WUNTRACED | WCONTINUED);
      if (w == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
      }
      if (WIFEXITED(status)) {
        int exitstatus = WEXITSTATUS(status);
        if (exitstatus == 0)
          return true;
        else {
          errno = exitstatus;
          perror(tokens[0]);
          return false;
        }
      } else if (WIFSIGNALED(status)) {
        printf("killed by signal %d\n", WTERMSIG(status));
      } else if (WIFSTOPPED(status)) {
        printf("stopped by signal %d\n", WSTOPSIG(status));
      } else if (WIFCONTINUED(status)) {
        printf("continued\n");
      }
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    return true;
  }
}
