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

void interpret(line *inp) {
  /* if (shell_state.n_tok == 0) */
  /*   return true; */

  /* #ifdef DEBUG */
  /* fprintf(stderr, "ntok: %zu bg: %s parsed: ", shell_state.n_tok, */
  /*         shell_state.bg ? "yes" : "no"); */
  /* for (size_t i = 0; i < shell_state.n_tok; ++i) { */
  /*   fprintf(stderr, "%s ", shell_state.tokens[i]); */
  /* } */
  /* fprintf(stderr, "\n"); */
  /* #endif */

  /* int ret = search_builtin(shell_state.tokens, shell_state.n_tok); */
  /* if (ret == QUIT_NOW) */
  /*   return QUIT_NOW; */
  /* else if (ret == true) */
  /*   return true; */
  /* else */
  /*   return search_external_cmd(shell_state.tokens, shell_state.bg); */

  job *j = inp->first_job;

  while (j) {
    launch_job(j, j->fg);
    j = j->next_job;
  }
}
