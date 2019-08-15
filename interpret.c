#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "interpret.h"
#include "main.h"
#include "parse.h"

extern struct state shell_state;

void interpret() {
  if (shell_state.n_tok == 0)
    return;

  if (search_builtin(shell_state.tokens, shell_state.n_tok))
    return;

  printf("command not found: %s\n", shell_state.tokens[0]);
}
