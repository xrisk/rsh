#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "history.h"
#include "interpret.h"
#include "main.h"
#include "parse.h"
#include "prompt.h"
#include "util.h"

struct state shell_state;

void set_homedir() {
  char *homedir = get_cwd();
  strncpy(shell_state.homedir, homedir, sizeof(shell_state.homedir));
}

void sigchld_handler() {
  pid_t pid;
  do {
    int status;
    pid = waitpid(-1, &status, WNOHANG | WUNTRACED);
    update_status(pid, status);
  } while (pid > 0);
}

void cleanup() {
  free_job_table();
  persist_history();
  free_history();
  tcsetattr(shell_state.shell_terminal, TCSADRAIN, &shell_state.shell_tmodes);
}

void initialize() {
  /* https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html#Initializing-the-Shell
   */

  shell_state.shell_terminal = STDIN_FILENO;
  bool shell_is_interactive = isatty(shell_state.shell_terminal);

  if (shell_is_interactive) {

    /* while (tcgetpgrp(shell_state.shell_terminal) != */
    /*        (shell_state.shell_pgid = getpgrp())) { */
    /*   printf("%d\n", tcgetpgrp(shell_state.shell_terminal)); */
    /*   kill(-shell_state.shell_pgid, SIGTTIN); */
    /* } */

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    pid_t shell_pgid = getpid();
    if (setpgid(shell_pgid, shell_pgid < 0)) {
      perror("unable to put the shell in it's own process group");
      exit(EXIT_FAILURE);
    }

    shell_state.shell_pgid = shell_pgid;

    tcsetpgrp(shell_state.shell_terminal, shell_pgid);
    tcgetattr(shell_state.shell_terminal, &shell_state.shell_tmodes);

    initialize_history();

    signal(SIGCHLD, sigchld_handler);
    atexit(cleanup);

  } else {
    fprintf(stderr, "stdin is not a tty; exiting!\n");
    exit(EXIT_FAILURE);
  }
}

void glob(char **ptr_to_line) {

  char *line = *ptr_to_line;
  /* line should be allocated on the heap; as realloc will be called on it */
  int tilde_count = 0;
  for (size_t i = 0; i < strlen(line); ++i) {
    if (line[i] == '~')
      tilde_count++;
  }

  if (tilde_count == 0)
    return;
  char *new =
      calloc(strlen(line) + strlen(shell_state.homedir) * tilde_count + 1,
             sizeof(char));

  char *ptr = new;
  for (size_t i = 0; i < strlen(line); ++i) {
    if (line[i] == '~') {
      ptr = stpcpy(ptr, shell_state.homedir);
    } else {
      *ptr = line[i];
      ++ptr;
    }
  }

  *ptr_to_line = realloc(line, strlen(new));
  strcpy(*ptr_to_line, new);
  free(new);
}

int main() {

  char *str = NULL;
  size_t line_sz = 0;

  initialize();

  set_homedir();

  while (1) {
    if (shell_state.quit)
      break;

    show_prompt();
    if (getline(&str, &line_sz, stdin) < 0) {
      if (feof(stdin)) {
        clearerr(stdin);
        printf("\n");
        continue;
      }
      perror("getline");
      exit(1);
    }

    str[strcspn(str, "\n")] = '\0';
    glob(&str);

    line *input_line = parse_line(str);

    interpret(input_line);
    free(input_line);

    add_history_entry(str);
  }

  if (str != NULL) {
    free(str);
    str = NULL;
  }

  return 0;
}
