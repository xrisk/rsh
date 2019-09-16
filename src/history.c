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

  FILE *f;
  if ((f = fopen("rsh_history", "r")) != NULL) {
    char *line = NULL;
    size_t sz = 0, idx = 0;
    while (idx < MAX_HISTORY && getline(&line, &sz, f) != -1) {
      line[strcspn(line, "\n")] = '\0';
      shell_state.history[idx++] = strdup(line);
    }
    if (line != NULL) {
      free(line);
      line = NULL;
    }
    fclose(f);
  }
}

void add_history_entry(char *line) {

  if (strspn(line, "\t ") == strlen(line))
    return;
  int new_index = (shell_state.head - 1) % MAX_HISTORY;
  if (new_index < 0)
    new_index += MAX_HISTORY;

  if (shell_state.history[new_index] != NULL) {
    free(shell_state.history[new_index]);
    shell_state.history[new_index] = NULL;
  }

  shell_state.history[new_index] = strdup(line);
  shell_state.head = new_index;
}

void show_history(process *p) {
  int idx = shell_state.head;
  int ctr = 1;

  int n = p->n_tokens > 1 ? atoi(p->argv[1]) : 10;

  do {
    if (shell_state.history[idx] == NULL)
      break;
    printf("%d: %s\n", ctr++, shell_state.history[idx]);
    idx = (idx + 1) % MAX_HISTORY;
  } while (idx != (shell_state.head + n) % MAX_HISTORY);
}

void persist_history() {
  FILE *f = fopen("rsh_history", "w");
  if (f == NULL) {
    perror("failed to open history file for writing");
    return;
  }
  int idx = shell_state.head;
  do {
    if (shell_state.history[idx] == NULL)
      break;
    fprintf(f, "%s\n", shell_state.history[idx]);
    idx = (idx + 1) % MAX_HISTORY;
  } while (idx != shell_state.head);
  fclose(f);
}

void free_history() {
  for (int i = 0; i < MAX_HISTORY; i++) {
    if (shell_state.history[i] != NULL) {
      free(shell_state.history[i]);
      shell_state.history[i] = NULL;
    }
  }
}
