#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cronjob.h"
#include "external.h"
#include "main.h"

extern struct state shell_state;

void handler() {

  pid_t pid;

  shell_state.alarm_duration -= shell_state.alarm_sleep;

  if (shell_state.alarm_duration < 0)
    return;

  process proc;
  memset(&proc, 0, sizeof(proc));
  proc.argv = shell_state.alarm_argv;
  proc.n_tokens = 0;

  while (1) {
    if (proc.argv[proc.n_tokens] == NULL)
      break;
    proc.n_tokens++;
  }

  switch (pid = fork()) {
  case -1:
    perror("fork");
    break;
  case 0:
    launch_process(&proc, 0, open("/dev/null", O_RDONLY), STDOUT_FILENO, 0);
    break;
  default:
    setpgid(pid, pid);
    alarm(shell_state.alarm_sleep);
  }
}

void cronjob(process *proc) {

  int time = -1;
  int duration = -1;

  char *t1 = NULL, *t2 = NULL;

  char **argv = calloc(proc->n_tokens, sizeof(char *));
  int idx = 0;

  for (size_t i = 0; i < proc->n_tokens; i++) {
    if (strcmp(proc->argv[i], "-t") == 0) {
      if (i + 1 != proc->n_tokens) {
        t1 = proc->argv[i + 1];
        i++;
        continue;
      }
    } else if (strcmp(proc->argv[i], "-p") == 0) {
      if (i + 1 != proc->n_tokens) {
        t2 = proc->argv[i + 1];
        i++;
        continue;
      }
    }
  }

  for (size_t i = 0; i < proc->n_tokens; i++) {
    if (strcmp(proc->argv[i], "-c") == 0) {
      i++;
      while (i < proc->n_tokens) {
        if (strcmp(proc->argv[i], "-t") == 0 ||
            strcmp(proc->argv[i], "-p") == 0)
          break;
        argv[idx++] = strdup(proc->argv[i]);
        i++;
      }
    }
  }

  if (t1 == NULL || t2 == NULL) {
    fprintf(stderr, "usage: cronjob -c <command> -t <period> -p <duration>\n");
    return;
  }

  char *endptr;
  time = strtol(t1, &endptr, 10);
  if (endptr == t1) {
    printf("invalid period\n");
    return;
  }
  duration = strtol(t2, &endptr, 10);
  if (endptr == t2) {
    printf("invalid duration\n");
    return;
  }

  shell_state.alarm_sleep = time;
  shell_state.alarm_duration = duration;
  shell_state.alarm_argv = argv;

  signal(SIGALRM, handler);

  alarm(shell_state.alarm_sleep);
}
