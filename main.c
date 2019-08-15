#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "interpret.h"
#include "main.h"
#include "parse.h"
#include "prompt.h"

struct state shell_state;

#define LINE_SIZE (1 * 1024 * 1024)
/* 1M line size seems good */

void free_line(void) {
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
}

void set_homedir() {
  char *homedir = get_cwd();
  strncpy(shell_state.homedir, homedir, sizeof(shell_state.homedir));
}

int main() {

  set_homedir();

  char *line = calloc(1, sizeof(char));
  while (1) {
    show_prompt();
    if (fgets(line, LINE_SIZE, stdin) == NULL)
      break;
    line[strcspn(line, "\n")] = '\0';
    split_into_subcommands(line);

    for (size_t i = 0; i < shell_state.n_subcommands; ++i) {
      parse_subcommand(shell_state.subcommands[i]);
      interpret();
    }
    free_line();
  }
}
