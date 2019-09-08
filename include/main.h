#ifndef MAIN_H
#define MAIN_H

#define MAX_PATH_LEN 4096
#define MAX_HISTORY 20

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <termios.h>

#define QUIT_NOW 1337
#define JOB_MAX 1024

/* https://www.gnu.org/software/libc/manual/html_node/Data-Structures.html#Data-Structures
 */

typedef struct process {
  struct process *next_process; /* next process in pipeline */
  char **argv, *infile, *outfile;
  pid_t pid;
  int completed, stopped, status, n_tokens;
  bool append;
} process;

typedef struct job {
  struct job *next_job;
  process *first_process; /* first proc in job */
  pid_t pgid;
  bool fg;
} job;

typedef struct line {
  job *first_job; /* line is a semi-colon delimited list of jobs */
} line;

typedef struct job_entry {
  job *job;
  struct job_entry *next;
} job_entry;

struct state {
  char cwd[MAX_PATH_LEN], hostname[MAX_PATH_LEN], username[MAX_PATH_LEN],
      shortdir[MAX_PATH_LEN], homedir[MAX_PATH_LEN];

  int shell_terminal;
  int shell_is_interactive;
  pid_t shell_pgid;
  pid_t fg_pid;

  struct termios shell_tmodes;
  char *history[MAX_HISTORY];
  int n_history, head;

  job_entry *job_table;
};

#endif
