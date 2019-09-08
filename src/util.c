#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "main.h"
#include "util.h"

extern struct state shell_state;

void insert_job(job *j) {
  if (j == NULL)
    return;
  job_entry *ent = calloc(1, sizeof(job_entry));
  ent->job = j;
  ent->next = shell_state.job_table;
  shell_state.job_table = ent;
}

void print_job_table() {
  job_entry *cur = shell_state.job_table;
  printf("job table: ");
  while (cur) {
    debug_print(cur->job);
    cur = cur->next;
  }
}

/* status is obtained from waitpid */
int update_status(pid_t pid, int status) {
  if (pid == 0)
    return -1;
  else if (pid < 0)
    return -1;

  for (job_entry *j = shell_state.job_table; j; j = j->next) {
    for (process *proc = j->job->first_process; proc;
         proc = proc->next_process) {
      if (proc->pid == pid) {
        proc->status = status;
        if (WIFSTOPPED(status))
          proc->stopped = 1;
        else {
          proc->completed = 1;

          if (!j->job->fg) {
            fprintf(stderr, "%s with pid %d exited\n", proc->argv[0],
                    proc->pid);
            if (WIFSIGNALED(status))
              fprintf(stderr, "pid %d terminated by signal\n", proc->pid);
          }
        }
        return 0;
      }
    }
  }
  return -1;
}

void wait_for_job(job *j) {
  while (1) {
    int fl = 0;
    for (process *p = j->first_process; p; p = p->next_process) {
      if (!(p->stopped || p->completed)) {
        fl = 1;
        break;
      }
    }
    if (!fl)
      break;
  }
}
