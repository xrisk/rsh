
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

struct state shell_state;

#define LINE_SIZE (1 * 1024 * 1024)
/* 1M line size seems good */

void free_tokens(void) {
  if (shell_state.tokens != NULL) {
    for (size_t i = 0; i < shell_state.n_tok; ++i) {
      if (shell_state.tokens[i] != NULL) {
        free(shell_state.tokens[i]);
        shell_state.tokens[i] = NULL;
      }
    }
    free(shell_state.tokens);
    shell_state.tokens = NULL;
    shell_state.n_tok = 0;
  }
}

void free_subcommands(void) {
  if (shell_state.subcommands != NULL) {
    for (size_t i = 0; i < shell_state.n_subcommands; ++i) {
      if (shell_state.subcommands[i] != NULL) {
        free(shell_state.subcommands[i]);
        shell_state.subcommands[i] = NULL;
      }
    }
    free(shell_state.subcommands);
    shell_state.subcommands = NULL;
    shell_state.n_subcommands = 0;
  }
  shell_state.bg = false;
}

void set_homedir() {
  char *homedir = get_cwd();
  strncpy(shell_state.homedir, homedir, sizeof(shell_state.homedir));
}

void sigchld_handler() {
  int status;
  bool f = false;

  do {
    f = false;
    pid_t ch = waitpid(-1, &status, WNOHANG);
    if (ch == -1) {
      return;
    }
    if (ch == 0)
      return;

    if (ch == shell_state.fg_pid) {
      shell_state.fg_pid = -1;
#ifdef DEBUG
      printf("fg ended\n");
#endif

#ifndef DEBUG
      continue;
#endif
    }

    f = true;
    fprintf(stderr, "pid %d exited", ch);
    if (WIFEXITED(status))
      fprintf(stderr, " with return status %d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
      fprintf(stderr, " because of signal %d\n", WTERMSIG(status));
    else
      fprintf(stderr, " abnormally reason unknown\n");
  } while (f);
}

void initialize() {
  /* https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html#Initializing-the-Shell
   */

  shell_state.shell_terminal = STDIN_FILENO;
  bool shell_is_interactive = isatty(shell_state.shell_terminal);

  if (shell_is_interactive) {

    while (tcgetpgrp(shell_state.shell_terminal) !=
           (shell_state.shell_pgid = getpgrp()))
      kill(-shell_state.shell_pgid, SIGTTIN);

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

    tcsetpgrp(shell_state.shell_terminal, shell_pgid);
    tcgetattr(shell_state.shell_terminal, &shell_state.shell_tmodes);

    initialize_history();
  } else {
    fprintf(stderr, "stdin is not a tty; exiting!\n");
    exit(1);
  }
}

void cleanup() {
  free_history();
  tcsetattr(shell_state.shell_terminal, TCSADRAIN, &shell_state.shell_tmodes);
}

int main() {

  char *line = NULL;
  size_t line_sz = 0;

  initialize();

  set_homedir();

  while (1) {
    show_prompt();

    signal(SIGCHLD, sigchld_handler);
    if (getline(&line, &line_sz, stdin) < 0) {
      if (errno == EAGAIN)
        continue;
      goto end;
    }

    line[strcspn(line, "\n")] = '\0';
    split_into_subcommands(line);

    for (size_t i = 0; i < shell_state.n_subcommands; ++i) {
      parse_subcommand(shell_state.subcommands[i]);
      if (interpret() == QUIT_NOW) {
        goto end;
      }
      free_tokens();
    }

    add_history_entry(line);
    free_subcommands();
  }

end:
  free_tokens();
  free_subcommands();
  if (line != NULL) {
    free(line);
    line = NULL;
  }

  cleanup();
  return 0;
}
