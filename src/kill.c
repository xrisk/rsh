#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "kill.h"
#include "main.h"
#include "util.h"

extern struct state shell_state;

void kjob(process *proc) {

  if (proc->n_tokens != 3) {
    fprintf(stderr, "usage: kjob <jobNumber> <signalNumber>\n");
    return;
  }

  int jobNumber = strtol(proc->argv[1], NULL, 10);
  if (jobNumber == 0) {
    fprintf(stderr, "invalid job number\n");
    return;
  }

  int signalNumber = strtol(proc->argv[2], NULL, 10);
  if (signalNumber == 0) {
    fprintf(stderr, "invalid signal number\n");
    return;
  }

  job_entry *j = get_job(jobNumber);
  if (j == NULL) {
    fprintf(stderr, "no such job\n");
    return;
  }

  if (kill(-j->job->pgid, signalNumber) < 0) {
    perror("kill");
  }
}

void overkill(process *proc) {
  if (proc->n_tokens != 1) {
    fprintf(stderr, "usage: overkill\n");
    return;
  }

  job_entry *j = shell_state.job_table;
  while (j) {
    kill(-j->job->pgid, SIGKILL);
    j = j->next;
  }
}
