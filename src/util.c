#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "main.h"
#include "util.h"

extern struct state shell_state;

void debug_print(job *j) {
  if (j) {
    process *p = j->first_process;
    while (p) {
      if (p->stopped)
        printf("(%s) ", "Stopped");
      else if (p->completed)
        printf("(%s) ", "Completed");
      else
        printf("(%s) ", "Running");
      for (size_t i = 0; i < p->n_tokens; ++i) {
        printf("%s ", p->argv[i]);
      }
      if (p->outfile)
        printf((p->append) ? ">>%s " : ">%s ", p->outfile);
      if (p->infile)
        printf("<%s ", p->infile);
      printf(" [%d] ", p->pid);
      if (p->next_process)
        printf("| ");
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

void free_process(process *proc) {
  if (proc == NULL)
    return;
  if (proc->argv) {
    for (int i = 0; i < proc->n_tokens; ++i) {
      free(proc->argv[i]);
      proc->argv[i] = NULL;
    }
    free(proc->argv);
    proc->argv = NULL;
  }
  free(proc->infile);
  free(proc->outfile);
  free(proc);
}

void free_job(job *j) {
  if (j == NULL)
    return;
  process *p = j->first_process;
  while (p) {
    process *tmp = p;
    p = p->next_process;
    free_process(tmp);
  }
  free(j);
}

void free_job_entry(job_entry *ent) {
  free_job(ent->job);
  ent->job = NULL;
  free(ent);
  ent = NULL;
}

void free_job_table(void) {
  job_entry *j = shell_state.job_table;
  while (j) {
    job_entry *t = j;
    j = j->next;
    free_job_entry(t);
  }
}

void prune_jobs(void) {
  job_entry *j = shell_state.job_table;
  job_entry *jlast = NULL, *to_free = NULL;
  while (j) {
    if (check_completed(j->job)) {
      if (jlast == NULL)
        shell_state.job_table = j->next;
      else
        jlast->next = j->next;
      to_free = j;
    } else {
      jlast = j;
    }
    j = j->next;
    if (to_free != NULL) {
      free_job_entry(to_free);
      to_free = NULL;
    }
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
  bool exit = false;
  while (!exit) {
    exit = true;
    for (process *p = j->first_process; p; p = p->next_process) {
      if (!(p->completed || p->stopped)) {
        exit = false;
        break;
      }
    }
    if (exit)
      break;
    sleep(1);
  }
}
