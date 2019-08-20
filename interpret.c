#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "external.h"
#include "interpret.h"
#include "main.h"
#include "parse.h"

extern struct state shell_state;

void interpret() {
  if (shell_state.n_tok == 0)
    return;

#ifdef DEBUG
  fprintf(stderr, "ntok: %zu bg: %s parsed: ", shell_state.n_tok,
          shell_state.bg ? "yes" : "no");
  for (size_t i = 0; i < shell_state.n_tok; ++i) {
    fprintf(stderr, "%s ", shell_state.tokens[i]);
  }
  fprintf(stderr, "\n");
#endif

  if (search_builtin(shell_state.tokens, shell_state.n_tok))
    return;

  search_external_cmd(shell_state.tokens, shell_state.bg);
  return;

  /*printf("command not found: %s\n", shell_state.tokens[0]);*/
}
