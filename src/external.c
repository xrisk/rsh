#include <stdbool.h>
#define _GNU_SOURCE
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin.h"
#include "external.h"
#include "main.h"
#include "util.h"

extern struct state shell_state;

/* https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html#Launching-Jobs
 */
void launch_process(process *proc, pid_t pgid, int infile, int outfile,
                    bool fg) {
  pid_t pid = getpid();

  if (pgid == 0)
    pgid = pid;
  setpgid(pid, pgid);

  if (fg) {
    tcsetpgrp(shell_state.shell_terminal, pgid);
  }

  signal(SIGINT, SIG_DFL);
  signal(SIGQUIT, SIG_DFL);
  signal(SIGTSTP, SIG_DFL);
  signal(SIGTTIN, SIG_DFL);
  signal(SIGTTOU, SIG_DFL);
  signal(SIGCHLD, SIG_DFL);

  if (proc->infile != NULL) {
    int fd = open(proc->infile, O_RDONLY);
    if (fd < 0) {
      perror("open");
      _exit(1);
    }
    infile = fd;
  }

  if (infile != STDIN_FILENO) {
    dup2(infile, STDIN_FILENO);
    close(infile);
  }

  if (proc->outfile != NULL) {
    outfile = open(proc->outfile, O_WRONLY | O_TRUNC | O_CREAT, 0644);
  }

  if (outfile != STDOUT_FILENO) {
    dup2(outfile, STDOUT_FILENO);
    close(outfile);
  }

  if (search_builtin(proc))
    _exit(0);

  if (execvp(proc->argv[0], proc->argv) < 0) {
    perror("execvp");
    _exit(1);
  }
}

void put_job_to_fg(job *j, int cont) {

  tcsetpgrp(shell_state.shell_terminal, j->pgid);
  if (cont) {
    kill(j->pgid, SIGCONT);
  }

  wait_for_job(j);
  tcsetpgrp(shell_state.shell_terminal, shell_state.shell_pgid);
}

void put_job_to_bg(job *j, int cont) {
  if (cont)
    kill(j->pgid, SIGCONT);
}

void launch_job(job *j, int fg) {
  int mypipe[2], infile, outfile;
  pid_t pid;

  process *proc;

  for (proc = j->first_process; proc; proc = proc->next_process) {
    if (proc->n_tokens == 0) {
      fprintf(stderr, "empty process in job!\n");
      return;
    }
  }

  if (!j->first_process->next_process) {
    if (search_builtin(j->first_process))
      return;
  }

  sigsetmask(sigmask(SIGCHLD));

  infile = STDIN_FILENO;
  outfile = STDOUT_FILENO;

  for (proc = j->first_process; proc != NULL; proc = proc->next_process) {
    if (proc->next_process) {

      pipe(mypipe);
      outfile = mypipe[1];
    }

    switch (pid = fork()) {
    case -1:
      perror("fork");
      exit(1);
    case 0:
      launch_process(proc, j->pgid, infile, outfile, fg);
      break;
    default:
      if (j->pgid == 0)
        j->pgid = pid;
      /* printf("setpgid(%d, %d)\n", proc->pid, j->pgid); */
      proc->pid = pid;
      setpgid(proc->pid, j->pgid);
      break;
    }
    if (infile != STDIN_FILENO)
      close(infile);
    if (outfile != STDOUT_FILENO)
      close(outfile);
    infile = mypipe[0];
  }

  insert_job(j);
  sigsetmask(0);

  if (fg) {
    put_job_to_fg(j, 0);
  } else
    put_job_to_bg(j, 0);
}
