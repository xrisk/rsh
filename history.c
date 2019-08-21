#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "history.h"
#include "main.h"

extern struct state shell_state;

void initialize_history() {
  for (int i = 0; i < MAX_HISTORY; i++)
    shell_state.history[i] = NULL;
}

void add_history_entry(char *line) {

  int new_index = (shell_state.head - 1) % MAX_HISTORY;
  if (new_index < 0)
    new_index += MAX_HISTORY;

  if (shell_state.history[new_index] != NULL)
    free(shell_state.history[new_index]);

  shell_state.history[new_index] = strdup(line);
  shell_state.head = new_index;
}

void show_history() {

  int idx = shell_state.head;
  int ctr = 1;

  do {
    if (shell_state.history[idx] == NULL)
      break;
    printf("%d: %s\n", ctr++, shell_state.history[idx]);
    idx = (idx + 1) % MAX_HISTORY;
  } while (idx != shell_state.head);
}
