#include <stdio.h>
#include <stdlib.h>

#include "env.h"
#include "main.h"

void set_environ(process *proc) {
  if (proc->n_tokens < 2 || proc->n_tokens > 3) {
    fprintf(stderr, "usage: setenv var [value]\n");
    return;
  }
  char *var = NULL, *value = NULL;
  var = (proc->argv)[1];
  if (proc->n_tokens == 3)
    value = (proc->argv)[2];
  else
    value = "";
  setenv(var, value, 1);
}

void unset_environ(process *proc) {
  if (proc->n_tokens != 2) {
    fprintf(stderr, "usage: unsetenv val\n");
    return;
  }
  unsetenv((proc->argv)[1]);
}
