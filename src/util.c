#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "main.h"
#include "util.h"

extern struct state shell_state;

void debug_print(job *j) {
  if (j) {
    process *p = j->first_process;
    while (p) {
      printf("(%s) ", (p->stopped) ? "Stopped" : "Running");
      for (size_t i = 0; i < p->n_tokens; ++i) {
        printf("%s ", p->argv[i]);
      }
      if (p->outfile)
        printf((p->append) ? ">>%s " : ">%s ", p->outfile);
      if (p->infile)
        printf("<%s ", p->infile);
      printf(" [%d] ", p->pid);
      if (p->next_process)
        printf("|");
      p = p->next_process;
    }
  }
}

void insert_job(job *j) {
  if (j == NULL)
    return;
  job_entry *ent = calloc(1, sizeof(job_entry));
  ent->job = j;
  ent->next = shell_state.job_table;
  shell_state.job_table = ent;
}

job_entry *get_job(int req) {
  int idx = 1;
  job_entry *cur = shell_state.job_table;
  while (cur != NULL && req != idx) {
    cur = cur->next;
    idx++;
  }

  return cur;
}

bool check_stopped(job *j) {
  for (process *p = j->first_process; p; p = p->next_process) {
    if (!p->stopped)
      return false;
  }
  return true;
}

bool check_completed(job *j) {
  for (process *p = j->first_process; p; p = p->next_process) {
    if (!p->completed)
      return false;
  }
  return true;
}

void prune_jobs() {
  job_entry *j = shell_state.job_table;
  job_entry *jlast = NULL;
  while (j) {
    if (check_completed(j->job)) {
      if (jlast == NULL)
        shell_state.job_table = j->next;
      else
        jlast->next = j->next;
    }
    jlast = j;
    j = j->next;
  }
}

void print_job_table() {
  prune_jobs();
  job_entry *cur = shell_state.job_table;
  int idx = 1;
  while (cur) {
    printf("[%d] ", idx++);
    debug_print(cur->job);
    printf("\n");
    cur = cur->next;
  }
}

/* status is obtained from waitpid */
int update_status(pid_t pid, int status) {
  if (pid == 0)
    return -1;
  else if (pid < 0)
    return -1;

  job_entry *j;

  for (j = shell_state.job_table; j; j = j->next) {
    for (process *proc = j->job->first_process; proc;
         proc = proc->next_process) {
      if (proc->pid == pid) {
        proc->status = status;
        if (WIFSTOPPED(status)) {
          j->job->fg = false;
          proc->stopped = 1;
        } else {
          proc->completed = 1;
          if (!j->job->fg) {
            if (WIFSIGNALED(status))
              fprintf(stderr, "%s with pid %d terminated by signal\n",
                      proc->argv[0], proc->pid);
            else
              fprintf(stderr, "%s with pid %d exited\n", proc->argv[0],
                      proc->pid);
          }
        }
        return 0;
      }
    }
  }
  return -1;
}

void wait_for_job(job *j) {
  while (!(check_stopped(j) || check_completed(j)))
    ;
}
