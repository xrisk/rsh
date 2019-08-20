#define _GNU_SOURCE

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "main.h"
#include "pinfo.h"

extern struct state shell_state;

void disp_pinfo(pid_t pid) {
  printf("pid -- %d\n", pid);

  char *path;
  asprintf(&path, "/proc/%d/status", pid);
  FILE *f = fopen(path, "r");
  int len = 4096;
  char *buffer = calloc(len, sizeof(char));
  int c, idx = 0;
  while ((c = fgetc(f)) != EOF) {
    if (idx == len) {
      buffer = realloc(buffer, 2 * len);
      len *= 2;
    }
    buffer[idx++] = c;
  }
  if (len == idx) {
    buffer = realloc(buffer, len + 1);
    ++len;
  }
  buffer[idx] = '\0';

  char *ptr = strtok(buffer, "\n");

  char *state;
  char *mem;
  while (ptr != NULL) {
    if (strncmp(ptr, "State", strlen("State")) == 0) {
      ptr += strlen("State:");
      while (*ptr == ' ' || *ptr == '\t')
        ptr++;
      state = strdup(ptr);
    }
    if (strncmp(ptr, "VmSize", strlen("VmSize")) == 0) {
      ptr += strlen("VmSize:");
      while (*ptr == ' ' || *ptr == '\t')
        ptr++;
      mem = strdup(ptr);
    }
    ptr = strtok(NULL, "\n");
  }

  char *tmp, *exe_path = NULL;
  exe_path = calloc(1024, sizeof(char));
  asprintf(&tmp, "/proc/%d/exe", pid);
  if (readlink(tmp, exe_path, 1024) < 0) {
    warn("readlink failed");
  }

  printf("state: %s\nmem: %s\nexe path:%s\n", state, mem, exe_path);
}

void pinfo() {

  pid_t pid;
  if (shell_state.n_tok == 1)
    pid = getpid();
  else
    pid = atoi(shell_state.tokens[1]);

  disp_pinfo(pid);
}
